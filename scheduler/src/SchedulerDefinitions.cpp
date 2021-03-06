#include <iostream>
#include "Scheduler.h"

using namespace std;

void Scheduler::print_processes() {
  cout << num_commands << " ";
  for (int i = 0; i < num_commands; i++) {
    cout << "(" << document_array[i].a << " ";
    cout << document_array[i].b << " ";
    cout << document_array[i].c << " ";
    cout << document_array[i].m << ") ";
  }
}

void verbose_print(int current_cycle, vector<Process> document_array) {
  cout << "Before cycle " << "     " << current_cycle << ":";
  for (int i = 0; i < document_array.size(); i++) {
    switch(document_array[i].state){
    case 0:
      cout << "    unstarted  0\t";
      break;
    case 1:
      cout << "        ready  0\t";
      break;
    case 2:
      cout << "      running  " << document_array[i].remaining_burst + 1 << "\t";
      break;
    case 3:
      cout << "      blocked  " << document_array[i].block_time + 1 << "\t";
      break;
    case -1:
      cout << "   terminated  0\t";
      break;
    default:
      cout << "ERROR: INVALID STATE\t";
      break;
    }
  }
  cout << "\n";
}

int Scheduler::randomOS(int U) {
  int randIntFromList;
  int value;
  randIntFromList = random_numbers[rand_index];
  rand_index++;
  value = ((randIntFromList % U) + 1);
  return value;
}


Scheduler::Scheduler() {

  ifstream random_file("random-numbers.txt");
  string tmp_string;
  while(getline(random_file, tmp_string)){
    random_numbers.push_back(stoi(tmp_string));
  }

  random_file.close();
  rand_index = 0;
  burst = 0;
  any_running = false;
  current_cycle = -1;
  total_processes_terminated = 0;
  finish_time = 0.00;
  total_block_time = 0.00;
  total_wait_time = 0.00;
  total_cpu_time = 0;
  total_turnaround_time = 0.00;
}

void Scheduler::ingest_document(string input_file, bool is_verbose) {
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
      document_array.push_back(process);
    }
  cout << "The original input was: ";
  print_processes();

  sort(document_array.begin(), document_array.end(),
       [](const Process & a, const Process & b) -> bool
       {
         return a.a < b.a;

       });
  cout << endl;

  cout << "The (sorted) input was:   ";
  print_processes();
  cout << endl;
}

void Scheduler::print_output() {
  for (int i = 0; i < document_array.size(); i++) {

    cout << "Process " << i << ":\n";
    cout << "\t(A,B,C,M) = " << "("
         << document_array[i].a << ","
         << document_array[i].b << ","
         << document_array[i].c << ","
         << document_array[i].m << ")\n";
    cout << "\tFinishing Time: " << document_array[i].finish_time;
    total_turnaround_time += (document_array[i].finish_time - document_array[i].a);
    cout << "\n\tTurnaround Time: " << (document_array[i].finish_time - document_array[i].a);
    cout << "\n\tI/O Time: " << document_array[i].time_blocked;
    cout << "\n\tWaiting Time: " << document_array[i].time_waited << "\n\n\n";
  }
}

void Scheduler::print_summary() {
  cout << "Summary Data:";
  cout << "\n\tFinishing Time: " << finish_time;
  cout << "\n\tCPU Utilization: " << total_cpu_time / finish_time;
  cout << "\n\tI/O Utilization: " << total_block_time / finish_time;
  cout << "\n\tThroughput: " << ((document_array.size() / finish_time) * 100) << " processes per hundred cycles";
  cout << "\n\tAverage Turnaround: " << (total_turnaround_time / document_array.size());
  cout << "\n\tAverage Waiting Time: " << (total_wait_time / document_array.size());
  cout << "\n";
}

void Scheduler::change_states(int current_cycle) {
  for (int i = 0; i < document_array.size(); i++) {
    switch(document_array[i].state){
    case 0: // unstarted
      if (document_array[i].a == current_cycle) {
        document_array[i].state = 1;
        document_array[i].rr_wait = 0;
        fcfs_queue.push(i);
      }
      break;
    case 2: // running
      any_running = true;
      // Terminated process comes first
      if (document_array[i].cpu_time <= 0) {
        document_array[i].state = -1;
        document_array[i].finish_time = current_cycle;
        total_processes_terminated++;
        any_running = false;
      }
      // Process to I/O block comes second
      else if (document_array[i].remaining_burst <= 0) {
        document_array[i].state = 3;
        document_array[i].block_time = document_array[i].cpu_burst * document_array[i].m;
        any_running = false;
      }
      // Process Pre-emption comes third
      else if (document_array[i].quantum == 0) {
        document_array[i].state = 1;
        document_array[i].rr_wait = 0;
        any_running = false;
      }
      break;
    case 3: // blocked
      if (document_array[i].block_time <= 0) {
        document_array[i].state = 1;
        document_array[i].rr_wait = 0;
        fcfs_queue.push(i);
      }
      break;
    }
  }
}

