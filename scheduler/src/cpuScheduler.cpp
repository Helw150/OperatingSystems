#include <iostream>
#include "scheduler.h"

using namespace std;

void scheduler::print_processes() {
  cout << num_commands << " ";
  for (int i = 0; i < num_commands; i++) {
    cout << "(" << process_vector[i].arrival_time << " ";
    cout << process_vector[i].max_burst << " ";
    cout << process_vector[i].initial_cpu_time << " ";
    cout << process_vector[i].multi << ") ";
  }
}

void verbose_print(int currentCycle, vector<Process> process_vector) {
  cout << "Before cycle " << setw(5) << currentCycle << ":";
  for (int i = 0; i < process_vector.size(); i++) {
    switch(process_vector[i].status){
    case 0:
      cout << "    unstarted  0.";
      break;
    case 1:
      cout << "        ready  0.";
      break;
    case 2:
      cout << "      running  " << process_vector[i].cpu_burst_left + 1 << ".";
      break;
    case 3:
      cout << "      blocked  " << process_vector[i].block_time + 1 << ".";
      break;
    case -1:
      cout << "   terminated  0.";
      break;
    default:
      cout << "ERROR: INVALID STATUS.";
      break;
    }
  }
  cout << "\n";
}

int scheduler::randomOS(int U) {
  int randIntFromList;
  int value;
  randIntFromList = random_numbers[rand_index];
  rand_index++;
  value = ((randIntFromList % U) + 1);
  return value;
}


scheduler::scheduler() {

  ifstream random_file("random-numbers.txt");
  string tmp_string;
  while(getline(random_file, tmp_string)){
    random_numbers.push_back(stoi(tmp_string));
  }

  random_file.close();
  rand_index = 0;
  cpu_burst = 0;
  any_running = false;
  has_waited = false;
  currentCycle = -1;
  total_processes_terminated = 0;
  total_cpu_time = 0;
  finish_time = 0.00;
  total_block_time = 0.00;
  total_wait_time = 0.00;
  total_cpu_time = 0;
  total_turnaround_time = 0.00;
}

void scheduler::process_input(string input_file, bool is_verbose) {
  verbose = is_verbose;
  ifstream infile(input_file);
  string line = "";
  getline(infile, line);
  istringstream iss(line);
  string subs;
  iss >> subs;
  num_commands = stoi(subs);
  for(int i = 0; i < num_commands; i++)
    {
      vector<int> tmp_storage;
      for(int j = 0; j < 4; j++)
        {
          iss >> subs;
          if (subs != " " && subs != "")
            {
              tmp_storage.push_back(stoi(subs));
            }
        }
      Process process (tmp_storage[0], tmp_storage[1],tmp_storage[2],tmp_storage[3]);
      process_vector.push_back(process);
    }
  cout << "The original input was: ";
  print_processes();

  sort(process_vector.begin(), process_vector.end(),
       [](const Process & a, const Process & b) -> bool
       {
         return a.arrival_time < b.arrival_time;

       });
  cout << endl;

  cout << "The sorted input was:   ";
  print_processes();
  cout << endl;
}

void scheduler::print_output() {
  for (int i = 0; i < process_vector.size(); i++) {

    cout << "Process " << i << ":\n";
    cout << "\t(A,B,C,M) = " << "("
         << process_vector[i].arrival_time << ","
         << process_vector[i].max_burst << ","
         << process_vector[i].initial_cpu_time << ","
         << process_vector[i].multi << ")\n";
    cout << "\tFinishing Time: " << process_vector[i].finish_time;
    total_turnaround_time += (process_vector[i].finish_time - process_vector[i].arrival_time);
    cout << "\n\tTurnaround Time: " << (process_vector[i].finish_time - process_vector[i].arrival_time);
    cout << "\n\tI/O Time: " << process_vector[i].total_block_time;
    cout << "\n\tWaiting Time: " << process_vector[i].total_wait_time << "\n\n\n";
  }
}

