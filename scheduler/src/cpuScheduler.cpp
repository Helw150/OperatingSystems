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

void verbose_print(int currentCycle, vector<Process> process_vector, bool verbose) {

  if (verbose) {
    cout << "Before cycle " << setw(5) << currentCycle << ":";
    for (int i = 0; i < process_vector.size(); i++) {
      cout << "\t" << process_vector[i].status;
            if (process_vector[i].status == "unstarted") {
                cout << setw(5) << "0" << setw(5);
            }

            if (process_vector[i].status == "ready") {
                cout << setw(9) << "0" << setw(5);
            }

            if (process_vector[i].status == "running") {
                cout << setw(7) << process_vector[i].cpu_burst_left + 1 << setw(5);
            }

            if (process_vector[i].status == "blocked") {
                cout << setw(7) << process_vector[i].block_time + 1 << setw(5);
            }

            if (process_vector[i].status == "terminated") {
                cout << setw(4) << "0" << setw(5);
            }

        }
        cout << endl;
    }
}


int scheduler::randomOS(int U) {
    int currentNumber;
    int value;

    random_numbers[rand_index];
    rand_index++;
    value = ((currentNumber % U) + 1);
    return value;
}


scheduler::scheduler() {

  ifstream random_file("random-numbers.txt");
  int tmp_number;
  while(random_file >> tmp_number){
    random_numbers.push_back(tmp_number);
  }
  random_file.close();
  //Set initial values for processes
  //Reset initial values for algorithm
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
  currentProcess = 0;
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

        cout << "Process " << i << ":" << endl;
        cout << "(A,B,C,M) = " << "("
        << process_vector[i].arrival_time << ","
        << process_vector[i].max_burst << ","
        << process_vector[i].initial_cpu_time << ","
        << process_vector[i].multi << ")" << endl;
        cout << "Finishing Time: " << process_vector[i].finish_time << endl;          //Finish Time

        total_turnaround_time += (process_vector[i].finish_time
                                  - process_vector[i].arrival_time);

        cout << "Turnaround Time: " << (process_vector[i].finish_time
                                        - process_vector[i].arrival_time) << endl;

        cout << "I/O Time: " << process_vector[i].total_block_time << endl;           //Total time blocked
        cout << "Waiting Time: " << process_vector[i].total_wait_time << endl;        //Total time at ready state

        cout << endl << endl;
    }
}

void scheduler::print_summary() {
    cout << "Summary Data: " << endl;
    cout << "Finishing Time: " << finish_time << endl;
    cout << "CPU Utilization: " << total_cpu_time / finish_time << endl;
    cout << "I/O Utilization: " << total_block_time / finish_time << endl;
    cout << "Throughput: " << ((process_vector.size() / finish_time) * 100) << " processes per hundred cycles" << endl;
    cout << "Average Turnaround: " << (total_turnaround_time / process_vector.size()) << endl;
    cout << "Average Waiting Time: " << (total_wait_time / process_vector.size()) << endl;
}

void scheduler::set_process(int currentCycle) {

    for (int i = 0; i < process_vector.size(); i++) {

        if (process_vector[i].status == "unstarted") {
            if (process_vector[i].arrival_time == currentCycle) {
                process_vector[i].status = "ready";
                process_vector[i].ready_time = 0;
                //append process into ready queue
                ready_queue.push(i);
            }
        }


        else if (process_vector[i].status == "blocked") {
            //If block time is finished - set to ready
            if (process_vector[i].block_time <= 0) {
                process_vector[i].status = "ready";
                process_vector[i].ready_time = 0;
                //append process into ready queue
                ready_queue.push(i);
            }
        }


        else if (process_vector[i].status == "running") {
            any_running = true;
            //If it is finished, set to terminated
            if (process_vector[i].CPU_time <= 0) {
                process_vector[i].status = "terminated";
                process_vector[i].finish_time = currentCycle; //set finished time
                total_processes_terminated++;
                any_running = false;
            }


                //If there is no burst left and it is not finished, then set to block
            else if (process_vector[i].cpu_burst_left <= 0) {
                process_vector[i].status = "blocked";

                //Set block time: cpu_burst * multi
                process_vector[i].block_time = process_vector[i].cpu_burst * process_vector[i].multi;
                any_running = false;
            }
        }
    }
}

