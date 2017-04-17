#!/usr/bin/env python3
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
                self.__receive_cb(client_id, message, sender)
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
    __MAX_TICK = 5  # FIXME: hardcoded
    __STARTING_POSITION = (0, 0)  # FIXME: hardcoded
    __STARTING_FIELD = 15
    __STARTING_DISPLAY = 0  # FIXME: hardcoded

    def __init__(self, number_of_players):
        self.__NUMBER_OF_PLAYERS = number_of_players
        self.__maze = Maze()
        self.__players = {}
        self.__player_turn = 0

    def add_player(self, auth_msg, message_sender):
        # TODO: check auth_msg
        auth_reply = AuthReply(AuthReply.accepted)
        player_number = len(self.__players)
        self.__players[player_number] = (
            Player(player_number, GameController.__STARTING_POSITION,
                   GameController.__STARTING_FIELD,
                   GameController.__STARTING_DISPLAY),
            message_sender)
        assert(len(self.__players) <= self.__NUMBER_OF_PLAYERS)
        game_info = self.__maze.get_info_as_str(
            GameController.__LEVEL, player_number, GameController.__MAX_TICK)
        yield from message_sender(str(auth_reply) + game_info)
        if len(self.__players) == self.__NUMBER_OF_PLAYERS:
            yield from self.__next_player_turn()  # TODO: start game
        return player_number

    def player_step_requested(self, client_id, message, message_sender):
        print(message)
        req = PlayerStepRequest(message)
        print(req)

    def __next_player_turn(self):
        logging.debug('next player turn: {}'.format(self.__player_turn))
        for player_number in self.__players:
            positions = {}
            for player_number in self.__players:
                positions[player_number] = self.__players[
                    player_number][0].position
            step_msg = PlayerStepMsg(positions, self.__player_turn)
            (player, message_sender) = self.__players[player_number]
            if player_number == self.__player_turn:
                step_msg.target = player.target
                step_msg.field = player.field
            msg = str(self.__maze) + str(step_msg)
            yield from message_sender(msg)
            self.__player_turn = (self.__player_turn + 1) % len(self.__players)


def main():
    logging.basicConfig(level=logging.DEBUG)
    IP_ADDRESS, TCP_PORT = '127.0.0.1', 32323
    loop = asyncio.get_event_loop()
    NUMBER_OF_PLAYERS = 1   # FIXME: hardcoded
    game_controller = GameController(NUMBER_OF_PLAYERS)
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
