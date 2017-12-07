# Building 
The code is written with c++11 in mind. To build, run the following command inside the src directory: 
```bash
c++ ./*.h ./*.cpp -std=c++11
```

# Running
The code expects the only input to be the text file as formatted according to the lab guidelines. Simply run 
```bash
./a.out INPUT_FILE_NAME_HERE
```

# Files
`main.cpp` contains the code that runs at the highest level in extreme abstraction.

`DeadlockDefinitions.cpp` holds the bulk of the functional code of the project.

`Deadlock.h` contains the abstract definitions of the Deadlock class

`Process.h` contains the abstract definitions of the Process class
