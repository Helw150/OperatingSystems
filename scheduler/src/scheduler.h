#ifndef LAB_2_TESTS_SCHEDULERMETHODS_H
#define LAB_2_TESTS_SCHEDULERMETHODS_H
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
    vector <int> temp_vector;     //For Sorting

    scheduler();
    vector<int> random_numbers;
    int rand_index;

    int num_commands;             //Number of processes
    char unused;                      //unused char
    int a;                            //Arrival Time
    int b;                            //Max burst
    int c;                            //Total CPU time needed
    int m;                            //Max IO block time
    bool verbose;

    int cpu_burst;                    //CPU Burst = randomOS(max burst)
    int total_processes_terminated;   //Current number of processes terminated
    int total_cpu_time;               //Time any process was running
    float total_turnaround_time;      //Total turnaround time of the algo
    float total_wait_time;            //Total wait time of the algo
    float finish_time;                //Finish cycle of algorithm
    float total_block_time;           //Time any process was blocked
    queue <int> ready_queue;          //Contains index which represent specific process object
    queue <int> empty;
    deque <int> sjf_queue;
    bool any_running;                 //True if a process is running
    bool is_found;                    //If process was found
    bool has_blocked;
    bool has_waited;
    int front_queue;
    int currentCycle;
    int currentProcess;
    int min;
    int num_ready;
    int min_vec_index;
    int longest_ready_time;
    int ready_process;

    void handle_queue();
    void print_summary();
    void print_output();
    void run_process(int currentCycle);
    void set_process(int currentCycle);
    void print_processes();
    void process_input(string input_file, bool is_verbose);
    int randomOS(int U);


    void Fcfs();
    void Uniprogrammed();
    void sjf();
    void rr();
};


#endif //LAB_2_TESTS_SCHEDULERMETHODS_H
