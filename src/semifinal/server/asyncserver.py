#!/usr/bin/env python3
import argparse
import asyncio
import logging
from collections import namedtuple
from functools import partial
from game import Player, Maze


class Client(object):

    def __init__(self, player, reader, writer):
        self.__is_registered = False
        self.__player = player
        self.__reader = reader
        self.__writer = writer

    @property
    def is_registered(self):
        return self.__is_registered

    def register(self, name, password):
        self.__is_registered = True
        self.__name = name
        self.__password = password

    @property
    def name(self):
        return self.__name

    @property
    def password(self):
        return self.__password

    @property
    def reader(self):
        return self.__reader

    @property
    def writer(self):
        return self.__writer


AuthenticationMessage = namedtuple(
    'AuthenticationMessage', ['name', 'password', 'level'])


class AuthReply(object):

    accepted = 'OK'

    def __init__(self, result):
        self.__result = result

    def __str__(self):
        return 'MESSAGE {!s}\n'.format(self.__result)


class PlayerStepMsg(object):

    def __init__(self, positions, player_turn, target=None, field=None):
        self.positions = positions
        self.player_turn = player_turn
        self.target = target
        self.field = field

    def __str__(self):
        lines = [
            'POSITION {} {} {}'.format(player,
                                       self.positions[player][0],
                                       self.positions[player][1])
            for player in self.positions
        ]
        lines.append('PLAYER {}'.format(self.player_turn))
        if self.target is not None:
            assert(self.field is not None)
            lines.extend([
                'MESSAGE OK',
                'TARGET {}'.format(self.target),
                'EXTRAFIELD {}'.format(self.field),
            ])
        lines.append('')
        return '\n'.join(lines)


class PlayerStepRequest(object):

    def __init__(self, message):
        parts = message.split('\n')
        assert(parts[0].startswith('PUSH'))
        assert(len(parts) == 1 or parts[1].startswith('GOTO'))
        goto = parts[0].split(' ')
        _, self.is_col, self.is_positive, self.number, self.field = goto
        self.is_col = self.is_col == '1'
        self.is_positive = self.is_positive == '1'
        self.number = int(self.number)
        self.field = int(self.field)
        if len(parts) > 1 and parts[1].startswith('GOTO'):
            _, self.x, self.y = parts[1].split(' ')


class EndGameMessage(object):

    def __init__(self, points):
        self.__points = points

    def __str__(self):
        return 'END {}\n'.format(self.__points)


def parse_authentication(message):
    parts = message.split('\n')
    assert(parts[0].startswith('LOGIN'))
    assert(parts[1].startswith('LEVEL'))
    login = parts[0].split(' ')
    level = parts[1].split(' ')
    return AuthenticationMessage(login[1], login[2], level[1])


class GameServer(object):

    def __init__(self, connect_cb, receive_cb, loop=None):
        self.__connect_cb = connect_cb
        self.__receive_cb = receive_cb
        self.__loop = loop if loop is not None else asyncio.get_event_loop()
        if loop is None:
            self.__loop = asyncio.get_event_loop()
        self.__server = None
        self.__clients = {}
        self.__tasks = {}

    def start(self, address, port):
        self.__server = self.__loop.run_until_complete(
            asyncio.streams.start_server(
                self.__accept_client, address, port, loop=self.__loop))

    def stop(self):
        if self.__server is not None:
            self.__server.close()
            self.__loop.run_until_complete(self.__server.wait_closed())
            self.__server = None

    def run(self):
        self.__loop.run_forever()

    def __accept_client(self, client_reader, client_writer):
        player = len(self.__tasks) + 1
        client = Client(player, client_reader, client_writer)
        self.__clients[player] = client
        task = self.__loop.create_task(self.__handle_client(client))
        self.__tasks[task] = (player, client_reader, client_writer)

        def client_done(task):
            logging.info('client is done: {}'.format(task))
            player, _, _ = self.__tasks[task]
            del self.__tasks[task]
            if player in self.__clients:
                del self.__clients[player]  # FIXME: __clients needed?

        task.add_done_callback(client_done)

    @asyncio.coroutine
    def __handle_client(self, client):
        try:
            message = yield from self.__extract_message(client.reader)
            logging.debug('received: {}'.format(message))
            auth_msg = parse_authentication(message)
            client.register(auth_msg.name, auth_msg.password)
            # auth_reply = AuthReply(AuthReply.accepted)
            # yield from self.__send_message(client.writer, auth_reply)
            sender = partial(GameServer.__send_message, self, client.writer)
            client_id = yield from self.__connect_cb(auth_msg, sender)
            while True:
                message = yield from self.__extract_message(client.reader)
                yield from self.__receive_cb(client_id, message, sender)
        except EOFError as error:
            logging.error(error)

    def __extract_message(self, client_reader):
        data = ''
        message = ''
        while data != '.\n':
            data = (yield from client_reader.readline()).decode('utf-8')
            if not data:
                raise EOFError('No more data')
            message += data
        return message

    def __send_message(self, client_writer, message):
        logging.debug('sending: {}'.format(message))
        MESSAGE_END = '.\n'.encode('utf-8')
        client_writer.write(str(message).encode('utf-8'))
        client_writer.write(MESSAGE_END)
        yield from client_writer.drain()
        logging.debug('message is sent')


