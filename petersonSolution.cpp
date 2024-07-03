#include <iostream>
#include <thread>

#define TRUE 1
#define FALSE 0
#define N 2

int turn = 0;
int interested[N];
int x = 0;

void enter_section(int process){
    int other = 1-process;
    interested[process] = TRUE;
    turn = process;
    while(turn==process && interested[other]==TRUE);
    
}

void leave_section(int process){
    interested[process] = FALSE;
}

class thread_object {
public:
    void operator()(int process) {
        for (int i = 0; i < 10000000; ++i) {
            enter_section(process);
            x += 1;
            std::cout<<"Thread "<<process<<" "<<x<<std::endl;
            leave_section(process);
        }
    }
};

int main(int argc, char *argv[]){
    thread_object obj;
    std::thread th1(obj, 0);
    std::thread th2(obj, 1);
    th1.join();
    th2.join();
    return 0;
}