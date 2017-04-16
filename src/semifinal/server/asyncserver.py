#!/usr/bin/env python3
import asyncio
import logging
from collections import namedtuple


class Client(object):

    def __init__(self, num, reader, writer):
        self.__is_registered = False
        self.__num = num
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

    def __init__(self, result, level=0, displays=0, player=0, maxtick=0):
        self.__result = result
        self.__level = level
        self.__displays = displays
        self.__player = player
        self.__maxtick = maxtick

    def __str__(self):
        lines = [
            'MESSAGE {!s}'.format(self.__result)
            ]
        if self.__result == AuthReply.accepted:
            pass  # TODO
        lines.append('')
        return '\n'.join(lines)


def parse_authentication(message):
    parts = message.split('\n')
    assert(parts[0].startswith('LOGIN'))
    assert(parts[1].startswith('LEVEL'))
    login = parts[0].split(' ')
    level = parts[1].split(' ')
    return AuthenticationMessage(login[1], login[2], level[1])


class GameServer(object):

    def __init__(self, loop=None):
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
        num = len(self.__tasks) + 1
        client = Client(num, client_reader, client_writer)
        self.__clients[num] = client
        task = self.__loop.create_task(self.__handle_client(client))
        self.__tasks[task] = (num, client_reader, client_writer)

        def client_done(task):
            logging.info('client is done: {}'.format(task))
            num, _, _ = self.__tasks[task]
            del self.__tasks[task]
            if num in self.__clients:
                del self.__clients[num]  # FIXME: __clients needed?

        task.add_done_callback(client_done)

    @asyncio.coroutine
    def __handle_client(self, client):
        try:
            message = yield from self.__extract_message(client.reader)
            logging.debug('received: {}'.format(message))
            auth_msg = parse_authentication(message)
            client.register(auth_msg.name, auth_msg.password)
            auth_reply = AuthReply('NOT IMPLEMENTED', auth_msg.level)
            yield from self.__send_message(client.writer, auth_reply)
            # TODO: auth_msg.level ?
            while True:
                message = yield from self.__extract_message(client.reader)
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


def main():
    logging.basicConfig(level=logging.DEBUG)
    IP_ADDRESS, TCP_PORT = '127.0.0.1', 32323
    loop = asyncio.get_event_loop()
    server = GameServer(loop)
    try:
        server.start(IP_ADDRESS, TCP_PORT)
        server.run()
    except:
        server.stop()
        loop.close()


if __name__ == '__main__':
    main()