void scheduler::run_process(int currentCycle) {
    has_blocked = false;
    for (int i = 0; i < process_vector.size(); i++) {

        //If there is currently a running process. Then decrement that processes cpu_burst_left
        if (process_vector[i].status == "running") {
            process_vector[i].cpu_burst_left = process_vector[i].cpu_burst_left - 1;
            total_cpu_time++;
            process_vector[i].CPU_time = process_vector[i].CPU_time - 1;
        }
        if (process_vector[i].status == "blocked") {
            process_vector[i].block_time = process_vector[i].block_time - 1;
            process_vector[i].total_block_time = process_vector[i].total_block_time + 1;
            has_blocked = true;
        }

        if (process_vector[i].status == "ready") {
            process_vector[i].ready_time = process_vector[i].ready_time + 1;
        }
    }

    if (has_blocked) {
        total_block_time++;
    }
}

void scheduler::Fcfs() {
    //Loop until all processes are terminated
    while (total_processes_terminated != process_vector.size()) {
        currentCycle++;
        verbose_print(currentCycle, process_vector, verbose);

        set_process(currentCycle);

        if (any_running == false) {
            if (!ready_queue.empty()) {
                front_queue = ready_queue.front();
                if (process_vector[front_queue].status == "ready") {
                    ready_queue.pop();

                    process_vector[front_queue].status = "running";
                    any_running = true;
                    cpu_burst = randomOS(process_vector[front_queue].max_burst);
                    process_vector[front_queue].cpu_burst_left = cpu_burst;
                    process_vector[front_queue].cpu_burst = cpu_burst;
                }
            }
        }

        //decrement values for running and blocked processes
        run_process(currentCycle);
        for (int i = 0; i < process_vector.size(); i++) {
            if (process_vector[i].status == "ready") {
                process_vector[i].total_wait_time = process_vector[i].total_wait_time + 1;
                has_waited = true;
            }
        }

        if (has_waited) {
            total_wait_time = total_wait_time + 1;
            has_waited = false;
        }
    }
    cout << "The Scheduling Algorithm used was First Come First Serve\n\n";
    finish_time = currentCycle;
    print_output();
    print_summary();
}

void scheduler::sjf() {
    cout << endl << "=========================Shortest Job First Start======================" << endl << endl;

    while (total_processes_terminated != process_vector.size()) {
        currentCycle++;
        verbose_print(currentCycle, process_vector, verbose);

        set_process(currentCycle);

        temp_vector.clear();
        min = 9999;
        num_ready = 0;
        is_found = false;

        if (any_running == false) {
            for (int i = 0; i < process_vector.size(); i++) {

                if (process_vector[i].status == "ready") {
                    temp_vector.push_back(i);
                }
            }
            while ((is_found == false) && (temp_vector.size() != 0)) {
                for (int i = 0; i < temp_vector.size(); i++) {
                    if (process_vector[temp_vector[i]].CPU_time <= min) {
                        min = process_vector[temp_vector[i]].CPU_time;
                    }
                }
                for (int i = 0; i < temp_vector.size(); i++) {
                  if (min == process_vector[temp_vector[i]].CPU_time) {
                        sjf_queue.push_back(temp_vector[i]);
                        is_found = true;
                        break;
                    }
                }
            }
        }

        if (any_running == false) {
            if (!sjf_queue.empty()) {
                front_queue = sjf_queue.front();
                if (process_vector[front_queue].status == "ready") {
                    sjf_queue.pop_front();

                    process_vector[front_queue].status = "running";
                    any_running = true;
                    cpu_burst = randomOS(process_vector[front_queue].max_burst);
                    process_vector[front_queue].cpu_burst_left = cpu_burst;
                    process_vector[front_queue].cpu_burst = cpu_burst;
                }
            }
        }

        run_process(currentCycle);
        for (int i = 0; i < process_vector.size(); i++) {
            if (process_vector[i].status == "ready") {
                process_vector[i].total_wait_time = process_vector[i].total_wait_time + 1;
                has_waited = true;
            }
        }

        if (has_waited) {
            total_wait_time = total_wait_time + 1;
            has_waited = false;
        }
    }
    cout << "The Scheduling Algorithm used was Shortest Job First" << endl << endl;
    finish_time = currentCycle;
    print_output();
    print_summary();
    cout << endl << "=========================Shortest Job First End========================" << endl << endl;
}

