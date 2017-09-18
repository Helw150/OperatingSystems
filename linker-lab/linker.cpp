// Simple C++ program to display "Hello World"

// Header file for input output functions
#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>
#include <regex>

using namespace std;

// a function to deal with vectors for debugging purposes
void print_vector(vector<string> vector){
  for (auto i = vector.begin(); i != vector.end(); ++i)
    cout <<" |" << *i << "| ";
  return;
}

// a function to deal with unordered maps for debugging purposes
void print_map(unordered_map<string, int> map){
  vector<pair<string,int>> temp_array(map.begin(), map.end());
  sort(temp_array.begin(), temp_array.end());
  for (auto i = temp_array.begin(); i != temp_array.end(); i++)
    cout << i->first << "=" << i->second << "\n";
  cout << "\n";
  return;
}

// print a vector map in the format for grading
void print_symbols(unordered_map<string, int> symbols){
  cout << "Symbol Table\n";
  print_map(symbols);
  return;
}

// a function to print command vectors in appropriate format
void print_memory_map(vector<string> vector){
  int command_index = 0;
  cout << "Memory Map\n";
  for (auto i = vector.begin(); i != vector.end(); ++i)
    {
      cout << command_index << ": " << *i << '\n';
      command_index++;
    }
  return;
}

// A function to add any new definitions in the line to the variable table
unordered_map<string, int> add_definitions(vector<string> line_array, unordered_map<string,int> variable_table, int base_address) {
  for (int i = 0; i < (line_array.size()-2)/2; i++)
    {
      variable_table.emplace(line_array[2*i+1], stoi(line_array[2*i+2])+base_address);
    }
  return variable_table;
}

unordered_map<int, string> local_define(vector<string> line_array, int base_address)
{
  unordered_map<int, string> local_definitions;
  for (int i = 0; i < (line_array.size()-2)/2; i++)
    {

      local_definitions.emplace(base_address+stoi(line_array[2*i+2]), line_array[2*i+1]);
    }
  return local_definitions;
}
// Handle the first pass of use commands
tuple<int,vector<string>> use_command(vector<string> program_array, vector<string>line_array, int base_address){
  int line_length, command, instruction;
  line_length = stoi(line_array[0]);
  for (int i = 1; i <= line_length; i++){
    instruction = stoi(line_array[i]);
    // this gives me just the final digit
    command = instruction-10*(instruction/10);
    string address = to_string((instruction/10));
    switch(command)
      {
      case 1:
        program_array.push_back(address);
        break;
      case 2:
        program_array.push_back(address);
        break;
      case 3:
        address = to_string(base_address+(instruction/10));
        program_array.push_back(address);
        break;
      case 4:
        int stripped_address;
        stripped_address = instruction/10-1000*((instruction/10)/1000);
        if (stripped_address != 777)
          {
          address = to_string(base_address+(instruction/10));
          } else {
          address = to_string((instruction/10));
        }
        program_array.push_back("external "+address);
        break;
      default:
        cout << "This should never occur. If it does, please debug the code" << "\n";
        exit(0);
      }
 }
  base_address += line_length;
  return make_tuple(base_address, program_array);
}

// Ingests and formats the input document
vector<vector<string>> ingest_document(int argc, char *argv[])
{
  if(argc == 1)
    {
      cout << "Please give a filename for input\n";
      exit(-1);
    }
  ifstream infile(argv[1]);
  vector<vector<string>> doc_array;
  vector<string> flat_array;
  string line = "";
  // take every entry into the document and turn it into an array
  while(line == "")
    {
      getline(infile, line);
    }
  while (getline(infile, line))
    {
      if (line == "")
        {
          continue;
        }
      istringstream iss(line);
      // First turn line into vector
      while (iss)
        {
          string subs;
          iss >> subs;
          if (subs != " " && subs != "")
            {
              flat_array.push_back(subs);
            }
        }
    }
  // format flat array into individual lines
  vector<string> line_array;
  int line_counter = 0;
  int i, j;
  for(i = 0; i < flat_array.size(); i++)
    {
      line_counter += 1;
      int line_length = stoi(flat_array[i]);
      int command_length = 1;
      // length of definition and local definition commands is 2
      if(line_counter%3 != 0)
        {
          command_length = 2;
        }
      for(j = 0; j < (line_length*command_length)+1; j++)
        {
          line_array.push_back(flat_array[i+j]);
        }
      line_array.push_back("end");
      i += j-1;
      doc_array.push_back(line_array);
      line_array.clear();
    }
  return doc_array;
}

