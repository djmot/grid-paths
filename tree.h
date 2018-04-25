#ifndef TREE_H
#define TREE_H

#include <vector>

///
///  Node class - represents nodes in the path building tree
///
/// (turns out this wasn't really necessary but oh well)
class Node {
    private:
        bool open;
        int parent;
        int right;
        int up;
        int left;
        int down;
        int pos[2];
        int type;
    public:
        Node () { // Default constructor creates root node
            this->open = false;
            this->type = -1;
            this->pos[0] = this->pos[1] = 0;
            this->parent = -1;
            this->right = -1;
            this->up = -1;
            this->left = -1;
            this->down = -1;
        }
        Node (bool open, int type, int x, int y, int parent, int right, int up, int left, int down) {
            this->open = open;
            this->type = type;
            this->pos[0] = x;
            this->pos[1] = y;
            this->parent = parent;
            this->right = right;
            this->up = up;
            this->left = left;
            this->down = down;
        }
        void flip_open() { open = !open; }
        void set_type(int type) { this->type = type; }
        void set_pos(int pos[2]) { this->pos[0] = pos[0]; this->pos[1] = pos[1]; }
        void set_parent(int parent) { this->parent = parent; }
        void set_right(int right) { this->right = right; }
        void set_up(int up) { this->up = up; }
        void set_left(int left) { this->left = left; }
        void set_down(int down) { this->down = down; }
        bool is_open() { return open; }
        int get_type() { return this->type; }
        int get_x() { return pos[0]; }
        int get_y() { return pos[1]; }
        int get_parent() { return parent; }
        int get_right() { return right; }
        int get_up() { return up; }
        int get_left() { return left; }
        int get_down() { return down; }
};


///
///  Some convenient structs
///

// PathCounts stores the numbers of different types of constructed paths
struct PathCounts
{
    int total;
    int opp;
    int adj;
};

// Position stores a grid position - used because vectors can store structs but not arrays
struct Position
{
    int x;
    int y;
};

// Position equality comparison
inline bool operator==(const Position& posA, const Position& posB) {
    return ( posA.x == posB.x && posA.y == posB.y );
}


#endif