class GameController(object):

    __LEVEL = 1  # FIXME: hardcoded
    __STARTING_FIELD = 15

    def __init__(self, number_of_players, seed=None):
        self.__NUMBER_OF_PLAYERS = number_of_players
        self.__seed = seed
        self.__reset(self.__seed)

    def __reset(self, seed):
        self.__maze = Maze(seed)
        self.__players = {}
        self.__player_turn = 0
        self.__tick = 0

    def add_player(self, auth_msg, message_sender):
        # TODO: check auth_msg
        auth_reply = AuthReply(AuthReply.accepted)
        player_number = len(self.__players)
        self.__players[player_number] = (
            Player(player_number, self.__maze.get_random_position(),
                   GameController.__STARTING_FIELD,
                   self.__maze.get_next_display()),
            message_sender)
        assert(len(self.__players) <= self.__NUMBER_OF_PLAYERS)
        game_info = self.__maze.get_info_as_str(
            GameController.__LEVEL, player_number, self.__maze.max_tick)
        yield from message_sender(str(auth_reply) + game_info)
        if len(self.__players) == self.__NUMBER_OF_PLAYERS:
            yield from self.__next_player_turn()
        return player_number

    def player_step_requested(self, client_id, message, message_sender):
        assert(client_id == self.__player_turn)
        print(message)
        req = PlayerStepRequest(message)
        print(req.__dict__)
        print(self.__maze)
        player = self.__players[client_id][0]
        field = self.__maze.push(req.is_col, req.is_positive, req.number,
                                 req.field, player.field)
        player.field = field
        print(self.__maze)
        new_position = (int(req.x), int(req.y))
        if self.__maze.goto(player.position, new_position) is not None:
            player.position = new_position
            print(self.__maze.get_display_position(player.target))
            print(new_position)
            if self.__maze.get_display_position(player.target) == \
               new_position:
                for player_number in self.__players:
                    other_player = self.__players[player_number][0]
                    if other_player.target == player.target:
                        other_player.reset_target(
                            self.__maze.get_next_display())
                next_target = self.__maze.get_next_display(player.target)
                player.add_point(next_target)
                if next_target is None:
                    yield from self.__end_game()
                    return
        self.__player_turn += 1
        if self.__player_turn == len(self.__players):
            self.__tick += 1
            self.__player_turn = 0
        if self.__tick <= self.__maze.max_tick:
            yield from self.__next_player_turn()
        else:
            yield from self.__end_game()

    def __next_player_turn(self):
        logging.debug('next player turn: {}'.format(self.__player_turn))
        if self.__tick > self.__maze.max_tick:
            return
        positions = {}
        for player_number in self.__players:
            positions[player_number] = self.__players[
                player_number][0].position
        for player_number in self.__players:
            step_msg = PlayerStepMsg(positions, self.__player_turn)
            player, message_sender = self.__players[player_number]
            if player_number == self.__player_turn:
                step_msg.target = player.target
                step_msg.field = player.field
            msg = str(self.__maze) + str(step_msg)
            yield from message_sender(msg)
        # TODO: add timer, do not wait forever for the client

    def __end_game(self):
        for player_number in self.__players:
            player, sender = self.__players[player_number]
            msg = EndGameMessage(player.points)
            yield from sender(str(msg))
        self.__reset(self.__seed)


def parse_args():
    parser = argparse.ArgumentParser(description='Semifinal Server')
    parser.add_argument('-s', '--seed', help='Random seed to use',
                        metavar='seed', type=int)
    parser.add_argument('-p', '--players', help='Number of players',
                        metavar='players', type=int, default=1)
    args = parser.parse_args()
    return args


def main():
    logging.basicConfig(level=logging.DEBUG)
    IP_ADDRESS, TCP_PORT = '127.0.0.1', 32323
    args = parse_args()
    loop = asyncio.get_event_loop()
    game_controller = GameController(args.players, args.seed)
    server = GameServer(
        partial(GameController.add_player, game_controller),
        partial(GameController.player_step_requested, game_controller),
        loop)
    try:
        server.start(IP_ADDRESS, TCP_PORT)
        server.run()
    except:
        server.stop()
        loop.close()


if __name__ == '__main__':
    main()
