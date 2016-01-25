
#ifndef SEMAPHORE_H
#define SEMAPHORE_H


#include <condition_variable>

class Semaphore {
public:
    Semaphore();

    void acquire();
    void release();
private:
    bool free;
    std::mutex mutex;
    std::condition_variable condition;
};

#endif /* SEMAPHORE_H */

