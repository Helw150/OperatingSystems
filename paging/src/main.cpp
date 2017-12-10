#include "Pager.h"
#include "Process.h"

int main(int argc, char *argv[])
{
  if(argc == 7 || argc == 8){

    Paging pager(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), argv[6]);
    pager.simulate();
    pager.outputs();
  } else {
    cout << "Incorrect number of arguments, please revisit the spec to correctly input arguments";
  }
}
