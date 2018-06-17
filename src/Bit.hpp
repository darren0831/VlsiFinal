#ifndef VLSI_FINAL_PROJECT_BIT_HPP_
#define VLSI_FINAL_PROJECT_BIT_HPP_
#include <vector>
#include <string>
#include "Pin.hpp"
class Bit {
public:
	Bit(std::string _name) {
		name = _name;
	}
	std::vector<Pin> pins;
	std::string name;
	void addPin(Pin p) {
		pins.push_back(p);
	}
	std::string print()
	{
		std::string s="";
		s+=name+"\n";
		for(unsigned i=0;i<pins.size();i++)
		{
			s+=std::string("\t")+pins[i].print()+std::string("\n");
		}
		return s;
	}

};

#endif
