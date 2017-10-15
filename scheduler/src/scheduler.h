#ifndef scheduler_header
#define scheduler_header
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <string>
#include <stdio.h>
#include "Process.h"
#include <queue>
#include <iomanip>
#include <algorithm>

using namespace std;


class scheduler {

public:

    vector <Process> process_vector;
    vector <Process> sorted_vector;

    scheduler();
    vector<int> random_numbers;
    int rand_index;

    int num_commands;
    int a;
    int b;
    int c;
    int m;
    bool verbose;

    int cpu_burst;
    int total_processes_terminated;
    int total_cpu_time;
    float total_turnaround_time;
    float total_wait_time;
    float finish_time;
    float total_block_time;
    queue <int> ready_queue;
    deque <int> sjf_queue;
    bool any_running;
    bool has_waited;
    int front_queue;
    int currentCycle;
    int longest_ready_time;
    int ready_process;

    void decrement_processes();
    void print_summary();
    void print_output();
    void run_process(int currentCycle);
    void set_process(int currentCycle);
    void print_processes();
    void process_input(string input_file, bool is_verbose);
    int randomOS(int U);


    void FCFS();
    void SJF();
    void RR();
    void HPRN();
};


#endif
