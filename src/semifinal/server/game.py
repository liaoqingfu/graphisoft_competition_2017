import random


class Player(object):

    def __init__(self, number, field, target, data):
        self.__number = number
        self.__field = field
        self.__points = 0
        self.__target = target
        self.__data = data

    @property
    def number(self):
        return self.__number

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

    @property
    def data(self):
        return self.__data


class MazeGraph(object):

    def __init__(self, maze):
        self.__g = {}
        for y in range(0, len(maze)):
            for x in range(0, len(maze[0])):
                self.__g[(x, y)] = self.__get_neighbors(maze, x, y)

    def __get_neighbors(self, maze, x, y):
        neighbors = []
        maxY = len(maze) - 1
        maxX = len(maze[0]) - 1
        if x > 0 and maze[y][x].field & 1 and maze[y][x-1].field & 4:
            neighbors.append((x-1, y))

        if x < maxX and maze[y][x].field & 4 and maze[y][x+1].field & 1:
            neighbors.append((x+1, y))

        if y > 0 and maze[y][x].field & 2 and maze[y-1][x].field & 8:
            neighbors.append((x, y-1))

        if y < maxY and maze[y][x].field & 8 and maze[y+1][x].field & 2:
            neighbors.append((x, y+1))

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


class Zone(object):

    def __init__(self, field):
        self.__field = field
        self.__display = None
        self.__players = set()

    @property
    def field(self):
        return self.__field

    @field.setter
    def field(self, value):
        self.__field = value

    @property
    def display(self):
        return self.__display

    @display.setter
    def display(self, value):
        assert(value is None and self.__display is not None
               or value is not None and self.__display is None)
        self.__display = value

    @property
    def players(self):
        return self.__players

    def add_player(self, player):
        assert(player not in self.__players)
        self.__players.add(player)

    def remove_player(self, player):
        assert(player in self.__players)
        self.__players.remove(player)


class Maze(object):

    def __init__(self, seed=None):
        self.__seed = random.seed(seed)
        m, n = random.randint(6, 15), random.randint(6, 15)
        self.__zones = []
        for i in range(0, n):
            row = []
            for j in range(0, m):
                row.append(Zone(random.randint(1, 15)))
            self.__zones.append(row)
        self.__starting_number_of_displays = random.randint(4, int(m * n / 4))
        self.__displays_found = 0
        for i in range(0, self.__starting_number_of_displays):
            x, y = random.randint(0, m - 1), random.randint(0, n - 1)
            if self.__zones[y][x].display is None:
                self.__zones[y][x].display = i
        self.__max_tick = random.randint(3, 15)

    @property
    def max_tick(self):
        return self.__max_tick

    def add_player(self, player_number):
        x = random.randint(0, len(self.__zones[0]) - 1)
        y = random.randint(0, len(self.__zones) - 1)
        self.__zones[y][x].add_player(player_number)

    def push(self, is_col, is_positive, number, field, orig_field):
        assert(self.__are_fields_equivalent(field, orig_field))
        if not is_col and is_positive:
            return self.__push_row_positive(number, field)
        elif not is_col:
            return self.__push_row_negative(number, field)
        elif is_col and is_positive:
            return self.__push_col_positive(number, field)
        else:
            return self.__push_col_negative(number, field)

    def __are_fields_equivalent(self, new_field, orig_field):
        equivalence_classes = [
            [1, 2, 4, 8], [3, 6, 9, 12], [7, 11, 13, 14], [5, 10], [15]
        ]
        for eq_class in equivalence_classes:
            if orig_field in eq_class:
                return new_field in eq_class
        assert(False)

    def __push_row_positive(self, row_num, field):
        row = self.__zones[row_num]
        zone = row.pop()
        remaining_field = zone.field
        zone.field = field
        row.insert(0, zone)
        return remaining_field

    def __push_row_negative(self, row_num, field):
        row = self.__zones[row_num]
        zone = row[0]
        row = row[1:]
        remaining_field = zone.field
        zone.field = field
        row.append(zone)
        self.__zones[row_num] = row
        return remaining_field

    def __push_col_positive(self, col_num, field):
        num_rows = len(self.__zones)
        zone = self.__zones[num_rows-1][col_num]
        remaining_field = zone.field
        zone.field = field
        for i in reversed(range(1, num_rows)):
            self.__zones[i][col_num] = self.__zones[i-1][col_num]
        self.__zones[0][col_num] = zone
        return remaining_field

    def __push_col_negative(self, col_num, field):
        num_rows = len(self.__zones)
        zone = self.__zones[0][col_num]
        remaining_field = zone.field
        zone.field = field
        for i in range(0, num_rows - 1):
            self.__zones[i][col_num] = self.__zones[i+1][col_num]
        self.__zones[num_rows-1][col_num] = zone
        return remaining_field

    def goto(self, player_number, target):
        position = None
        for y in range(0, len(self.__zones)):
            for x in range(0, len(self.__zones[0])):
                if player_number in self.__zones[y][x].players:
                    position = (x, y)
        assert(position is not None)
        g = MazeGraph(self.__zones)
        if g.find_path(position, target) is not None:
            self.__zones[position[1]][position[0]].remove_player(player_number)
            target_zone = self.__zones[target[1]][target[0]]
            target_zone.add_player(player_number)
            return target_zone.display is not None
        return None

    def get_display_position(self, display_number):
        for y in range(0, len(self.__zones)):
            for x in range(0, len(self.__zones[0])):
                if self.__zones[y][x].display == display_number:
                    return (x, y)
        assert(False)

    def get_next_display(self, current_display_number=None):
        if current_display_number is not None:
            for y in range(0, len(self.__zones)):
                for x in range(0, len(self.__zones[0])):
                    if self.__zones[y][x].display == current_display_number:
                        self.__zones[y][x].display = None
            self.__displays_found += 1
        skip = random.randint(0, self.__starting_number_of_displays -
                              self.__displays_found - 1)
        for y in range(0, len(self.__zones)):
            for x in range(0, len(self.__zones[0])):
                if self.__zones[y][x].display is not None:
                    if skip == 0:
                        return self.__zones[y][x].display
                    else:
                        skip -= 1
        return None

    def get_info_as_str(self, level, player_number):
        lines = [
            'LEVEL {}'.format(level),
            'SIZE {} {}'.format(len(self.__zones[0]), len(self.__zones)),
            'DISPLAYS {}'.format(self.__starting_number_of_displays),
            'PLAYER {}'.format(player_number),
            'MAXTICK {}'.format(self.__max_tick),
            ''  # needed for the last newline
        ]
        return '\n'.join(lines)

    def __str__(self):
        fieldlist = []
        for row in self.__zones:
            for zone in row:
                fieldlist.append(zone.field)
        fields = [
            'FIELDS {}'.format(str(fieldlist)[1:-1]),
        ]
        displays = []
        for y in range(0, len(self.__zones)):
            for x in range(0, len(self.__zones[0])):
                zone = self.__zones[y][x]
                if zone.display is not None:
                    displays.append('DISPLAY {} {} {}'.format(
                        zone.display, x, y))
        displays = sorted(displays, key=lambda d: int(d.split(' ')[1]))
        positions = []
        for y in range(0, len(self.__zones)):
            for x in range(0, len(self.__zones[0])):
                for player in self.__zones[y][x].players:
                    positions.append('POSITION {} {} {}'.format(player, x, y))
        positions = sorted(positions, key=lambda p: int(p.split(' ')[1]))
        lines = fields + displays + positions + ['']
        return '\n'.join(lines)
