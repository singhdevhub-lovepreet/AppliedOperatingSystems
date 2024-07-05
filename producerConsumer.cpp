#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class Semaphore {
private:
    std::mutex mutex_;
    std::condition_variable condition_;
    unsigned long count_ = 0; // Initialized as locked.
public:
    
    Semaphore(int value){
        count_ = value;
    }

    void release() {
        std::lock_guard<std::mutex> lock(mutex_);
        ++count_;
        condition_.notify_one();
    }
    
    void acquire() {
        std::unique_lock<std::mutex> lock(mutex_);
        while(!count_) // Handle spurious wake-ups.
            condition_.wait(lock);
        --count_;
    }
};

std::mutex mtx;
std::queue<int> buffer;
const unsigned int MAX_BUFFER_SIZE = 10;

Semaphore empty_slots(MAX_BUFFER_SIZE); // Semaphore for empty slots
Semaphore filled_slots(0); // Semaphore for filled slots

void producer(int value) {
    empty_slots.acquire(); // Wait if there are no empty slots
    
    {
        std::lock_guard<std::mutex> lock(mtx); // slightly more effective than unique_lock, auto acquire, release on constructor and destructor
        buffer.push(value);
        std::cout << "Producing " << value << std::endl;
        std::cout << "Buffer size after producing: " << buffer.size() << std::endl << std::endl;
    }
    
    filled_slots.release(); // Signal that a slot has been filled
}

void consumer() {
    filled_slots.acquire(); // Wait if there are no filled slots
    
    int value;
    {
        std::lock_guard<std::mutex> lock(mtx);
        value = buffer.front();
        buffer.pop();
        std::cout << "Consuming " << value << std::endl;
        std::cout << "Buffer size after consuming: " << buffer.size() << std::endl << std::endl;
    }
    
    empty_slots.release(); // Signal that a slot has been emptied
}

int main() {
    std::thread producerThread([] {
        for (int i = 1; i <= 20000; ++i) {
            producer(i);
        }
    });
    
    std::thread consumerThread([] {
        for (int i = 1; i <= 20000; ++i) {
            consumer();
        }
    });
    
    producerThread.join();
    consumerThread.join();
    
    return 0;
}