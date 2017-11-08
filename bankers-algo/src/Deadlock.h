#ifndef deadlock_header
#define deadlock_header
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <map>
#include <iostream>
#include <string>
#include <stdio.h>
#include <algorithm>
#include <queue>
#include "Process.h"

using namespace std;

class Deadlock {
 public:

  // Functions
  Deadlock();
  void ingest_document(string input_file);
  void initiate_process(string line);
  void process_runtime(int type, string line);
  void handle_compute(string line);
  void process_head(string line);
  void optimistic_run();
  void kill_process(int pid_for_death, Process process_for_death);
  void release_resources(unordered_map<int,int> released);
  void print_results();
  
  // Variables
  int phase, total_time, total_wait;
  map<int, Process> process_table;
  map<int, string> results_table;
  unordered_map<int,int> available_resources;
  int num_processes;
  int num_resources;
};


#endif
