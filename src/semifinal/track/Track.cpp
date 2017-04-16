#include "Track.hpp"

Track::Track(std::size_t width, std::size_t height,
        const std::vector<int>& fieldTypes,
        Monitors monitors, Princesses princesses) :
        fields{width, height},
        monitors(std::move(monitors)),
        princesses(std::move(princesses)) {
    std::transform(fieldTypes.begin(), fieldTypes.end(), fields.begin(),
            [](int type) { return Field{type, false, -1}; });
    for (Point p : this->monitors) {
        fields[p].monitor = true;
    }
    for (std::size_t i = 0; i < this->princesses.size(); ++i) {
        fields[this->princesses[i]].princess = i;
    }
}

void Track::movePrincess(int player, Point target) {
    int& fromPrincess = fields[princesses[player]].princess;
    int& toPrincess = fields[target].princess;
    assert(fromPrincess == player);
    assert(toPrincess == -1);
    // canMove() is not checked: it's expensive

    fromPrincess = -1;
    toPrincess = player;
    princesses[player] = target;
}

void Track::removeMonitor(int id) {
    bool& monitor = fields[monitors[id]].monitor;
    assert(monitor);
    assert(isInsideMatrix(fields, monitors[id]));

    monitor = false;
    monitors[id] = -p11;
}

std::ostream& operator<<(std::ostream& os, const Track& track) {
    os << "  ";
    for (std::size_t i = 0; i < track.width(); ++i) {
        os << i % 10 << " ";
    }
    os << "\n";
    Point p;
    for (p.y = 0; p.y < static_cast<int>(track.height()); ++p.y) {
        os << p.y % 10 << " ";
        for (p.x = 0; p.x < static_cast<int>(track.width()); ++p.x) {
            if (p.x != 0) {
                os << "─";
            }
            os << track.getField(p);
        }
        os << "\n";
    }
    return os;
}