void scheduler::print_summary() {
  cout << "Summary Data:";
  cout << "\n\tFinishing Time: " << finish_time;
  cout << "\n\tCPU Utilization: " << total_cpu_time / finish_time;
  cout << "\n\tI/O Utilization: " << total_block_time / finish_time;
  cout << "\n\tThroughput: " << ((process_vector.size() / finish_time) * 100) << " processes per hundred cycles";
  cout << "\n\tAverage Turnaround: " << (total_turnaround_time / process_vector.size());
  cout << "\n\tAverage Waiting Time: " << (total_wait_time / process_vector.size());
  cout << "\n";
}

void scheduler::set_process(int currentCycle) {
  for (int i = 0; i < process_vector.size(); i++) {
    switch(process_vector[i].status){
    case 0: // unstarted
      if (process_vector[i].arrival_time == currentCycle) {
        process_vector[i].status = 1;
        process_vector[i].ready_time = 0;
        ready_queue.push(i);
      }
      break;
    case 2: // running
      any_running = true;
      // Terminated process comes first
      if (process_vector[i].CPU_time <= 0) {
        process_vector[i].status = -1;
        process_vector[i].finish_time = currentCycle;
        total_processes_terminated++;
        any_running = false;
      }
      // Process to I/O block comes second
      else if (process_vector[i].cpu_burst_left <= 0) {
        process_vector[i].status = 3;
        process_vector[i].block_time = process_vector[i].cpu_burst * process_vector[i].multi;
        any_running = false;
      }
      // Process Pre-emption comes third
      else if (process_vector[i].quantum == 0) {
        cout << "quantum";
        process_vector[i].status = 1;
        process_vector[i].ready_time = 0;
        any_running = false;
      }
      break;
    case 3: // blocked
      if (process_vector[i].block_time <= 0) {
        process_vector[i].status = 1;
        process_vector[i].ready_time = 0;
        ready_queue.push(i);
      }
      break;
    }
  }
}

void scheduler::run_process(int currentCycle) {
  bool blocked = false;
  for (int i = 0; i < process_vector.size(); i++) {
    switch(process_vector[i].status)
      {
      case 1: // ready
        process_vector[i].ready_time++;
        break;
      case 2: // running
        process_vector[i].cpu_burst_left--;
        total_cpu_time++;
        process_vector[i].CPU_time--;
        if (process_vector[i].quantum > 0) {
          process_vector[i].quantum--;
        }
        break;
      case 3: // blocked
        process_vector[i].block_time--;
        process_vector[i].total_block_time++;
        blocked = true;
        break;
      }
  }
  if (blocked){
    total_block_time++;
  }
}

void scheduler::decrement_processes()
{
  bool has_waited = false;
  //decrement values for running and blocked processes
  run_process(currentCycle);
  for (int i = 0; i < process_vector.size(); i++) {
    if (process_vector[i].status == 1) {
      process_vector[i].total_wait_time = process_vector[i].total_wait_time + 1;
      has_waited = true;
    }
  }

  if (has_waited) {
    total_wait_time++;
  }
}

void scheduler::FCFS() {
  //Loop until all processes are terminated
  while (total_processes_terminated < process_vector.size()) {
    currentCycle++;
    if(verbose){
      verbose_print(currentCycle, process_vector);
    }
    set_process(currentCycle);

    if (any_running == false) {
      if (!ready_queue.empty()) {
        front_queue = ready_queue.front();
        if (process_vector[front_queue].status == 1) {
          ready_queue.pop();

          process_vector[front_queue].status = 2;
          any_running = true;
          cpu_burst = randomOS(process_vector[front_queue].max_burst);
          process_vector[front_queue].cpu_burst_left = cpu_burst;
          process_vector[front_queue].cpu_burst = cpu_burst;
        }
      }
    }
    decrement_processes();
  }
  cout << "The Scheduling Algorithm used was First Come First Serve\n\n";
  finish_time = currentCycle;
  print_output();
  print_summary();
}

