#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <queue>
#include "Deadlock.h"

using namespace std;

Deadlock::Deadlock() {
  phase = 0;
  num_processes = 0;
  num_resources = 0;
}

Process::Process() {
  finish = 0;
  waiting = 0;
}

// a function to deal with vectors for debugging purposes
void print_vector(vector<int> vector){
  for (auto i = vector.begin(); i != vector.end(); ++i)
    cout <<" |" << *i << "| ";
  return;
}

// Finalize all resources from the previous cycle
void Deadlock::release_resources(unordered_map<int,int> released){
  for (auto i = released.begin(); i != released.end(); i++){
    available_resources[i->first] += i->second;
  }
}

void Deadlock::optimistic_run(){
  int current_pid, num_finished = 0;
  while(num_finished < num_processes){
    ++phase;
    //cout << ++phase << "-------------------\n";
    // This variable stores all releases to perform at the end of the cycle
    
    unordered_map<int, int> released;
    int num_blocked = 0;
    // shallow copy the table so we can delete while iterating
    map<int, Process> tmp_table = process_table;     
    for (auto i = tmp_table.begin(); i != tmp_table.end(); i++){
      current_pid = i->first;
      string results;
      const Process current_process = i->second;
      vector<int> command = current_process.instructions.front();
      switch(command[0]){
      case 0: // Requesting resources
	//cout << current_pid << " request\n";
	if(available_resources[command[1]] >= command[2]){
	  available_resources[command[1]] -= command[2];
	  process_table[current_pid].owned[command[1]] += command[2];
	  process_table[current_pid].instructions.pop();
	} else {
	  process_table[current_pid].waiting++;
	  num_blocked += 1;
	}
	break;
      case 1: // Releasing resources
	//cout << current_pid << " release\n";
	released[command[1]] += command[2];
	process_table[current_pid].owned[command[1]] -= command[2];
	process_table[current_pid].instructions.pop();
	break;
      case 2: //Terminating process
	//cout << current_pid << " kill\n";
        results = to_string(phase)+"\t"+to_string(current_process.waiting);
	results_table.emplace(current_pid, results);
	kill_process(current_pid, current_process);
	num_finished++;
	break;
      case 3: // Computing - simply pass
	process_table[current_pid].instructions.pop();
	break;
      default:
	cout << "Error - incorrect request\n";
	break;
      }
    }
    if(num_blocked != 0 && num_blocked == (num_processes - num_finished)){
      int pid_for_death = process_table.begin()->first;
      Process process_for_death = process_table.begin()->second;
      results_table.emplace(pid_for_death, "Aborted");
      kill_process(pid_for_death, process_for_death);
      num_finished++;
    }
    release_resources(released);
  }
  print_results();
  return;
}

void Deadlock::print_results(){
  for(auto i = results_table.begin(); i != results_table.end(); i++){
    cout << "Task " << to_string(i->first) << "\t" << i->second << "\n";
  }
  return;
}

void Deadlock::kill_process(int pid_for_death, Process process_for_death){
  for(auto i = process_for_death.owned.begin(); i != process_for_death.owned.end(); i++){
    available_resources[i->first] += i->second;
  }
  process_table.erase(pid_for_death);
}  

void Deadlock::process_head(string line){
  istringstream iss(line);
  string subs;
  iss >> subs;
  num_processes = stoi(subs);
  iss >> subs;
  num_resources = stoi(subs);
  for(int i = 1; i <= num_resources; i++){
    iss >> subs;
    //cout << i << " " << stoi(subs) << "\n";
    available_resources.emplace(i, stoi(subs));
  }
}

void Deadlock::initiate_process(string line){
  istringstream iss(line);
  iss >> line;
  int process_number, resource_number, resource_amount;
  iss >> process_number >> resource_number >> resource_amount;
  process_table[process_number].requests.emplace(resource_number, resource_amount);
  process_table[process_number].owned.emplace(resource_number, 0);
}

void Deadlock::process_runtime(int type, string line){
  istringstream iss(line);
  iss >> line;
  int process_number, resource_number, resource_amount;
  iss >> process_number >> resource_number >> resource_amount;
  process_table[process_number].instructions.push({type, resource_number, resource_amount});
}

void Deadlock::handle_compute(string line){
  istringstream iss(line);
  iss >> line;
  int process_number, num_cycles;
  iss >> process_number >> num_cycles;
  for(int i = 0; i < num_cycles; i++){
    process_table[process_number].instructions.push({3, -1, -1});
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