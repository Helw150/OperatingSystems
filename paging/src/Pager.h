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
  int quantum;
  int machine_size;
  int page_size;
  int process_size;
  int job_mix;
  int reference_num;
  string replacement_method;

  vector<Process> process_table;
  
  // Functions
  Paging(int M, int P, int S, int J, int N, string R);
  
};


#endif
