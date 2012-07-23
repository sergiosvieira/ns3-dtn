#include <iostream>
#include "ns3/core-module.h"

using namespace ns3;

class Opt {
public:
	int soma(int a, int b) {
		return a + b;
	}

	int subtrai(int a, int b) {
		return a - b;
	}

};

int main(int argc, char** argv) {
	Opt opt_;
	Callback<int, int, int> opt;

	opt = MakeCallback(&Opt::soma, &opt_);

	std::cout << "operação=" << opt(1, 2)  << "\n";

	opt = MakeCallback(&Opt::subtrai, &opt_);

	std::cout << "operação=" << opt(1, 2)  << "\n";

	return 1;
}