void scheduler::rr() {
    cout << endl << "=========================Round Robin Start=============================" << endl << endl;
    is_found = false;
    min_vec_index = 0;

    while (total_processes_terminated != process_vector.size()) {
        currentCycle++;
        verbose_print(currentCycle, process_vector, verbose);
        for (int i = 0; i < process_vector.size(); i++) {
            if (process_vector[i].status == "unstarted") {
                if (process_vector[i].arrival_time == currentCycle) {
                    process_vector[i].status = "ready";
                    process_vector[i].ready_time = 0;
                }
            }


            else if (process_vector[i].status == "blocked") {
                if (process_vector[i].block_time <= 0) {
                    process_vector[i].status = "ready";
                    process_vector[i].ready_time = 0;
                }
            }


            else if (process_vector[i].status == "running") {
                if (process_vector[i].CPU_time <= 0) {
                    process_vector[i].status = "terminated";
                    process_vector[i].finish_time = currentCycle;
                    total_processes_terminated++;
                    any_running = false;
                }

                else if (process_vector[i].cpu_burst_left <= 0) {
                    process_vector[i].status = "blocked";
                    process_vector[i].block_time = process_vector[i].cpu_burst * process_vector[i].multi;
                    process_vector[i].ready_time = 0;
                    any_running = false;
                }
            }
        }
        for (int i = 0; i < process_vector.size(); i++) {
            if (process_vector[i].quantum == 0) {
                if (process_vector[i].status == "running") {
                    process_vector[i].status = "ready";
                    process_vector[i].ready_time = 0;
                    any_running = false;
                }
            }
        }
        longest_ready_time = -1;
        ready_process = -1;
        for (int i = 0; i < process_vector.size(); i++) {
            if (process_vector[i].status == "ready" && process_vector[i].ready_time > longest_ready_time) {
                ready_process = i;
                longest_ready_time = process_vector[i].ready_time;
            }
        }

        if (any_running == false & ready_process != -1) {
            process_vector[ready_process].status = "running";
            any_running = true;
            process_vector[ready_process].quantum = 2;

            if (process_vector[ready_process].cpu_burst_left == 0) {
              cpu_burst = randomOS(process_vector[ready_process].max_burst);
                process_vector[ready_process].cpu_burst_left = cpu_burst;
                process_vector[ready_process].cpu_burst = cpu_burst;
            }
        }

        for (int i = 0; i < process_vector.size(); i++) {

            if (process_vector[i].status == "running") {
                process_vector[i].quantum = process_vector[i].quantum - 1;
            }
        }

        run_process(currentCycle);

        for (int i = 0; i < process_vector.size(); i++) {
            if (process_vector[i].status == "ready") {
                process_vector[i].total_wait_time = process_vector[i].total_wait_time + 1;
                has_waited = true;
            }
        }

        if (has_waited) {
            total_wait_time = total_wait_time + 1;
            has_waited = false;
        }
    }
    cout << "The Scheduling Algorithm used was Round Robin" << endl << endl;
    finish_time = currentCycle;
    print_output();
    print_summary();
    cout << endl << "=========================Round Robin End=============================" << endl << endl;
}

Process::Process(int a, int b, int c, int m) {
    arrival_time = a;
    max_burst = b;
    initial_cpu_time = c;
    multi = m;
    status = "unstarted";
    cycle = 0;
    total_wait_time = 0;
    total_block_time = 0;
    cpu_burst = 0;
    cpu_burst_left = 0;
    CPU_time = initial_cpu_time;
    turn_around_time = 0;
    ready_time = 0;
}