vector<string> replace_external(int index, string variable_name, vector<string> program_array, unordered_map<int,string> local_definitions, unordered_map<string,int> variable_definitions)
{
  string current = program_array[index];
  vector<string> current_array;
  istringstream iss(current);
  // First turn line into vector
  while (iss)
    {
      string subs;
      iss >> subs;
      current_array.push_back(subs);
    }
  if (current_array[0] != "external"){
    cout << index << " " << "Error here\n";
    exit(-1);
  }
  int next_index = stoi(current_array[1])-1000*(stoi(current_array[1])/1000);
  if (next_index != 777){
    program_array = replace_external(next_index, variable_name, program_array, local_definitions, variable_definitions);
  }
  program_array[index] = to_string(variable_definitions.find(variable_name)->second+1000*(stoi(current_array[1])/1000));
  return program_array;
}

vector<string> second_pass_use(vector<string> program_array, unordered_map<string,int> variable_definitions, unordered_map<int,string> local_definitions)
{
  for (auto i = local_definitions.begin(); i != local_definitions.end(); i++)
    {
      program_array = replace_external(i->first, i->second, program_array, local_definitions, variable_definitions);
    }
  return program_array;
}
tuple<unordered_map<string, int>, vector<string>> first_pass(vector<vector<string>> doc_array)
{
  unordered_map<string, int> variable_table;
  vector<string> program_array;
  int line_counter = 0;
  int base_address = 0;
  unordered_map<int, string> local_definitions;
  for(int i = 0; i < doc_array.size(); i++)
    {
      vector<string> line_array = doc_array[i];
      switch(line_counter%3)
        {
        case 0:
          // Handles the declaration of variables
          variable_table = add_definitions(line_array, variable_table, base_address);
          break;
        case 1:
          // Handles the usage of variables
          break;
        case 2:
          // Handles the module itself
          tie(base_address, program_array) = use_command(program_array, line_array, base_address);
          break;
        default:
          // Handles what should be an impossible error case
          cout << "Something has gone awry. This case shouldn't be possible " << line_counter%3 << "\n";
          exit(-1);
        }
      line_counter++;
    }
  return make_tuple(variable_table, program_array);
}

vector<string> second_pass(vector<vector<string>> doc_array, unordered_map<string,int> variable_table, vector<string> program_array)
{
  int line_counter = 0;
  int base_address = 0;
  unordered_map<int, string> local_definitions;
  for(int i = 0; i < doc_array.size(); i++)
    {
      vector<string> line_array = doc_array[i];
      switch(line_counter%3)
        {
        case 0:
          // Handles the declaration of variables
          break;
        case 1:
          // Handles the usage of variables
          local_definitions = local_define(line_array, base_address);
          break;
        case 2:
          // Handles the module itself
          program_array = second_pass_use(program_array, variable_table, local_definitions);
          int line_length;
          line_length = stoi(line_array[0]);
          base_address += line_length;
          break;
        default:
          // Handles what should be an impossible error case
          cout << "Something has gone awry. This case shouldn't be possible " << line_counter%3 << "\n";
          exit(-1);
        }
      line_counter++;
    }
  return program_array;
}

// main function -
// where the execution of program begins
int main( int argc, char *argv[])
{
  vector<vector<string>> doc_array = ingest_document(argc, argv);
  unordered_map<string, int> variable_table;
  vector<string> program_array;
  tie(variable_table, program_array) = first_pass(doc_array);
  print_symbols(variable_table);
  program_array = second_pass(doc_array, variable_table, program_array);
  print_memory_map(program_array);
  return 0;
}

