#include <iostream>
#include <string>

#include "helpfunctions.hh"

using namespace std;
using namespace Help;

int main(int argc, char const *argv[])
{
	string tulos = runCommand("cat testi");
	 cout << tulos << endl;
	return 0;
}