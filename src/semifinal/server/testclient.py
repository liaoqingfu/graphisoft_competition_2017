#!/usr/bin/env python3
import asyncio


login_lines = [
    'LOGIN asdf asdf',
    'LEVEL 1',
]


step_msg = 'PUSH {} {} {} {}\nGOTO {} {}'


class GameData:

    def __init__(self):
        self.number_of_displays = None
        self.displays = None
        self.target = None

    def get_display_position(self, number_of_display):
        display = self.displays[number_of_display-1]
        return display


def main():

    def read_message(reader):
        message = ''
        data = ''
        while data != '.\n':
            data = (yield from reader.readline()).decode('utf-8')
            if not data:
                raise EOFError('No more data')
            message += data
        return message

    @asyncio.coroutine
    def client(loop):
        print('about to connect...')
        reader, writer = yield from asyncio.streams.open_connection(
            'localhost', 32323, loop=loop)
        print('connected')
        msg = '\n'.join(login_lines) + '\n.\n'
        writer.write(msg.encode('utf-8'))
        yield from writer.drain()
        mazedata = (yield from read_message(reader)).split('\n')[:-2]
        print('maazedata:\n', mazedata)
        game_data = GameData()
        game_data.number_of_displays = int((mazedata[-3].split(' '))[1])
        print('number of displays: ', game_data.number_of_displays)
        while True:
            turninfo = yield from read_message(reader)
            turninfo = turninfo.split('\n')[:-2]
            print('turninfo:\n', turninfo)
            game_data.displays = []
            for disp in turninfo[1:1+game_data.number_of_displays]:
                disp_parts = disp.split(' ')
                game_data.displays.append((disp_parts[1], disp_parts[2]))
            print('displays: ', game_data.displays)
            field = (turninfo[-1].split(' '))[1]
            print('field: {}'.format(field))
            target_parts = turninfo[-2].split(' ')
            target = game_data.get_display_position(int(target_parts[1]))
            print('target: ', target)
            msg = step_msg.format(1, 1, 0, field, target[0], target[1]) +\
                '\n.\n'
            writer.write(msg.encode('utf-8'))
            yield from writer.drain()

    print('hello')
    loop = asyncio.get_event_loop()
    loop.run_until_complete(client(loop))


if __name__ == '__main__':
    main()
