# Minimum-value-with-threads
Find minimal value from the input file using threads


Input file (intput.dat) contains some multiple of structs called 'Slog' that contains one long data, and one array of 15 elements.
This program concurently extract each Slog and finds minimum element in from array sequnce. Field _cekanje_ simulates waiting for data to be processed.


Compile program with this command
_g++ -pthread --std=c++14 main.cpp_
and run it with _./a.out input.dat_


For generating input files you can use _generator.cpp_. You can compile it with _g++ generator.cpp_, and run it with
_./a.out numberOfSlogs_. For example _./a.out 150_ will generate 150 Slogs in binary input.dat file
