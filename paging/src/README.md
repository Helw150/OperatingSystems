# Building 
The code is written with c++11 in mind. To build, run the following command inside the src directory: 
```bash
c++ ./*.h ./*.cpp -std=c++11
```

If the code is run outside the src directory, it will not have access to the random-numbers file and will fail.

# Running
The code expects the only input to be the text file as formatted according to the lab guidelines. Simply run 
```bash
./a.out M P S J N R (debug does nothing)
```

# Files
`main.cpp` contains the code that runs at the highest level in extreme abstraction.

`PagingDefinitions.cpp` holds the bulk of the functional code of the project.

`Pager.h` contains the abstract definitions of the Pager class

`Process.h` contains the abstract definitions of the Process class
