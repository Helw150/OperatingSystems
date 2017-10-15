#include "scheduler.h"
#include "Process.h"

int main(int argc, char *argv[]) {
  string input_processes;
  const string VERBOSE = "--verbose";
  bool is_verbose = false;
  scheduler FCFS, RR, SJF, HPRN;
  if (argv[1] == VERBOSE) {
    input_processes = argv[2];
    is_verbose = true;
  }
  else if (argc == 2){
    input_processes = argv[1];
  } else {
    cout << "Incorrect invocation";
    return 0;
  }
  // FCFS.process_input(input_processes, is_verbose);
  // FCFS.FCFS();
  // cout << "\n-----------------------\n";
  // RR.process_input(input_processes, is_verbose);
  // RR.RR();
  // cout << "\n-----------------------\n";
  // SJF.process_input(input_processes, is_verbose);
  // SJF.SJF();
  // cout << "\n-----------------------\n";
  HPRN.process_input(input_processes, is_verbose);
  HPRN.HPRN();
  return 0;
}
