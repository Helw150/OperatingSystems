#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <queue>
#include "Deadlock.h"

using namespace std;

Deadlock::Deadlock() {
  phase = 0;
  total_time = 0;
  total_wait = 0;
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

bool cmp (pair<int, Process> i, pair<int, Process> j) {
  return (j.second.current_wait < i.second.current_wait);
}

vector<pair<int, Process>> sort_by_wait_time(map<int, Process> process_table){
  vector<pair<int, Process>> mapVector;
  for (auto iterator = process_table.begin(); iterator != process_table.end(); ++iterator) {
    mapVector.push_back(*iterator);
  }
  sort(mapVector.begin(), mapVector.end(), cmp);
  return mapVector;
}

bool Deadlock::banker_safe(vector<int> command, int command_pid){
  // Set up a toy space in which this step happens
  Deadlock toy_space = *this;
  toy_space.available_resources[command[1]] -= command[2];
  toy_space.process_table[command_pid].owned[command[1]] += command[2];
  // Try to complete all processes using maximum greed
  int num_complete = 0;
  int num_current_process = toy_space.process_table.size();
  while(num_complete < num_current_process){
    bool can_complete;
    bool did_complete = false;
    for(auto i = toy_space.process_table.begin(); i != toy_space.process_table.end(); i++){
      unordered_map<int,int> needed = i->second.requests;
      can_complete = true;
      for(auto j = needed.begin(); j != needed.end(); j++){
	needed[j->first] -= i->second.owned[j->first];
	if(needed[j->first] > toy_space.available_resources[j->first]){
	  can_complete = false;
	  break;
	}
      }
      if(can_complete){
	for(auto j = i->second.owned.begin(); j != i->second.owned.end(); j++){
	  toy_space.available_resources[j->first] += j->second;
	}
	toy_space.process_table.erase(i->first);
	num_complete++;
	did_complete = true;
	break;
      }
    }
    if(!did_complete){
      return false;
    }
  }
  return true;
}

void Deadlock::optimistic_run(){
  simulate(false);
  return;
}

void Deadlock::bankers_algo(){
  simulate(true);
  return;
}

void Deadlock::check_exceeds(){
  for(auto i = process_table.begin(); i != process_table.end(); i++){
    int process_number = i->first;
    Process process_checked = i->second;
    for(auto j = process_checked.requests.begin(); j != process_checked.requests.end(); j++){
      int resource_number = j->first, resource_amount = j->second;
      if(available_resources[resource_number] < resource_amount){
	results_table.emplace(process_number, "Aborted - Claim exceeds existing resources");
	kill_process(process_number, process_checked);
	num_processes--;
      }
    }
  }
}

void Deadlock::simulate(bool banker){
  int current_pid, num_finished = 0;
  bool chain_abort = false;
  check_exceeds();
  while(num_finished < num_processes){
    ++phase;
    //cout << phase << "-------------------\n";
    // This variable stores all releases to perform at the end of the cycle
    
    unordered_map<int, int> released;
    int num_blocked = 0;
    // shallow copy the table so we can delete while iterating
    vector<pair<int, Process>> tmp_table = sort_by_wait_time(process_table);     
    for (auto i = tmp_table.begin(); i != tmp_table.end(); i++){
      current_pid = i->first;
      string results;
      float percentage;
      const Process current_process = i->second;
      vector<int> command = current_process.instructions.front();
      switch(command[0]){
      case 0: // Requesting resources
	// If resources and either not banker or safe state
	if(process_table[current_pid].owned[command[1]]+command[2] > process_table[current_pid].requests[command[1]]){
	  results_table.emplace(current_pid, "Aborted - Requested Greater Resources than Claimed");
	  kill_process(current_pid, current_process);
	  num_finished++;
	} else {
	  if(available_resources[command[1]] >= command[2] && (!banker || banker_safe(command, current_pid))){
	    //cout << current_pid << " request\n";
	    chain_abort = false;
	    process_table[current_pid].current_wait = 0;
	    available_resources[command[1]] -= command[2];
	    process_table[current_pid].owned[command[1]] += command[2];
	    process_table[current_pid].instructions.pop();
	  } else {
	    //cout << current_pid << " waiting\n";
	    if(!chain_abort){
	      process_table[current_pid].current_wait++;
	      process_table[current_pid].waiting++;
	    }
	    num_blocked += 1;
	  }
	}
	break;
      case 1: // Releasing resources
	//cout << current_pid << " release\n";
	chain_abort = false;
	released[command[1]] += command[2];
	process_table[current_pid].owned[command[1]] -= command[2];
	process_table[current_pid].instructions.pop();
	break;
      case 2: //Terminating process
	//cout << current_pid << " kill\n";
	chain_abort = false;
	total_time += phase;
	total_wait += current_process.waiting;
	percentage = float(current_process.waiting)/float(phase)*100;
	results = to_string(phase)+"   "+to_string(current_process.waiting)+"   "+to_string(int(round(percentage)))+"%";
	results_table.emplace(current_pid, results);
	kill_process(current_pid, current_process);
	num_finished++;
	break;
      case 3: // Computing - simply pass
	chain_abort = false;
	process_table[current_pid].instructions.pop();
	break;
      default:
	cout << "Error - incorrect request\n";
	break;
      }
    }
    // If not banker check for deadlock and abort
    if(!banker && num_blocked != 0 && num_blocked == (num_processes - num_finished)){
      int pid_for_death = process_table.begin()->first;
      //cout << pid_for_death << " aborted\n";
      Process process_for_death = process_table.begin()->second;
      if(chain_abort){
	phase--;
      }
      results_table.emplace(pid_for_death, "Aborted");
      kill_process(pid_for_death, process_for_death);
      num_finished++;
      chain_abort = true;
    }
    release_resources(released);
  }
  if(banker){
    cout << "              Banker's\n";
  } else {
    cout << "              FIFO\n";
  }
  print_results();
  return;
}

void Deadlock::print_results(){
  for(auto i = results_table.begin(); i != results_table.end(); i++){
    cout << "     Task " << to_string(i->first) << "      " << i->second << "\n";
  }
  cout << "     Total       " << total_time << "   " << total_wait << "   "<< int(round((float(total_wait)/float(total_time))*100))<<"%\n";
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
  phase = num_resources - 1;
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
