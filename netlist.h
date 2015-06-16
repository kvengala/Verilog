#ifndef NETLIST_H
#define NETLIST_H

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <assert.h>


class netlist;
class gate;
class net;
class pin;
struct evl_token   // Structure Evaluate Token 
{
	enum token_type{ NAME, NUMBER, SINGLE };   // token type defined here... 
	token_type type;
	std::string str;
	int LineNo;
}; 

typedef std::list<evl_token> evl_tokens;
struct evl_wire {
	std::string name;
	int width;
}; // struct evl_wire
typedef std::list<evl_wire> evl_wires;
struct evl_statement
{
	enum statement_type { MODULE, WIRE, COMPONENT, ENDMODULE };  // Sequence of token statements type defined here 
	statement_type type;
	evl_tokens tokens;
};
typedef std::list<evl_statement> evl_statements;
struct evl_module
{
	std::string name;
};
typedef std::list<evl_module>evl_modules;
struct evl_pin {
	std::string name;
	int msb, lsb;
}; // struct evl_pin
typedef std::list<evl_pin> evl_pins;
struct evl_component {
	int NoPins;
	std::string type, name;
	evl_pins pins;
}; // evl_component
typedef std::list<evl_component> evl_components;
typedef std::map<std::string, int> evl_wires_table;
//std::string net_name;
evl_wires_table make_wires_table(const evl_wires &wires);


class net
{
	friend class netlist;
	std::list<pin *> connections_;
	void append_pin(pin *p);
	//net() {}
}; // class net

class pin
{
	friend class gate;
	gate *gate_;
	size_t pin_index_;
	std::vector<net *> nets_;

	bool create(gate *g, size_t pin_index, const evl_pin &p, std::map<std::string, net *> &netlist_nets);

}; // class pin

class gate
{
	friend class netlist;
	std::vector<pin *> pins_;

	bool create(const evl_component &c, std::map<std::string, net *> &netlist_nets);
	bool create_pin(const evl_pin &p, size_t pin_index, std::map<std::string, net *> &netlist_nets);

}; // class gate

typedef std::list<net *> evl_nets;

struct gate_pin
{
	std::string type;
	std::string name;
	int pin_position;
}; // struct gate_pin

class netlist
{
	std::list<gate *> gates_;
	std::list<net *> evl_nets_;

public:

	typedef std::list<std::string> string_list;
	struct pin_lis
	{
		std::string name;
		string_list str;
	}; // struct gate_pin

	std::map<std::string, net *> nets_;

	
	string_list nets_list;
	string_list comp_pins_;

	typedef std::list<pin_lis> pins_list;
	pins_list pins_list_;

	int pin_count;
	int wires_count;
	int components_count;

	typedef std::vector<int> vector_int;

	typedef std::vector<std::string> vector_string;

	typedef std::map<std::string, gate_pin> gate_pin_map;
	gate_pin_map gate_pin_map_;

	typedef std::map<std::string, std::string> string_2_list;
	string_2_list predef_gates_type;

	bool create(const evl_wires &wires, const evl_components &comps);
	void create_net(std::string net_name1);
	bool create_nets(const evl_wires &wires);
	bool create_gates(const evl_components &comps);
	bool create_connects(const evl_components &components, const string_list &nets_, gate_pin_map &gate_pin_map_, pins_list &pins_list_);
	void create_pins_list(const evl_pins &pins, vector_string &comp_pins_, vector_int &pin_position);
	bool semantics_error(const std::string &type_, const std::string &name_);
	bool structural_semantics(const std::string &type_, const std::string &name_, const evl_pins &pins_, string_2_list &predef_gates_type);
	void netlist_write(std::ostream &out, const evl_components &components, const evl_modules &mod);

}; // class netlist

bool parse_evl_file(std::string evl_file, evl_modules &module_name, evl_wires &wires, evl_components &comps);
std::string make_net_name(std::string wire_name, int i);

#endif

