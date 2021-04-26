
#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <list>
#include <fstream>
#include <vector> 

using namespace std;
using namespace chrono;
using namespace this_thread;

const milliseconds READ_INTERVAL(2);
const unsigned DUZINA_NIZA(15);
const unsigned MAX_CAPACITY(7);
const unsigned MAX_NITI(4);


struct Slog {
    long cekanje;
    double niz[DUZINA_NIZA];
};

class SinhronaVrednost {
private:

    double minMaxValue;
    
    mutex m;

    vector<double> minimumVector;   //  minumalnu vrednost svakog sloga stavljamo u vektor, radi debug-a


public:
    SinhronaVrednost() {
        minMaxValue = -numeric_limits<double>::max();        
    }

    void pushMinimum(Slog s) {
        unique_lock<mutex> l(m);

        this_thread::sleep_for(milliseconds(s.cekanje));

        double tmpMin = s.niz[0];

        for(int i = 1 ; i < DUZINA_NIZA ; ++i) {
            if(s.niz[i] < tmpMin) {
                tmpMin = s.niz[i];
            }
        }
        
        
        if(tmpMin > minMaxValue) {
            minMaxValue = tmpMin;
        }


        minimumVector.push_back(tmpMin);
        //cout << "Nit je dodala vrednost " << tmpMin << " u vektor" << endl;

    }

    double getMaxMin() {    // Minimax :)

        return minMaxValue;
    }

    void printAllMin() {    //  opciona metoda za stampanje svih minimalnih elemenata po slogovima
        vector<double>::const_iterator it = minimumVector.begin();
        
        cout << endl;

        while(it != minimumVector.end()) {
            cout << *it << " ";
            ++it;
        }

        cout << "Vektor ima " << minimumVector.size() << " clanova" << endl;
    }
};

class RedCekanja {
private:
    
    int remCapacity;
    int nitiNaCekanju;

    mutex m;

    list<Slog> lista;
    
    condition_variable punRed;
    condition_variable prazanRed;
    //condition_variable zaostaleNiti;  moze i bez date varijable, ali ukoliko se otkomentarisu linije gde se koristi,
    //                                  program ce biti za nijansu razumljiviji

    bool endThread;

public:

    RedCekanja(int kapacitet) : remCapacity(kapacitet), endThread(false), nitiNaCekanju(0) {

    }

    Slog getSlog() {
        unique_lock<mutex> l(m);

        while(remCapacity == MAX_CAPACITY) {
            ++nitiNaCekanju;
            cout << "Ooops, lista je trenutno prazna. Na podatke ceka " << nitiNaCekanju << " niti" << endl;

            prazanRed.wait(l);  // cekamo da neka od niti preuzme podatak iz liste ne bi li se uneo sledeci

            --nitiNaCekanju;

        }


        Slog tmp;
        tmp = lista.back();
        lista.pop_back();

        ++remCapacity;

        //zaostaleNiti.notify_one();
        punRed.notify_one();    // posto smo oslobodili novo mesto u listi, mozemo ubaciti sledeci element u listu
            // dati notify se moze koristi kasnije i kada budemo cekali da sve ostale niti zavrse izvrsavanje

        //cout << "Nit je uspesno uzela Slog za obradu" << endl;
        return tmp;

    }

    void putSlog(Slog s) {
        unique_lock<mutex> l(m);

        while(remCapacity == 0) {
            cout << "Nit trenutno ceka na upis u listu" << endl;
            punRed.wait(l);
            
        }

        lista.push_back(s);
        --remCapacity;

        prazanRed.notify_one(); //  obavestamo da je trenutno upisan novi Slog u listu, pa lista vise nije prazna

    }

    void checkNitiNaCekanju() {
        unique_lock<mutex> l(m);

        while(!(remCapacity  == MAX_CAPACITY)) {    // cekamo dok se se lista ne isprazni, a to znaci da smo obradili sve podatke
            //zaostaleNiti.wait(l);
            punRed.wait(l);

        }
    }

    int getNitiNaCekanju() {
        return nitiNaCekanju;

    }

    void setEndFlag() {
        endThread = true;

    }

    bool getEndFlag() {
        return endThread;

    }

    
};

void ucitava(char* fname, RedCekanja& rc, SinhronaVrednost& v) {

    ifstream input(fname, ios::in | ios::binary);

    long cekanje;

    while(1) {
        
        sleep_for(READ_INTERVAL);   // simuliramo cekanje od 2ms koliko traje citanje

        if(input.eof()) {
            break;      // ukoliko stignemo do kraja fajla, onda zavrsavamo sa citanjem fajla
        }
        
        Slog tmpSlog;
        input.read((char *)&cekanje, sizeof(long));
        tmpSlog.cekanje = cekanje;


        for(int i = 0 ; i < DUZINA_NIZA ; ++i) {
            double tmpVrednost;
            input.read((char *)&tmpVrednost, sizeof(double));
            tmpSlog.niz[i] = tmpVrednost;
        }

        rc.putSlog(tmpSlog);

    }

    input.close();

    cout << "Kraj fajla!" << endl;

    rc.checkNitiNaCekanju();    // cekamo da se lista isprazni i da nemamo vise podataka za obradu

    rc.setEndFlag();            // postavljamo flag koji ce terminirati sve niti iz petlje u funkciji obrada

}

void obrada(RedCekanja& rc, SinhronaVrednost& v) {

    while(rc.getEndFlag() == false) {
        Slog tmpSlog;

        tmpSlog = rc.getSlog();

        v.pushMinimum(tmpSlog);
        
    }
}

int main(int argc, char** argv) {
    if(argc != 2) {
        cout << "Navesti ime ulaznog fajla." << endl;
        exit(1);
    }


    RedCekanja rc(MAX_CAPACITY);
    SinhronaVrednost sv;

    thread nitUcitavac(ucitava, argv[1], ref(rc), ref(sv)); // puni listu za Slogovima
    thread nitiObradjivaci[MAX_NITI];


    for(int i = 0 ; i < MAX_NITI ; ++i) {
        nitiObradjivaci[i] = thread(obrada, ref(rc), ref(sv));
    }


    nitUcitavac.join();

    for(int i = 0 ; i < MAX_NITI ; ++i) {
        nitiObradjivaci[i].join();
    }


    //sv.printAllMin(); // opciono stampanje svih minimalnih elemenata po slogovima iz datoteke

    cout << endl << "Najveci najmanji element je " << sv.getMaxMin() << endl;

    return 0;
}