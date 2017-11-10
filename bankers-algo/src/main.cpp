#include "Deadlock.h"
#include "Process.h"

int main(int argc, char *argv[])
{
  Deadlock deadlock;
  string input_file = argv[1];
  deadlock.ingest_document(input_file);
  Deadlock banker = deadlock, optimism = deadlock;
  optimism.optimistic_run();
  banker.bankers_algo();
}
