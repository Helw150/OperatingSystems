#ifndef deadlock_header
#define deadlock_header
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string>
#include <queue>
#include "Process.h"

using namespace std;

class Deadlock {
 public:

  // Functions
  Deadlock();
  // A Function which formats the input file into our struct
  void ingest_document(string input_file);
  // Input parsing helper functions
  void process_head(string line);
  void initiate_process(string line);
  void process_runtime(int type, string line);
  void handle_compute(string line);
  // Output Handling and Final Computations
  void print_results();
  // Helper labeled functions for Main clarity
  void optimistic_run();
  void bankers_algo();
  // A Helper function to check if a request leads to a safe state
  bool banker_safe(vector<int> command, int command_pid);
  // A Helper function to kill and release a processes resources
  void kill_process(int pid_for_death, Process process_for_death);
  // A Helper function to save all released resources for the end of the phase
  void release_resources(unordered_map<int,int> released);
  // A helper function to kill any request that exceeds max resources
  void check_exceeds();

  // The workhorse
  // Holds code for both Banker and Optimistic as controlled by a bool
  void simulate(bool banker);
  
  // Variables
  // The Cycle count, Time the simulation has been running, and the time processes in the simulation spent waiting
  int phase, total_time, total_wait;
  // An ordered mapping of Process ID's to a Process class details
  map<int, Process> process_table;
  // An ordered mapping of Process ID's to the final results
  map<int, string> results_table;
  // A mapping of Resource ID's to the currently availability of that resource
  unordered_map<int,int> available_resources;
  // The total number of processes in the system
  int num_processes;
  // The total number of resources we have in available resources
  int num_resources;
};


#endif
