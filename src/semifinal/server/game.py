import itertools


class Player(object):

    def __init__(self, number, pos, field, target):
        self.__number = number
        self.__pos = pos
        self.__field = field
        self.__points = 0
        self.__target = target

    @property
    def number(self):
        return self.__number

    @property
    def position(self):
        return self.__pos

    @property
    def field(self):
        return self.__field

    @property
    def points(self):
        return self.__points

    @property
    def target(self):
        return self.__target

    def add_point(self, new_target):
        self.__points += 1
        self.__target = new_target


class MazeGraph(object):

    def __init__(self, maze):
        self.__g = {}
        for i in range(1, len(maze)):
            for j in range(1, len(maze[0])):
                self.__g[(i, j)] = self.__get_neighbors(maze, i, j)

    def __get_neighbors(self, maze, i, j, point):
        neighbors = []
        maxI = len(maze)
        maxJ = len(maze[0])
        if i > 0 and maze[i][j] & 1 and maze[i - 1][j] & 4:
            neighbors.append((i - 1, j))

        if i < maxI and maze[i][j] & 4 and maze[i + 1][j] & 1:
            neighbors.append((i + 1, j))

        if j > 0 and maze[i][j] & 2 and maze[i][j - 1] & 8:
            neighbors.append((i, j - 1))

        if j < maxJ and maze[i][j] & 8 and maze[i][j + 1] & 2:
            neighbors.append((i, j + 1))

        return neighbors

    def find_path(self, start, end, path=[]):
        path = path + [start]
        if start == end:
            return path
        if start not in self.__g:
            return None
        for node in self.__g[start]:
            if node not in path:
                newpath = self.find_path(node, end, path)
                if newpath:
                    return newpath
        return None


class Maze(object):

    FIELDS = [
        [4, 6, 7, 10, 5],
        [6, 7, 2, 14, 2],
        [13, 8, 2, 4, 10],
        [6, 5, 2, 1, 11],
        [10, 7, 2, 2, 1]
    ]

    def __init__(self):
        self.__fields = Maze.FIELDS
        self.__displays = {0: (2, 2), 1: (5, 5), 2: (4, 2)}

    def get_info_as_str(self, level, player_number, max_tick):
        lines = [
            'LEVEL {}'.format(level),
            'SIZE 5 5',
            'DISPLAYS {}'.format(len(self.__displays)),
            'PLAYER {}'.format(player_number),
            'MAXTICK {}'.format(max_tick),
            ''  # needed for the last newline
        ]
        return '\n'.join(lines)

    def __str__(self):
        lines = [
            'FIELDS {}'.format(str(list(
                itertools.chain(*self.__fields)))[1:-1]),
        ]
        for display in self.__displays:
            lines.append('DISPLAY {} {} {}'.format(
                display, self.__displays[display][0],
                self.__displays[display][1]))
        # for player in self.__players:
        #     lines.append('POSITION {} {} {}'.format(
        #         player, self.__players[player].position[0],
        #         self.__players[player].position[1]))
        lines.append('')
        return '\n'.join(lines)
