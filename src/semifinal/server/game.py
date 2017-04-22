import itertools
import random


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

    @position.setter
    def position(self, value):
        self.__pos = value

    @property
    def field(self):
        return self.__field

    @field.setter
    def field(self, value):
        self.__field = value

    @property
    def points(self):
        return self.__points

    @property
    def target(self):
        return self.__target

    def add_point(self, new_target):
        self.__points += 1
        self.__target = new_target

    def reset_target(self, new_target):
        self.__target = new_target


class MazeGraph(object):

    def __init__(self, maze):
        self.__g = {}
        for i in range(0, len(maze)):
            for j in range(0, len(maze[0])):
                self.__g[(i, j)] = self.__get_neighbors(maze, i, j)

    def __get_neighbors(self, maze, i, j):
        neighbors = []
        maxI = len(maze) - 1
        maxJ = len(maze[0]) - 1
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

    def __init__(self, seed=None):
        self.__seed = random.seed(seed)
        m, n = random.randint(6, 15), random.randint(6, 15)
        self.__fields = []
        for i in range(0, n):
            row = []
            for j in range(0, m):
                row.append(random.randint(1, 15))
            self.__fields.append(row)
        self.__displays = {}
        self.__remaining_displays = set()
        for i in range(0, random.randint(4, int(m * n / 4))):
            self.__displays[i] = (random.randint(0, m - 1),
                                  random.randint(0, n - 1))
            self.__remaining_displays.add(i)
        self.__max_tick = random.randint(3, 15)

    @property
    def max_tick(self):
        return self.__max_tick

    def get_random_position(self):
        return (random.randint(0, len(self.__fields[0])),
                random.randint(0, len(self.__fields)))

    def push(self, is_col, is_positive, number, field, orig_field):
        assert(self.__fields_are_equivalent(field, orig_field))
        if not is_col and is_positive:
            return self.__push_row_positive(number, field)
        elif not is_col:
            return self.__push_row_negative(number, field)
        elif is_col and is_positive:
            return self.__push_col_positive(number, field)
        else:
            return self.__push_col_negative(number, field)

    def __fields_are_equivalent(self, new_field, orig_field):
        equivalent_classes = [
            [1, 2, 4, 8], [3, 6, 9, 12], [7, 11, 13, 14], [5, 10], [15]
        ]
        for eq_class in equivalent_classes:
            if orig_field in eq_class:
                return new_field in eq_class
        assert(False)

    def __push_row_positive(self, row_num, field):
        row = self.__fields[row_num]
        row.insert(0, field)
        return row.pop()

    def __push_row_negative(self, row_num, field):
        row = self.__fields[row_num]
        row.append(field)
        remaining_field = row[0]
        self.__fields[row_num] = row[1:]
        return remaining_field

    def __push_col_positive(self, col_num, field):
        num_rows = len(self.__fields)
        remaining_field = self.__fields[num_rows-1][col_num]
        for i in reversed(range(1, num_rows)):
            self.__fields[i][col_num] = self.__fields[i-1][col_num]
        self.__fields[0][col_num] = field
        return remaining_field

    def __push_col_negative(self, col_num, field):
        num_rows = len(self.__fields)
        remaining_field = self.__fields[0][col_num]
        for i in range(0, num_rows - 1):
            self.__fields[i][col_num] = self.__fields[i+1][col_num]
        self.__fields[num_rows-1][col_num] = field
        return remaining_field

    def goto(self, position, target):
        g = MazeGraph(self.__fields)
        if g.find_path(position, target) is not None:
            return True
        return False

    def get_display_position(self, display_number):
        return self.__displays[display_number]

    def get_next_display(self, current_display_number=None):
        if current_display_number is not None:
            self.__remaining_displays.remove(current_display_number)
        if len(self.__remaining_displays) > 0:
            return random.choice(list(self.__remaining_displays))
        return None

    def get_info_as_str(self, level, player_number, max_tick):
        lines = [
            'LEVEL {}'.format(level),
            'SIZE {} {}'.format(len(self.__fields[0]), len(self.__fields)),
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
        lines.append('')
        return '\n'.join(lines)
