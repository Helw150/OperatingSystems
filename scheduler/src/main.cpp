#include "Scheduler.h"
#include "Process.h"

int main(int argc, char *argv[]) {
  string input_processes;
  const string VERBOSE = "--verbose";
  bool is_verbose = false;
  Scheduler FCFS, RR, SJF, HPRN;
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
  FCFS.ingest_document(input_processes, is_verbose);
  FCFS.FCFS();
  cout << "\n-----------------------\n";
  RR.ingest_document(input_processes, is_verbose);
  RR.RR();
  cout << "\n-----------------------\n";
  SJF.ingest_document(input_processes, is_verbose);
  SJF.SJF();
  cout << "\n-----------------------\n";
  HPRN.ingest_document(input_processes, is_verbose);
  HPRN.HPRN();
  return 0;
}
