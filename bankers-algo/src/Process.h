#ifndef process_header
#define process_header
#include <unordered_map>
#include <vector>
#include <queue>
using namespace std;

class Process {
public:

  Process();
  int finish, waiting, current_wait;
  queue<vector<int>> instructions;
  unordered_map<int, int> requests;
  unordered_map<int, int> owned;
};

#endif