void Scheduler::handle_time_change(int current_cycle) {
  bool blocked = false;
  for (int i = 0; i < document_array.size(); i++) {
    switch(document_array[i].state)
      {
      case 1: // ready
        document_array[i].rr_wait++;
        break;
      case 2: // running
        document_array[i].remaining_burst--;
        total_cpu_time++;
        document_array[i].cpu_time--;
        if (document_array[i].quantum > 0) {
          document_array[i].quantum--;
        }
        break;
      case 3: // blocked
        document_array[i].block_time--;
        document_array[i].time_blocked++;
        blocked = true;
        break;
      }
  }
  if (blocked){
    total_block_time++;
  }
}

void Scheduler::decrement_processes()
{
  bool has_waited = false;
  //decrement values for running and blocked processes
  handle_time_change(current_cycle);
  for (int i = 0; i < document_array.size(); i++) {
    if (document_array[i].state == 1) {
      document_array[i].time_waited++;
      has_waited = true;
      total_wait_time++;
    }
  }
}

void Scheduler::FCFS() {
  while (total_processes_terminated < document_array.size()) {
    current_cycle++;
    if(verbose){
      verbose_print(current_cycle, document_array);
    }
    change_states(current_cycle);
    if (any_running == false) {
      if (!fcfs_queue.empty()) {
        int first_come = fcfs_queue.front();
        if(document_array[first_come].state == 1){
          fcfs_queue.pop();

          document_array[first_come].state = 2;
          any_running = true;
          burst = randomOS(document_array[first_come].b);
          document_array[first_come].remaining_burst = burst;
          document_array[first_come].cpu_burst = burst;
        }
      }
    }
    decrement_processes();
  }

  cout << "The Scheduling Algorithm used was First Come First Serve\n\n";
  finish_time = current_cycle;
  print_output();
  print_summary();
}

void Scheduler::SJF() {
  while (total_processes_terminated < document_array.size()) {
    current_cycle++;
    if(verbose){
      verbose_print(current_cycle, document_array);
    }
    change_states(current_cycle);

    if(any_running == false){
      int min = 99999999;
      int shortest_ready_job;
      int not_all_blocked = false;
      for(int i = 0; i < document_array.size(); i++){
        if (document_array[i].cpu_time < min && document_array[i].state == 1) {
          shortest_ready_job = i;
          min = document_array[i].cpu_time;
          not_all_blocked = true;
        }
      }

      if(not_all_blocked){
        document_array[shortest_ready_job].state = 2;
        any_running = true;
        burst = randomOS(document_array[shortest_ready_job].b);
        document_array[shortest_ready_job].remaining_burst = burst;
        document_array[shortest_ready_job].cpu_burst = burst;
      }
    }
    decrement_processes();
  }
  cout << "The Scheduling Algorithm used was Shortest Job First\n\n";
  finish_time = current_cycle;
  print_output();
  print_summary();
}

void Scheduler::HPRN() {
  while (total_processes_terminated < document_array.size()) {
    current_cycle++;
    if(verbose){
      verbose_print(current_cycle, document_array);
    }
    change_states(current_cycle);

    if(any_running == false){
      float max_PR = -9999999;
      int highest_penalty_job;
      int not_all_blocked = false;
      for(int i = 0; i < document_array.size(); i++){
        float PR = (float(current_cycle-document_array[i].a))/float(max(document_array[i].c-document_array[i].cpu_time, 1));

        if (PR > max_PR && document_array[i].state == 1) {
          highest_penalty_job = i;
          max_PR = PR;
          not_all_blocked = true;
        }
      }
      if(not_all_blocked){
        document_array[highest_penalty_job].state = 2;
        any_running = true;
        burst = randomOS(document_array[highest_penalty_job].b);
        document_array[highest_penalty_job].remaining_burst = burst;
        document_array[highest_penalty_job].cpu_burst = burst;
      }
    }
    decrement_processes();
  }
  cout << "The Scheduling Algorithm used was Highest Penalty Ratio Next\n\n";
  finish_time = current_cycle;
  print_output();
  print_summary();
}

void Scheduler::RR() {

  while (total_processes_terminated < document_array.size()) {
    current_cycle++;
    if(verbose){
      verbose_print(current_cycle, document_array);
    }
    change_states(current_cycle);

    int longest_rr_wait = -1;
    int ready_process = -1;

    // Choose who has been waiting it's turn the longest
    // Since we use > for the wait time,
    // the process with an earlier arrival time will run first
    for (int i = 0; i < document_array.size(); i++) {
      if (document_array[i].state == 1 && document_array[i].rr_wait > longest_rr_wait) {
        ready_process = i;
        longest_rr_wait = document_array[i].rr_wait;
      }
    }

    if (any_running == false & ready_process != -1) {
      document_array[ready_process].state = 2;
      any_running = true;
      document_array[ready_process].quantum = 2;

      if (document_array[ready_process].remaining_burst == 0) {
        burst = randomOS(document_array[ready_process].b);
        document_array[ready_process].remaining_burst = burst;
        document_array[ready_process].cpu_burst = burst;
      }
    }

    decrement_processes();
  }
  cout << "The Scheduling Algorithm used was Round Robin\n\n";
  finish_time = current_cycle;
  print_output();
  print_summary();
}

Process::Process
(int a_arg, int b_arg, int c_arg, int m_arg) {
  a = a_arg;
  b = b_arg;
  c = c_arg;
  m = m_arg;
  state = 0;
  time_waited = 0;
  time_blocked = 0;
  cpu_burst = 0;
  remaining_burst = 0;
  cpu_time = c;
  rr_wait = 0;
  quantum = -1;
}
