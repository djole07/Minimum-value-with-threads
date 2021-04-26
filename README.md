# Minimum-value-with-threads
Find minimal value from the input file using threads


Input file (intput.dat) contains some multiple of structs called 'Slog' that contains one long data, and one array of 15 elements.
This program concurently extract each Slog and finds minimum element in from array sequnce. Field /*cekanje*/ simulates waiting for data to be processed.


Compile program with this command
/*g++ -pthread --std=c++14 main.cpp*/
and run it with /*./a.out input.dat*/


For generating input files you can use /*generator.cpp*/. You can compile it with /*g++ generator.cpp*/, and run it with
/*./a.out numberOfSlogs*/. For example /*./a.out 150*/ will generate 150 Slogs in binary input.dat file
