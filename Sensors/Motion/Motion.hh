#ifndef MOTION_HH
#define MOTION_HH

#include <string>

using namespace std;

class Motion
{
    public:
    
    Motion();
    ~Motion();
    bool init();
    
    // bool socketOn(const string id);
    // bool socketOff(const string id);
    // bool isSocketOn(const string id);
};

#endif // MOTION_HH