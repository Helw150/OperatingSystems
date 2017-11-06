#ifndef process_header
#define process_header
#include <unordered_map>
#include <vector>
using namespace std;

class Process {
public:

  Process();
  vector<vector<int>> instructions;
  unordered_map<int, int> requests;
  unordered_map<int, int> owned;
};

#endif
