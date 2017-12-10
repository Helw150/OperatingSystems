#ifndef process_header
#define process_header
#include <unordered_map>
#include <vector>
#include <queue>
using namespace std;

class Process {
public:

  // Probability Variables
  float A, B, C, D;
  int residency_time, faults, evictions,  current_word, number_of_references;
  
  Process(float a, float b, float c, int reference_num);
};

#endif
