#ifndef paging_header
#define paging_header
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

class Paging {
 public:
  // Variables
  vector<int> random_numbers;
  int rand_index;
  int quantum;
  int machine_size;
  int page_size;
  int process_size;
  int job_type;
  int reference_num;
  string replacement_method;

  int last_in;
  int run_time;
  vector<Process> process_table;
  vector<vector<int>> frame_table;
  
  // Functions
  Paging(int M, int P, int S, int J, int N, string R);
  void jobType();
  void initializeFrames();
  void firstWords();
  int randomNumber();
  void simulate();
  void assignReference(int active_process);
  void newCycle(int active_process);
  void outputs();
};


#endif
