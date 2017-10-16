#ifndef process_header
#define process_header

using namespace std;

class Process {
public:

    Process(int a_arg, int b_arg, int c_arg, int m_arg);
    int a;
    int b;
    int c;
    int m;
    int cpu_time;
    int block_time;
    int cpu_burst;
    int time_waited;
    int time_blocked;
    int remaining_burst;
    int finish_time;
    int quantum;
    int rr_wait;
    int state;
};

#endif
