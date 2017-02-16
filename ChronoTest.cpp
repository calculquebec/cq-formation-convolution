#include <string>
#include <vector>
#include <iostream>
#include "Chrono.hpp"

using namespace std;

int main(void) {
    Chrono lChrono(false);
    string x = "";
    cout << "Testing the Chrono class interface" << endl;
    cout << "The resolution on this machine is: " << lChrono.getRes() << " sec" << endl;
    lChrono.resume();
    double lCount, lBest = lChrono.get();
    for(int i=0; i<1000000; ++i) {
        lChrono.reset();
        lCount = lChrono.get();
        if(lCount < lBest) lBest = lCount;
    }
    cout << "The shortest measured time is " << lBest << " sec" << endl;
    lChrono.reset(true);
    cout << "Choose between 'get', 'pause', 'resume', or 'reset'" << endl;
    while(x != "quit") {
        lCount = lChrono.get();
        cout << "Count = " << lCount << " sec" << endl;
        cin >> x;
        if(x == "pause") {
            lChrono.pause();
        } else if(x == "reset") {
            lChrono.reset();
        } else if(x == "resume") {
            lChrono.resume();
        } else if(x != "get") {
            cout << "Commande '" << x << "' invalide" << endl;
        }
    }
    return 0;
}
