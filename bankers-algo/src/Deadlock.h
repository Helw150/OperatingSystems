#ifndef deadlock_header
#define deadlock_header
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
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

  // Variables
  int phase;
  unordered_map<int, Process> process_table;
  vector<int> max_resource_vector;
  vector<int> available_resource_vector;
  int num_processes;
  int num_resources;
};


#endif
