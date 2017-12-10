#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <queue>
#include "Pager.h"

using namespace std;

// Function as outlined in the lab guidelines to simulate finding the next reference
void Paging::newCycle(int active_process){
  long int MAXINT = 2147483647;
  Process current_process = process_table[active_process];
  int random_number = randomNumber();
  float probability = (float) random_number / (MAXINT + 1.0);
  
  if (probability < current_process.A) {
    process_table[active_process].current_word = (current_process.current_word + 1) % process_size;
  }
  else if (probability < current_process.A + current_process.B) {
    process_table[active_process].current_word = (current_process.current_word - 5 + process_size) % process_size;
  }
  else if (probability < current_process.A + current_process.B + current_process.C) {
    process_table[active_process].current_word = (current_process.current_word + 4) % process_size;
  }
  else {
    random_number = randomNumber();
    process_table[active_process].current_word = random_number % process_size;
  }
}

void Paging::assignReference(int active_process){
  int current_reference = process_table[active_process].current_word/page_size;
  //cout << process_table[active_process].current_word;
  
  bool is_in_table = false;
  // Check if we need to assign the reference to our frame table
  for (int i = 0; i < frame_table.size(); i++){
    if(frame_table[i][0] == current_reference && frame_table[i][1] == active_process){
      //cout<<"found";
      is_in_table = true;
      // Update reference time for LRU
      frame_table[i][3] = run_time;
      return;
    }
  }

  if(!is_in_table){
    //cout << "active_process";
    process_table[active_process].faults += 1;
    // Check if there is a free block and fill the block
    for (int i = frame_table.size()-1; i >= 0 ; i--){
      if(frame_table[i][0] == -1){
	// Set the Values of this frame to the present state
	frame_table[i][0] = current_reference;
	frame_table[i][1] = active_process;
	frame_table[i][2] = 0;
	frame_table[i][3] = run_time;
	// Keep track of the frame for LIFO
	last_in = i;
	return;
      }
    }
    int frame_for_eviction = 0;
    // Go through eviction
    if(replacement_method == "random"){
      // Pull a random number and use it to choose a frame
      int random = randomNumber();
      frame_for_eviction = random % frame_table.size();
    } else if (replacement_method == "lifo") {
      // Simply call the most recently inserted frame to be evicted
      frame_for_eviction = last_in;
    } else if (replacement_method == "lru") {
      // Find the process which has the lowest reference number
      // Lowest reference number indicates the least recently referenced
      int lru_frame = -1;
      int lru_time = 0;
      for(int i = 0; i < frame_table.size(); i++){
	if(lru_frame == -1 || frame_table[i][3] < lru_time){
	  lru_frame = i;
	  lru_time = frame_table[i][3];
	}
      }
      frame_for_eviction = lru_frame;
    } else {
      cout << "Error: Non-existant replacement method";
    }

    // record info about the current evicted process
    process_table[frame_table[frame_for_eviction][1]].residency_time += frame_table[frame_for_eviction][2];
    process_table[frame_table[frame_for_eviction][1]].evictions += 1;
    // replace the process in the frame table
    frame_table[frame_for_eviction][0] = current_reference;
    frame_table[frame_for_eviction][1] = active_process;
    frame_table[frame_for_eviction][2] = 0;
    frame_table[frame_for_eviction][3] = run_time;
    // Keep track of the frame for LIFO
    last_in = frame_for_eviction;
  }
}



void Paging::simulate(){
  int finished_processes = 0;
  while(finished_processes < process_table.size()){
    for(int i = 0; i < process_table.size(); i++){
      for(int tmp = 0; tmp < quantum; tmp++){
	// Only run this loop if the current process is not finished
	if(process_table[i].number_of_references > 0) {
	  assignReference(i);

	  // Use our probabilities to find the new word
	  newCycle(i);
	  // Iterate the residency time of every occupied frame
	  for(int j = 0; j < frame_table.size(); j++){
	    if(frame_table[j][1] != -1){
	      frame_table[j][2]++;
	    }
	  }
	  // Keep track of overall run time
	  run_time++;
	  // Remove a cycle from the process that was just run
	  process_table[i].number_of_references--;
	  if(process_table[i].number_of_references == 0){
	    finished_processes++;
	  }
	}
      }
    }
  }
}

