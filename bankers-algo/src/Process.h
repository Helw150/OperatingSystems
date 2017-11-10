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
  // A queue which stores all of our instructions
  queue<vector<int>> instructions;
  // A mapping of resource ID to max claims
  unordered_map<int, int> requests;
  // A mapping of resource ID to current ownership
  unordered_map<int, int> owned;
};

#endif
