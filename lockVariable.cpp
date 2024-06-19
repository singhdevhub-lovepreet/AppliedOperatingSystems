#include <iostream>
#include <thread>

int lock = 0;

class thread_object {
public:
    void operator()(int& x) {
        for (int i = 0; i < 10000; ++i) {
            while (lock != 0);
            lock = 1;
            x += 1;
            lock = 0;
        }
    }
};

int main() {
    int x = 0;
    thread_object obj;
    std::thread t1(obj, std::ref(x));
    std::thread t2(obj, std::ref(x));

    t1.join();
    t2.join();

    std::cout << "Final value of x: " << x << std::endl;

    return 0;
}