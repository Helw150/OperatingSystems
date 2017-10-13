#include "scheduler.h"
#include "Process.h"

int main(int argc, char *argv[]) {
  string input_processes;
  const string VERBOSE = "--verbose";
  bool is_verbose = false;
  scheduler genericScheduler, FCFS, RR, SJF;
  if (argv[1] == VERBOSE) {
    input_processes = argv[2];
    genericScheduler.process_input(input_processes, true);
  }
  else if (argc == 2){
    input_processes = argv[1];
    genericScheduler.process_input(input_processes, false);
  } else {
    cout << "Incorrect invocation";
    return 0;
  }
  FCFS = RR = SJF = genericScheduler;
  FCFS.Fcfs();
  SJF.sjf();
  RR.rr();
  return 0;
}