void scheduler::SJF() {
  //Loop until all processes are terminated
  while (total_processes_terminated < process_vector.size()) {
    currentCycle++;
    if(verbose){
      verbose_print(currentCycle, process_vector);
    }
    set_process(currentCycle);

    if(any_running == false){
      int min = 99999999;
      int shortest_ready_job;
      int not_all_blocked = false;
      for(int i = 0; i < process_vector.size(); i++){
        if (process_vector[i].CPU_time < min && process_vector[i].status == 1) {
          shortest_ready_job = i;
          min = process_vector[i].CPU_time;
          not_all_blocked = true;
        }
      }

      if(not_all_blocked){
          process_vector[shortest_ready_job].status = 2;
          any_running = true;
          cpu_burst = randomOS(process_vector[shortest_ready_job].max_burst);
          process_vector[shortest_ready_job].cpu_burst_left = cpu_burst;
          process_vector[shortest_ready_job].cpu_burst = cpu_burst;
        }
    }
    decrement_processes();
  }
  cout << "The Scheduling Algorithm used was Shortest Job First\n\n";
  finish_time = currentCycle;
  print_output();
  print_summary();
}

void scheduler::HPRN() {
  //Loop until all processes are terminated
  while (total_processes_terminated < process_vector.size()) {
    currentCycle++;
    if(verbose){
      verbose_print(currentCycle, process_vector);
    }
    set_process(currentCycle);

    if(any_running == false){
      int max_PR = -9999999;
      int highest_penalty_job;
      int not_all_blocked = false;
      for(int i = 0; i < process_vector.size(); i++){
        float PR = process_vector[i].total_wait_time/max(process_vector[i].initial_cpu_time-process_vector[i].CPU_time, 1);
        if (PR > max_PR && process_vector[i].status == 1) {
          highest_penalty_job = i;
          max_PR = process_vector[i].CPU_time;
          not_all_blocked = true;
        }
      }

      if(not_all_blocked){
          process_vector[highest_penalty_job].status = 2;
          any_running = true;
          cpu_burst = randomOS(process_vector[highest_penalty_job].max_burst);
          process_vector[highest_penalty_job].cpu_burst_left = cpu_burst;
          process_vector[highest_penalty_job].cpu_burst = cpu_burst;
        }
    }
    decrement_processes();
  }
  cout << "The Scheduling Algorithm used was Highest Penalty Ratio Next\n\n";
  finish_time = currentCycle;
  print_output();
  print_summary();
}

void scheduler::RR() {

  while (total_processes_terminated < process_vector.size()) {
    currentCycle++;
    if(verbose){
      verbose_print(currentCycle, process_vector);
    }
    set_process(currentCycle);

    longest_ready_time = -1;
    ready_process = -1;

    // Choose who has been waiting it's turn the longest
    // Since we use > for the wait time,
    // the process with an earlier arrival time will run first
    for (int i = 0; i < process_vector.size(); i++) {
      if (process_vector[i].status == 1 && process_vector[i].ready_time > longest_ready_time) {
        ready_process = i;
        longest_ready_time = process_vector[i].ready_time;
      }
    }

    if (any_running == false & ready_process != -1) {
      process_vector[ready_process].status = 2;
      any_running = true;
      process_vector[ready_process].quantum = 2;

      if (process_vector[ready_process].cpu_burst_left == 0) {
        cpu_burst = randomOS(process_vector[ready_process].max_burst);
        process_vector[ready_process].cpu_burst_left = cpu_burst;
        process_vector[ready_process].cpu_burst = cpu_burst;
      }
    }

    decrement_processes();
  }
  cout << "The Scheduling Algorithm used was Round Robin\n\n";
  finish_time = currentCycle;
  print_output();
  print_summary();
}

Process::Process
(int a, int b, int c, int m) {
  arrival_time = a;
  max_burst = b;
  initial_cpu_time = c;
  multi = m;
  status = 0;
  cycle = 0;
  total_wait_time = 0;
  total_block_time = 0;
  cpu_burst = 0;
  cpu_burst_left = 0;
  CPU_time = initial_cpu_time;
  turn_around_time = 0;
  ready_time = 0;
  quantum = -1;
}
