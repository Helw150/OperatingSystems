#include <iostream>
#include <vector>
#include <unordered_map>
#include "Deadlock.h"

using namespace std;

Deadlock::Deadlock() {
  phase = 0;
  num_processes = 0;
  num_resources = 0;
}

Process::Process() {

}

// a function to deal with vectors for debugging purposes
void print_vector(vector<int> vector){
  for (auto i = vector.begin(); i != vector.end(); ++i)
    cout <<" |" << *i << "| ";
  return;
}

void Deadlock::process_head(string line){
  istringstream iss(line);
  string subs;
  iss >> subs;
  num_processes = stoi(subs);
  iss >> subs;
  num_resources = stoi(subs);
  for(int i = 0; i < num_resources; i++){
    iss >> subs;
    max_resource_vector.push_back(stoi(subs));
    available_resource_vector.push_back(stoi(subs));
  }
}

void Deadlock::initiate_process(string line){
  istringstream iss(line);
  iss >> line;
  int process_number, resource_number, resource_amount;
  iss >> process_number >> resource_number >> resource_amount;
  // if ( process_table.find(process_number) == process_table.end() ) {
  //   Process process;
  //   process_table.emplace(process_number, process);
  // }
  cout << "lel";
  process_table[process_number].requests.emplace(resource_number, resource_amount);
}

void Deadlock::process_runtime(int type, string line){
  istringstream iss(line);
  iss >> line;
  int process_number, resource_number, resource_amount;
  iss >> process_number >> resource_number >> resource_amount;
  process_table[process_number].instructions.push_back({type, resource_number, resource_amount});
}

void Deadlock::handle_compute(string line){
  istringstream iss(line);
  iss >> line;
  int process_number, num_cycles;
  iss >> process_number >> num_cycles;
  for(int i = 0; i < num_cycles; i++){
    process_table[process_number].instructions.push_back({3, -1, -1});
  }
}

void Deadlock::ingest_document(string input_file) {
  ifstream infile(input_file);
  string line = "";
  getline(infile, line);
  process_head(line);
  while(getline(infile, line)){
    if(line.size()){
      istringstream iss(line);
      string subs;
      iss >> subs;
      if(subs == "initiate"){
	initiate_process(line);
      } else if (subs == "request") {
	process_runtime(0, line);
      } else if (subs == "compute") {
	handle_compute(line);
      } else if (subs == "release") {
	process_runtime(1, line);
      } else if (subs == "terminate") {
        process_runtime(2, line);
      } else {
	cout << "Error in Input Processing";
	return;
      }
    }
  }
}
