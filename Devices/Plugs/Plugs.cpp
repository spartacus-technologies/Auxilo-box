#include <iostream>
#include "Plugs.hh"
#include "../../Helpfunctions/helpfunctions.hh"

using namespace std; //cin, cout, string, vector

Plugs::Plugs()
{

}
Plugs::~Plugs()
{

}

bool Plugs::socketOn(const string id)
{
    if(!isSocketOn(id))
    {
        Help::runCommand("sudo sispmctl -o " + id);
    }
}

bool Plugs::socketOff(const string id)
{
    if(isSocketOn(id))
    {
        Help::runCommand("sudo sispmctl -f " + id);
    }
}

bool Plugs::isSocketOn(const string id)
{
    string temp = Help::runCommand("sudo sispmctl -g " + id);
    //Socket is on because the last letter of the output is 'n'
    if(temp[temp.size()-2] == 'n')
    {
        return true;
    }
    return false;
}

/*
bool Plugs::init(){

}
*/