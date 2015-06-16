#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <assert.h>
#include <map>
#include "netlist.h"


int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cerr << "You should provide a file name." << std::endl;
		return -1;
	}

//	std::cout << argv[1] << std::endl;
	std::ifstream input_file(argv[1]);
	if (!input_file)
	{
		std::cerr << "I can't read " << argv[1] << "." << std::endl;
		return -1;
	}
	evl_wires wires;
	evl_components components;
	evl_modules module_name;
	

	if (!parse_evl_file(argv[1], module_name, wires, components))
	{
		std::cerr << "theres is an error in parsing " << argv[1] << "." << std::endl;
		return -1;
	}
	evl_wires_table wires_table = make_wires_table(wires);

	// create output file
	std::string output_file_name = argv[1];

	output_file_name +=  ".netlist";
	
	//std::cout <<"main"<< std::endl;
	//std::cout << output_file_name <<std::endl;

	std::ofstream output_file(output_file_name.c_str());

	if (!output_file)
	{
		std::cerr << "I can't write " << output_file_name << "." << std::endl;
		return -1;
	}

	netlist netlist_1;

	if (!netlist_1.create(wires, components))
	{
		return -1;
	}

	netlist_1.netlist_write(output_file, components, module_name);

	//std::cout << argv[1] << ": Info" << ": Ports " << ports.size()
		//<< ", Components " << netlist_1.components_count << ", Assigns 0" << std::endl;

	/*std::cout << argv[1] << ": Info: build module " << ": " << components.size()<< " gates, " << netlist_1.nets_list.size() << " nets, " << netlist_1.pin_count << " pins." << std::endl;

	std::cout << argv[1] << ": Info: Tokens have been written into file: " << output_file_name << std::endl;

	std::cout<<std::endl;*/

}


