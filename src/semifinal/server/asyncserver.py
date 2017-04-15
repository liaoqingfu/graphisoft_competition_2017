#!/usr/bin/env python3
import asyncio
import sys
from collections import namedtuple


class Client(object):

    def __init__(self, name, password):
        self.__is_registered = True
        self.__name = name
        self.__password = password

    @property
    def is_registered(self):
        return self.__is_registered

    @property
    def name(self):
        return self.__name

    @property
    def password(self):
        return self.__password


AuthenticationMessage = namedtuple(
    'AuthenticationMessage', ['name', 'password',
                              'level'])


def parse_authentication(message):
    parts = message.split('\n')
    assert(parts[0].startswith('LOGIN'))
    assert(parts[1].startswith('LEVEL'))
    login = parts[0].split(' ')
    level = parts[1].split(' ')
    return AuthenticationMessage(login[1], login[2], level[1])


class GameServer(object):

    def __init__(self):
        self.__server = None
        self.__clients = {}

    def start(self, address, port, loop):
        self.__server = loop.run_until_complete(
            asyncio.streams.start_server(
                self.__accept_client, address, port, loop=loop))

    def stop(self, loop):
        if self.__server is not None:
            self.__server.close()
            loop.run_until_complete(self.__server.wait_closed())
            self.__server = None

    def __accept_client(self, client_reader, client_writer):
        task = asyncio.Task(self.__handle_client(client_reader, client_writer))
        self.__clients[task] = (client_reader, client_writer)

        def client_done(task):
            print('client is done: ', task, file=sys.stderr)
            del self.__clients[task]

        task.add_done_callback(client_done)

    @asyncio.coroutine
    def __handle_client(self, client_reader, client_writer):
        try:
            while True:
                message = yield from self.__extract_message(client_reader)
                print(message)
                auth_msg = parse_authentication(message)
                print(auth_msg)
                # yield from client_writer.drain()
        except EOFError as error:
            print(error)

    @asyncio.coroutine
    def __extract_message(self, client_reader):
        data = ''
        message = ''
        while data != '.\n':
            data = (yield from client_reader.readline()).decode('utf-8')
            if not data:
                raise EOFError('No more data')
            message += data
        return message


def main():
    IP_ADDRESS, TCP_PORT = '127.0.0.1', 32323
    loop = asyncio.get_event_loop()
    server = GameServer()
    try:
        server.start(IP_ADDRESS, TCP_PORT, loop)
        loop.run_forever()
    except:
        server.stop(loop)
        loop.close()


if __name__ == '__main__':
    main()
