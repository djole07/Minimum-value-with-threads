#include <iostream>
#include <random>
#include <functional>
#include <fstream>
#include <algorithm>

using namespace std;

int main(int argc, char** argv){
    if(argc != 2){
        cerr << "Morate specificirati koliko slogova." << endl;
    }
    int n = atoi(argv[1]);
	
    default_random_engine gen;
	
    uniform_real_distribution<double> pdist(-8.0, 8.0);
    uniform_int_distribution<long> tdist(10, 70);

    auto pdie = bind(pdist, gen);
    auto tdie = bind(tdist, gen);
	
    ofstream f("input.dat", ios::out | ios::binary);
	
    for(int i = 0; i < n; i++){
		
        long t = tdie();
	
        f.write((char*)&t, sizeof(long));
		
		for(int i = 0 ; i < 15 ; ++i) {
			double tmp = pdie();
			f.write((char*)&tmp, sizeof(double));
		}
    }
	
    f.close();
	
    return 0;
}