void Paging::outputs(){
  // Print the details that were input into the run
  cout << "The machine size is " << machine_size << ".\n"
       << "The page size is " << page_size << ".\n"
       << "The process size is " << process_size << ".\n"
       << "The job mix number is " << job_type << ".\n"
       << "The number of references per process is " << reference_num << ".\n"
       << "The replacement algorithm is " << replacement_method << ".\n";
  
  cout << "\n----------------------------\n";
  // Print the details of each process and calculate global values
  vector<int> totals(3,0);
  for(int i = 0; i < process_table.size(); i++){
    totals[0] += process_table[i].faults;
    totals[1] += process_table[i].evictions;
    totals[2] += process_table[i].residency_time;
    cout << "Process " << i + 1 << " had " << process_table[i].faults << " faults and ";
    if(process_table[i].evictions){
      cout << (double)process_table[i].residency_time/process_table[i].evictions << " average residency.\n";
    } else {
      cout << "with no evictions, the average residence is undefined.\n";
    }
  }
  cout << "\n----------------------------\n";
  // Print the global values
  cout << "The total number of faults is " << totals[0] << " and ";
  if(totals[1]){
    cout << "the overall average residency is " << (double)totals[2]/totals[1] << ".\n";
  } else {
    cout << "had no evictions, leaving the average residency undefined.\n";
  }
}

/* HELPERS FOR PAGER SETUP */

// This function sets up the processes and probabilities based on the Job Type we are fed
void Paging::jobType(){
  switch(job_type){
  case 1:
    {
      Process single(1, 0, 0, reference_num);
      process_table.push_back(single);
    }
    break;
  case 2:
    {
      Process quadruple(1, 0, 0, reference_num);
      process_table.push_back(quadruple);
      process_table.push_back(quadruple);
      process_table.push_back(quadruple);
      process_table.push_back(quadruple);
    }
    break;
  case 3:
    {
      Process random_quadruple(0, 0, 0, reference_num);
      process_table.push_back(random_quadruple);
      process_table.push_back(random_quadruple);
      process_table.push_back(random_quadruple);
      process_table.push_back(random_quadruple);
    }
    break;
  case 4:
    {
      Process first(0.75, 0.25, 0, reference_num);
      Process second(0.75, 0, 0.25, reference_num);
      Process third(0.75, 0.125, 0.125, reference_num);
      Process fourth(0.5, 0.125, 0.125, reference_num);
      process_table.push_back(first);
      process_table.push_back(second);
      process_table.push_back(third);
      process_table.push_back(fourth);
    }
    break;
  default:
    {
      cout << "Error Invalid Job Type Given\n Assuming Job Type 1\n";
      Process only(1, 0, 0, reference_num);
      process_table.push_back(only);
    }
    break;
  }
}

void Paging::initializeFrames(){
  vector<vector<int>> tmp(machine_size/page_size);
  frame_table = tmp;
  for(int i = 0; i < frame_table.size(); i++){
    // Last Referenced Page Initially None
    frame_table[i].push_back(-1);
    // Current Owner Process Initially None
    frame_table[i].push_back(-1);
    // How Long the the current Process has existed in this frame
    frame_table[i].push_back(0);
    // Last Reference Time Initially None
    frame_table[i].push_back(-1);
  }
}

void Paging::firstWords(){
  for(int i = 0; i < process_table.size(); i++){
    process_table[i].current_word = (111*(i+1)) % process_size;
  }
}
/* END SETUP HELPERS */

/* GENERAL HELPERS */

int Paging::randomNumber() {
  int randIntFromList;
  randIntFromList = random_numbers[rand_index];
  rand_index++;
  return randIntFromList;
}

/* END GENERAL HELPERS */

/* INITIALIZATION OF THE STRUCTS FOR PAGING AND PROCESS*/

Paging::Paging(int M, int P, int S, int J, int N, string R) {
  /* Convert random numbers into a vector */
  ifstream random_file("random-numbers.txt");
  string tmp_string;
  while(getline(random_file, tmp_string)){
    random_numbers.push_back(stoi(tmp_string));
  }
  rand_index = 0;
  random_file.close();
  /* End random number setup */
  machine_size = M;
  page_size = P;
  process_size = S;
  job_type = J;
  reference_num = N;
  replacement_method = R;
  quantum = 3;
  run_time = 0;
  last_in = 0;
  
  jobType();
  initializeFrames();
  firstWords();
}

Process::Process(float a, float b, float c, int reference_num) {
  A = a;
  B = b;
  C = c;
  D = (((1-a)-b)-c);
  number_of_references = reference_num;
  // The total amount of time this process has spent in residency
  residency_time = 0;
  // The Number of Faults this Process has had
  faults = 0;
  // The Number of times this Process has been evicted
  evictions = 0; 
}

/* END STRUCT SETUP */

