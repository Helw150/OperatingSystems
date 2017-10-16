#ifndef scheduler_header
#define scheduler_header
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <string>
#include <stdio.h>
#include <algorithm>
#include <queue>
#include "Process.h"

using namespace std;


class Scheduler {

 public:
  Scheduler();
  vector <Process> document_array;

  vector<int> random_numbers;
  int rand_index;

  int num_commands;
  bool verbose;

  int burst;
  int total_processes_terminated;
  int total_cpu_time;
  float total_turnaround_time;
  float total_wait_time;
  float total_block_time;
  float finish_time;
  bool any_running;
  int current_cycle;
  queue <int> fcfs_queue;

  void decrement_processes();
  void handle_time_change(int currentCycle);
  void change_states(int currentCycle);
  void print_processes();
  void print_summary();
  void print_output();
  void ingest_document(string input_file, bool is_verbose);
  int randomOS(int U);


  void FCFS();
  void SJF();
  void RR();
  void HPRN();
};


#endif
