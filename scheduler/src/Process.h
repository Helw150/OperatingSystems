
#ifndef process_header
#define process_header


using namespace std;

class Process {
public:

    Process(int a, int b, int c, int m);
    int arrival_time;
    int max_burst;
    int CPU_time;
    int multi;
    int cycle;
    int block_time;
    int cpu_burst;
    int total_wait_time;
    int total_block_time;
    int turn_around_time;
    int initial_cpu_time;
    int cpu_burst_left;
    int finish_time;
    int quantum;
    int ready_time;
    int status;
};

#endif
