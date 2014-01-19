#ifndef PLUGS_HH
#define PLUGS_HH

#include <string>

using namespace std;

class Plugs
{
    public:
    
    Plugs();
    ~Plugs();
    //bool init();
    
    //Ehdottaisin että id on int, mikäli ne ovat numeroita. Helpompi ja selkeämpi kutsua ja 
    //kääntää intti stringiksi täällä, koska sitä varten on jo funktiot olemassa.
    //Jos ne taas pitää olla stringejä niin consti viite olisi kova sana.
    // En koske, koska en kuitenkaan ymmärrä. -vesa
    bool socketOn(const string id);
    bool socketOff(const string id);
    bool isSocketOn(const string id);
};

#endif // PLUGS_HH