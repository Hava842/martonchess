#ifndef DEPTH_H
#define DEPTH_H

class Depth {
public:
    static const int MAX_PLY = 256;
    static const int MAX_DEPTH = 64;
    static const int MAX_QUISCENT = 16;

private:
    Depth();
    ~Depth();
};

#endif /* DEPTH_H */

