#include "Track.hpp"

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
