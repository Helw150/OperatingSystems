#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <queue>
#include "Pager.h"

using namespace std;

/* BEGIN STRUCT SETUP */

Paging::Paging(int M, int P, int S, int J, int N, string R) {
  machine_size = M;
  page_size = P;
  process_size = S;
  job_mix = J;
  reference_num = N;
  replacement_method = R;
  quantum = 3;

  switch(J){
  case 1:
    {
      Process single(1, 0, 0);
      process_table.push_back(single);
    }
    break;
  case 2:
    {
      Process quadruple(1, 0, 0);
      process_table.push_back(quadruple);
      process_table.push_back(quadruple);
      process_table.push_back(quadruple);
      process_table.push_back(quadruple);
    }
    break;
  case 3:
    {
      Process random_quadruple(1, 0, 0);
      process_table.push_back(random_quadruple);
      process_table.push_back(random_quadruple);
      process_table.push_back(random_quadruple);
      process_table.push_back(random_quadruple);
    }
    break;
  case 4:
    {
      Process first(0.75, 0.25, 0);
      Process second(0.75, 0, 0.25);
      Process third(0.75, 0.125, 0.125);
      Process fourth(0.5, 0.125, 0.125);
      process_table.push_back(first);
      process_table.push_back(second);
      process_table.push_back(third);
      process_table.push_back(fourth);
    }
    break;
  default:
    {
      Process only(1, 0, 0);
      process_table.push_back(only);
    }
    break;
  }
}

Process::Process(float a, float b, float c) {
  A = a;
  B = b;
  C = c;
  D = (((1-a)-b)-c);
}

/* END STRUCT SETUP */

