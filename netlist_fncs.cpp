#include <iostream>
#include <string>
#include <list>
#include <map>
#include<algorithm>
#include <stddef.h>
#include <sstream>
#include <assert.h>
#include <fstream>

#include "netlist.h"



evl_wires_table make_wires_table(const evl_wires &wires) {
	evl_wires_table wires_table;
	for (evl_wires::const_iterator it = wires.begin();
		it != wires.end(); ++it) {
		wires_table.insert(std::make_pair(it->name, it->width));
	}

	return wires_table;
}

void netlist::create_net(std::string net_name2)
{
	nets_list.push_back(net_name2);
}

bool netlist::create_nets(const evl_wires &wires)
{

	for (evl_wires::const_iterator iter = wires.begin(); iter != wires.end(); ++iter)
	{
		if (iter->width == 1)
		{
			create_net(iter->name);
			//std::cout << iter->name << std::endl;
		}
		else
		{
			for (int i = 0; i < iter->width; ++i)
			{
				std::cout << iter->name << iter->width << std::endl;
				create_net(make_net_name(iter->name, i));
			}
		}
	}
	std::cout << "create nets complete"<< std::endl;
	return true;
}

bool netlist::create_gates(const evl_components &components)
{
	for (evl_components::const_iterator iter_c = components.begin(); iter_c != components.end(); ++iter_c)
	{
		gates_.push_back(new gate);
		gates_.back()->create(*iter_c, nets_);
	}
	std::cout << "create gates complete" << std::endl;
	return true;

}

bool netlist::create(const evl_wires &wires, const evl_components &comps)
{
	bool ret_value;

	create_nets(wires);
	create_gates(comps);

	wires_count = 0;

	ret_value = create_connects(comps, nets_list, gate_pin_map_, pins_list_);

	wires_count = nets_list.size();

	std::cout << ret_value << std::endl;
	return ret_value;
	
}


bool netlist::semantics_error(const std::string &type_, const std::string &name_)
{
	std::cerr << "Error: Number of pins for " << type_ << " gate: " << name_ << " is invalid!" << std::endl;

	return false;
}

bool netlist::structural_semantics(const std::string &type_, const std::string &name_, const evl_pins &pins_, string_2_list &predef_gates_type)
{
	if (type_ == "and")
	{
		if (pins_.size() < 3)
		{
			return semantics_error(type_, name_);
		}

		evl_pins::const_iterator iter_p = pins_.begin(); // The first pin is an output pin

		predef_gates_type[(*iter_p).name] = "output";

		for (evl_pins::const_iterator iter_p = pins_.begin()++; iter_p != pins_.end(); ++iter_p) // All pins except the first are input pins
		{
			predef_gates_type[(*iter_p).name] = "input";
		}
	}

	else if (type_ == "or")
	{
		if (pins_.size() < 3)
		{
			return semantics_error(type_, name_);
		}

		evl_pins::const_iterator iter_p = pins_.begin(); // The first pin is an output pin

		predef_gates_type[(*iter_p).name] = "output";

		for (evl_pins::const_iterator iter_p = pins_.begin()++; iter_p != pins_.end(); ++iter_p) // All pins except the first are input pins
		{
			predef_gates_type[(*iter_p).name] = "input";
		}
	}

	else if (type_ == "xor")
	{
		if (pins_.size() < 3)
		{
			return semantics_error(type_, name_);
		}

		evl_pins::const_iterator iter_p = pins_.begin(); // The first pin is an output pin

		predef_gates_type[(*iter_p).name] = "output";

		for (evl_pins::const_iterator iter_p = pins_.begin()++; iter_p != pins_.end(); ++iter_p) // All pins except the first are input pins
		{
			predef_gates_type[(*iter_p).name] = "input";
		}
	}

	else if (type_ == "not")
	{
		if (pins_.size() != 2)
		{
			return semantics_error(type_, name_);
		}
		evl_pins::const_iterator iter_p = pins_.begin(); // The first pin is an output pin

		predef_gates_type[(*iter_p).name] = "output";

		iter_p = pins_.begin()++;	// The second pin is an input pin

		predef_gates_type[(*iter_p).name] = "input";
	}

	else if (type_ == "buf")
	{
		if (pins_.size() != 2)
		{
			return semantics_error(type_, name_);
		}
		evl_pins::const_iterator iter_p = pins_.begin(); // The first pin is an output pin

		predef_gates_type[(*iter_p).name] = "output";

		iter_p = pins_.begin()++;	// The second pin is an input pin

		predef_gates_type[(*iter_p).name] = "input";
	}

	else if (type_ == "dff")
	{
		if (pins_.size() != 2)
		{
			return semantics_error(type_, name_);
		}

		evl_pins::const_iterator iter_p = pins_.begin(); // The first pin is an output pin

		predef_gates_type[(*iter_p).name] = "output";

		iter_p = pins_.begin()++;	// The second pin is an input pin

		predef_gates_type[(*iter_p).name] = "input";
	}

	else if (type_ == "one")
	{
		if (pins_.size() < 1)
		{
			return semantics_error(type_, name_);
		}

		for (evl_pins::const_iterator iter_p = pins_.begin(); iter_p != pins_.end(); ++iter_p) // All pins are output pins
		{
			predef_gates_type[(*iter_p).name] = "output";
		}
	}

	else if (type_ == "zero")
	{
		if (pins_.size() < 1)
		{
			return semantics_error(type_, name_);
		}

		for (evl_pins::const_iterator iter_p = pins_.begin(); iter_p != pins_.end(); ++iter_p)	// All pins are output pins
		{
			predef_gates_type[(*iter_p).name] = "output";
		}
	}

	else if (type_ == "input")
	{
		if (pins_.size() < 1)
		{
			return semantics_error(type_, name_);
		}

		for (evl_pins::const_iterator iter_p = pins_.begin(); iter_p != pins_.end(); ++iter_p) // All pins are output pins
		{
			predef_gates_type[(*iter_p).name] = "output";
		}
	}

	else if (type_ == "output")
	{
		if (pins_.size() < 1)
		{
			return semantics_error(type_, name_);
		}

		for (evl_pins::const_iterator iter_p = pins_.begin(); iter_p != pins_.end(); ++iter_p)  // All pins are input pins
		{
			predef_gates_type[(*iter_p).name] = "input";
		}
	}
	return true;
}

void netlist::create_pins_list(const evl_pins &pins, vector_string &comp_pins_, vector_int &pin_position)
{
	size_t index = 0;

	for (evl_pins::const_iterator iter_p = pins.begin(); iter_p != pins.end(); ++iter_p)
	{
		if ((*iter_p).msb == -1)		// a 1-bit wire
		{
			comp_pins_.push_back((*iter_p).name);
			pin_position.push_back(index);
		}
		else if ((*iter_p).msb == (*iter_p).lsb)	// a 1-bit wire
		{
			comp_pins_.push_back((make_net_name((*iter_p).name, (*iter_p).msb)));
			pin_position.push_back(index);
		}
		else	// a bus
		{
			for (int i = (*iter_p).lsb; i <= (*iter_p).msb; ++i)
			{
				comp_pins_.push_back((make_net_name((*iter_p).name, i)));
				pin_position.push_back(index);
			}
		}
		index++;
	}
	//std::cout << "create_pinlist done" << std::endl;
}

bool netlist::create_connects(const evl_components &components, const string_list &nets_list_, gate_pin_map &gate_pin_map_, pins_list &pins_list_)
{
	pin_lis pl;
	for (string_list::const_iterator iter_n = nets_list_.begin(); iter_n != nets_list_.end(); ++iter_n)
	{
		string_list evl_pins_n_;
		std::cout << "before the pins_list" << std::endl;
		for (evl_components::const_iterator iter_c = components.begin(); iter_c != components.end(); ++iter_c)
		{
			vector_int pin_position;
			vector_string comp_pins_;
			
			create_pins_list((*iter_c).pins, comp_pins_, pin_position);
			std::cout << iter_c->name<<" " <<iter_c->type<<std::endl;
			std::cout << "after the pins_list" << std::endl;
			string_2_list predef_gates_type;

			/*if (!structural_semantics((*iter_c).type, (*iter_c).name, (*iter_c).pins, predef_gates_type)) //Check for semantics
			{
				return false;
			}*/

			for (size_t i = 0; i < comp_pins_.size(); ++i)
			{
				if (*iter_n == comp_pins_[i])
				{
					gate_pin gate_pin_;
					gate_pin_.name = (*iter_c).name;
					gate_pin_.type = (*iter_c).type;

					gate_pin_.pin_position = pin_position[i];

					std::ostringstream pin_index_str;

					pin_index_str << pin_position[i];

					std::string pin_net_map = *iter_n + (*iter_c).name + (*iter_c).type + pin_index_str.str();
					std::cout << pin_net_map << std::endl;
					evl_pins_n_.push_back(pin_net_map);

					gate_pin_map_.insert(std::make_pair(pin_net_map, gate_pin_));
				}
			}
		}
		pl.name = *iter_n;
		pl.str = evl_pins_n_;
		pins_list_.push_back(pl);
		std::cout << "componenets done" << std::endl;
		//pins_list_.insert(std::make_pair(*iter_n, evl_pins_n_));
	}

	return 1;
}

void netlist::netlist_write(std::ostream &out, const evl_components &components, const evl_modules &mod) // Writing into output file
{
	
	for (evl_modules::const_iterator modit = mod.begin(); modit != mod.end(); ++modit)
	{
		out << "module" << " " << modit->name << std::endl;
	}

	components_count = components.size();

	out << "nets" << " " << nets_list.size() << std::endl; // " " << components.size() << std::endl;
	std::cout << nets_list.size() << std::endl;

	for (pins_list::const_iterator iter_p = pins_list_.begin(); iter_p != pins_list_.end(); ++iter_p)
	{
		out <<"  "<< "net " << iter_p->name << " " << iter_p->str.size() << std::endl;

		for (string_list::const_iterator iter_s = iter_p->str.begin(); iter_s != iter_p->str.end(); ++iter_s)
		{
			if (gate_pin_map_[*iter_s].name != "")
			{
				out << "    " << gate_pin_map_[*iter_s].type << " " << gate_pin_map_[*iter_s].name << " " << gate_pin_map_[*iter_s].pin_position << std::endl;
			}
			else
			{
				out << "    " << gate_pin_map_[*iter_s].type  << " " << gate_pin_map_[*iter_s].pin_position << std::endl;
			}
		}
	}

	std::string pin_name;
	std::list<std::string> pin_names;
	pin_count = 0;
	out << "components" << " " << components.size() << std::endl;
	for (evl_components::const_iterator iter_c = components.begin(); iter_c != components.end(); ++iter_c)
	{
		
		if ((*iter_c).name != "")
		{
			out << "  " << "component " << (*iter_c).type << " " << (*iter_c).name << " " << (*iter_c).pins.size() << std::endl;

			pin_count += (*iter_c).pins.size();
		}
		else{
			out << "  " << "component " << (*iter_c).type << " "  << (*iter_c).pins.size() << std::endl;

			pin_count += (*iter_c).pins.size();
		}

		for (evl_pins::const_iterator iter_p = (*iter_c).pins.begin(); iter_p != (*iter_c).pins.end(); ++iter_p)
		{
			int pin_width;

			if ((*iter_p).msb == -1)
			{
				pin_width = 1;

				out <<"    "<< "pin " << pin_width << " " << (*iter_p).name << std::endl;
			}
			else if ((*iter_p).msb == (*iter_p).lsb)
			{
				pin_width = 1;
				pin_name = make_net_name((*iter_p).name, (*iter_p).msb);
				out <<"    "<< "pin " << pin_width << " " << pin_name << std::endl;
			}
			else
			{
				pin_width = (*iter_p).msb - (*iter_p).lsb + 1;
				out <<"    "<< "pin " << pin_width;
				for (int i = (*iter_p).lsb; i <= (*iter_p).msb; ++i)
				{
					std::string pin_names_temp;
					pin_names_temp = make_net_name((*iter_p).name, i);

					pin_names.push_back(pin_names_temp);
					out << " " << pin_names_temp;
				}

				out << std::endl;

			}
		}
	}
}

bool gate::create(const evl_component &component, std::map<std::string, net *> &netlist_nets)
{
	// store gate type and name;
	size_t pin_index = 0;

	for (evl_pins::const_iterator iter_p = component.pins.begin(); iter_p != component.pins.end(); ++iter_p)
	{
		create_pin(*iter_p, pin_index, netlist_nets);
		++pin_index;
	}
	return 1;
}

bool gate::create_pin(const evl_pin &p, size_t pin_index, std::map<std::string, net *> &netlist_nets)
{
	pins_.push_back(new pin);

	return pins_.back()->create(this, pin_index, p, netlist_nets);
}

std::string make_net_name(std::string wire_name, int i)
{
	//std::cout << i << std::endl;
	assert(i >= 0);

	std::ostringstream oss;

	oss << wire_name << "[" << i << "]";

	return oss.str();
}

bool pin::create(gate *g, size_t pin_index, const evl_pin &p, std::map<std::string, net *> &netlist_nets)
{
	//  store g and pin_index;

	if (p.msb == -1) // a 1-bit wire
	{
		nets_.push_back(netlist_nets[p.name]);
	}
	else   // a bus
	{
		//size_t index = 0;

		for (int i = p.lsb; i <= p.msb; ++i)
		{
			nets_.push_back(netlist_nets[make_net_name(p.name, i)]);
		}
	}
	return true;
}

void net::append_pin(pin *p)
{
	connections_.push_back(p);

}

bool pin_valid(evl_wires &wires, evl_pin &p) // Pin Validation 
{
	int valid = 0;
	for (evl_wires::const_iterator it = wires.begin(); it != wires.end(); ++it) {
		if ((it->name) == (p.name)) {
			valid = 1;
		}
	}
	if (!valid)
		return false;
	else
		return true;
}

bool pin_valid_bus(evl_wires &wires, evl_pin &p)
{
	int valid = 0;
	for (evl_wires::const_iterator it = wires.begin(); it != wires.end(); ++it) {
		if ((it->name) == (p.name)) {
			valid = 1;
			if (it->width > 1) {
				p.msb = it->width - 1;
				p.lsb = 0;
			}
		}
	}
	if (!valid)
		return false;
	else
		return true;
}

bool extract_tokens_from_line(std::string line, int LineNo, evl_tokens &tokens)
{
	for (size_t i = 0; i < line.size();)
	{
		//std::cout << line[i]<<std::endl;

		if (line[i] == '/')
		{
			++i;
			if ((i == line.size()) || (line[i] != '/'))
			{
				std::cerr << "LINE " << LineNo
					<< ": a single / is not allowed" << std::endl;
				return false;
			}
			break; // skip the rest of the line by exiting the loop
		}
		else if (isspace(line[i]))   // Is space Function is Used here 
		{
			++i;
		}
		else if ((line[i] == '(') || (line[i] == ')')
			|| (line[i] == '[') || (line[i] == ']')
			|| (line[i] == ':') || (line[i] == ';')
			|| (line[i] == ','))
		{

			evl_token token;
			token.LineNo = LineNo;
			token.type = evl_token::SINGLE;
			token.str = std::string(1, line[i]);
			tokens.push_back(token);
			++i;
		}
		else if (isalpha(line[i]) || (line[i] == '_')
			|| (line[i] == '\\') || (line[i] == '.'))
		{
			size_t name_begin = i;
			for (++i; i < line.size(); ++i)
			{
				if (!(isalpha(line[i]) || isdigit(line[i])
					|| (line[i] == '_') || (line[i] == '\\')
					|| (line[i] == '.')))
				{
					break; 	// [name_begin, i) is the range for the token
				}
			}
			evl_token token;
			token.LineNo = LineNo;
			token.type = evl_token::NAME;
			token.str = line.substr(name_begin, i - name_begin);
			tokens.push_back(token);

		}
		else if (isdigit(line[i]))
		{
			size_t number_begin = i;
			for (++i; i<line.size(); ++i)
			{
				if (!isdigit(line[i]))
				{
					break; 	// [number_begin, i) is the range for the token
				}
			}
			evl_token token;
			token.LineNo = LineNo;
			token.type = evl_token::NUMBER;
			token.str = line.substr(number_begin, i - number_begin);
			tokens.push_back(token);
		}
		else
		{
			std::cerr << "LINE " << LineNo
				<< ": invalid character" << std::endl;
			return false;
		}
	}
	return true; // Done here
}

bool extract_tokens_from_file(std::string file_name,  // Extracint Tokens From the File 
	evl_tokens &tokens)
{
	std::ifstream input_file(file_name.c_str());
	if (!input_file)
	{
		std::cerr << "I can't read " << file_name << "." << std::endl;
		return false;
	}
	//std::cout << "insidefncs-extract_tokens_from_file" << std::endl;
	tokens.clear();

	std::string line;
	for (int LineNo = 1; std::getline(input_file, line); ++LineNo)
	{
		//std::cout << "insidefor_loop-extract_tokens_from_line" << std::endl;
		if (!extract_tokens_from_line(line, LineNo, tokens))
		{
			//std::cout << "insidefncs-extract_tokens_from_line" << std::endl;
			return false;
		}
	}
	return true;
}
bool store_tokens_to_file(std::string file_name,  // Store Tokens to Output file with extension .token
	const evl_tokens &tokens)

{
	std::ofstream output_file(file_name.c_str());
	if (!output_file)
	{
		std::cerr << "I can't write into file " << file_name << "."
			<< std::endl;
		return false;
	}
	//		display_tokens(output_file, tokens);
	return true;
}

bool token_is_semicolon(const evl_token &token) // Semicolon Testing or Predication for Statements 
{
	if (token.str == ";")
	{
		return true;
	}
	else
		return false;
}

bool move_tokens_to_statement(evl_tokens &statement_tokens, evl_tokens &tokens)
{
	assert(statement_tokens.empty());
	assert(!tokens.empty());
	evl_tokens::iterator next_sc = std::find_if(tokens.begin(), tokens.end(), token_is_semicolon);
	if (next_sc == tokens.end())
	{
		std::cerr << "Look for ';' but reach the end of file" <<
			std::endl;
		return false;
	}
	++next_sc;
	//statement_tokens.splice(statement_tokens.begin(), tokens, tokens.begin(), next_sc);
	statement_tokens.splice(statement_tokens.end(), tokens, tokens.begin(), next_sc);
	return true;
}

bool group_tokens_into_statements(evl_statements &statements, evl_tokens &tokens)
{
	assert(statements.empty());
	for (; !tokens.empty();)
	{
		//std::cout << "insidegroup tokensinto statements" << std::endl;
		// Generate one token per iteration
		evl_token token = tokens.front();
		if (token.type != evl_token::NAME)
		{
			std::cerr << "Need a NAME token but found '" << token.str
				<< "'on line" << token.LineNo << std::endl;
			return false;
		}
		if (token.str == "module")
		{						// MODULE statement
			evl_statement module;
			module.type = evl_statement::MODULE;
			if (!move_tokens_to_statement(module.tokens, tokens))
				return false;
			statements.push_back(module);
		}
		else if (token.str == "endmodule")
		{                 // ENDMODULE statement
			evl_statement endmodule;

			endmodule.type = evl_statement::ENDMODULE;
			endmodule.tokens.push_back(token);

			tokens.erase(tokens.begin());
			statements.push_back(endmodule);;
		}
		else if (token.str == "wire")
		{				// WIRE statement

			evl_statement wire;
			wire.type = evl_statement::WIRE;
			if (!move_tokens_to_statement(wire.tokens, tokens))
				return false;
			statements.push_back(wire);
		}
		else
			//		COMPONENT statement
		{
			evl_statement component;
			component.type = evl_statement::COMPONENT;
			if (!move_tokens_to_statement(component.tokens, tokens))
				return false;
			statements.push_back(component);
		}
	}
	return true;
}


bool process_wire_statement(evl_wires &wires, evl_statement &s)
{
	evl_wire ew;
	//names nm;
	assert(s.type == evl_statement::WIRE);
	enum state_type {
		INIT, WIRE, DONE, WIRES, WIRE_NAME,
		BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE
	};
	state_type state = INIT;
	int Bus_Width = 1;
	for (; !s.tokens.empty() && (state != DONE); s.tokens.pop_front())
	{
		evl_token t = s.tokens.front();
		//  Consume one token at a time and Start with INIT State 
		if (state == INIT)
		{
			if (t.str == "wire")
			{
				state = WIRE;
			}
			else
			{
				std::cerr << "Need 'wire' but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}
		else if (state == WIRE)
		{
			if (t.type == evl_token::NAME)
			{
				ew.width = Bus_Width;
				ew.name = t.str;
				wires.push_back(ew);
				state = WIRE_NAME;
			}

			else if (t.str == "[")
			{
				state = BUS;
			}

			else {
				std::cerr << "Need NAME but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}
		else if (state == WIRES)
		{
			if (t.type == evl_token::NAME)
			{
				ew.width = Bus_Width;
				ew.name = t.str;
				//std::cout << "wire" << ew.name << std::endl;
				wires.push_back(ew);
				//wires[t.str] = Bus_Width;
				state = WIRE_NAME;
			}

			else
			{
				std::cerr << "Need NAME but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}
		else if (state == WIRE_NAME)
		{
			if (t.str == ",")
			{
				state = WIRES;
			}
			else if (t.str == ";")
			{
				state = DONE;
			}
			else
			{
				std::cerr << "Need ',' or ';' but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}

		}

		else if (state == BUS)
		{
			if (t.type == evl_token::NUMBER)
			{
				Bus_Width = atoi(t.str.c_str()) + 1; //changed
				//Bus_Width = atoi(t.str.c_str());

				state = BUS_MSB;
			}
			else
			{
				std::cerr << "Need NUMBER but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}

		else if (state == BUS_MSB)
		{
			if (t.str == ":")
			{
				state = BUS_COLON;
			}
			else
			{
				std::cerr << "Need ':' but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}

		else if (state == BUS_COLON)
		{
			if (t.str == "0")
			{
				state = BUS_LSB;
			}
			else
			{
				std::cerr << "Need '0' but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}

		else if (state == BUS_LSB)
		{
			if (t.str == "]")
			{
				state = BUS_DONE;
			}
			else
			{
				std::cerr << "Need ']' but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}

		else if (state == BUS_DONE)
		{
			if (t.type == evl_token::NAME)
			{
				//wires.insert(std::make_pair(t.str, Bus_Width)); --- changes made
				//nm.push_back(t.str); ---changes made
				ew.width = Bus_Width;
				ew.name = t.str;
				wires.push_back(ew);
				state = WIRE_NAME;

			}

			else
			{
				std::cerr << "Need NAME but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}

		else
		{
			assert(false); // An Error if occurs here
		}
	}
	if (!s.tokens.empty() || (state != DONE))
	{
		std::cerr << "something wrong with the statement" << std::endl;
		return false;
	}
	//std::cout << "wire functions " << std::endl;
	return true;
}


bool process_component_statement(evl_components &components,
	evl_statement &s, evl_wires &wires)
{
	assert((!(s.type == evl_statement::WIRE))
		&& (!(s.type == evl_statement::MODULE))
		&& (!(s.type == evl_statement::ENDMODULE)));
	enum state_type {
		INIT, TYPE, NAME, PINS, PIN_NAME,
		BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE, PINS_DONE, DONE
	};
	state_type state = INIT;
	evl_component comp;
	evl_pin pin;
	int no_ofpins = 0;

	for (; !s.tokens.empty() && (state != DONE); s.tokens.pop_front())
	{
		evl_token t = s.tokens.front();
		//  Starts computation with INIT state 

		if (state == INIT)
		{

			if (t.type == evl_token::NAME)
			{
				comp.type = t.str;
				//comp.name = "NONE";
				state = TYPE;
			}
			else {
				std::cerr << "Need NAME but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}

		else if (state == TYPE)
		{
			if (t.type == evl_token::NAME)
			{
				comp.name = t.str;
				state = NAME;
			}
			else if (t.str == "(")
			{
				state = PINS;
			}
			else {
				std::cerr << "Need NAME or '(' but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}

		else if (state == NAME)
		{
			if (t.str == "(")
			{
				state = PINS;
			}
			else {
				std::cerr << "Need '(' but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}

		else if (state == PINS)
		{
			if (t.type == evl_token::NAME)
			{
				pin.name = t.str;
				pin.msb = -1; pin.lsb = -1;

				state = PIN_NAME;

			}

			else
			{
				std::cerr << "Need NAME but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}

		else if (state == PIN_NAME)
		{
			if (t.str == ",")
			{
				if (!pin_valid_bus(wires, pin))
				{
					std::cerr << "The pin is not valid, check it out" << std::endl;
				}
				comp.pins.push_back(pin);
				no_ofpins++;
				state = PINS;
			}
			else if (t.str == ")")
			{
				if (!pin_valid_bus(wires, pin))
				{
					std::cerr << "The pin is not valid, check it out" << std::endl;
				}
				comp.pins.push_back(pin);
				//no_ofpins++;
				state = PINS_DONE;
			}
			else if (t.str == "[")
			{
				state = BUS;
			}
			else
			{
				std::cerr << "Need ',' or ')' or '[' but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}//

		else if (state == BUS)
		{
			if (t.type == evl_token::NUMBER)
			{
				pin.msb = atoi(t.str.c_str());
				pin.lsb = atoi(t.str.c_str());
				state = BUS_MSB;
			}
			else
			{
				std::cerr << "Need NUMBER but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}

		else if (state == BUS_MSB)
		{
			if (t.str == ":")
			{
				state = BUS_COLON;
			}
			else if (t.str == "]")
			{
				state = BUS_DONE;
			}
			else
			{
				std::cerr << "Need ':' or ']' but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}

		else if (state == BUS_COLON)
		{
			if (t.type == evl_token::NUMBER)
			{
				pin.lsb = atoi(t.str.c_str());
				state = BUS_LSB;
			}
			else
			{
				std::cerr << "Need NUMBER but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}

		else if (state == BUS_LSB)
		{
			if (t.str == "]")
			{
				state = BUS_DONE;
			}
			else
			{
				std::cerr << "Need ']' but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
		}

		else if (state == BUS_DONE)
		{
			if (t.str == ")")
			{

				state = PINS_DONE;

			}
			else if (t.str == ",")
			{
				state = PINS;

			}
			else
			{
				std::cerr << "Need ')' or ',' but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}
			if (!pin_valid(wires, pin))
			{
				std::cerr << "The pin is not valid, check it out" << std::endl;
			}

			comp.pins.push_back(pin);
			no_ofpins++;
		}
		else if (state == PINS_DONE)
		{
			if (t.str == ";")
			{
				state = DONE;
			}
			else
			{
				std::cerr << "Need ';' but found '" << t.str
					<< "' on line " << t.LineNo << std::endl;
				return false;
			}

		}
		else if (state == DONE)
		{
			comp.NoPins = no_ofpins;
			return true;
		}
		else
		{
			assert(false); // shouldn't reach here
		}

	}
	components.push_back(comp);

	if (!s.tokens.empty() || (state != DONE))
	{
		std::cerr << "something wrong with the statement" << std::endl;
		return false;
	}
	//std::cout << "inside components" << std::endl;
	return true;
}



bool process_module_statement(evl_modules &modules, evl_statement &s)
{
	evl_module module;
	for (; !s.tokens.empty(); s.tokens.pop_front())
	{
		evl_token t = s.tokens.front();
		if (t.type == evl_token::NAME)
		{
			module.name = t.str;
		}
		else
		{
			break;
		}

	}
	modules.push_back(module);
	std::cout <<"module" << module.name<<std::endl;
	return true;
}

bool parse_evl_file(std::string evl_file, evl_modules &module_name, evl_wires &wires, evl_components &comps)
{

	evl_tokens tokens;
	if (!extract_tokens_from_file(evl_file, tokens))  // Extract token From the file given into tokens
	{
		std::cout << "inside-extract_tokens_from_file"<<std::endl;
		return false;
	}
	//	display_tokens(//std::cout, tokens);

	evl_statements statements;
	std::cout << "after-extract_tokens_from_file" << std::endl;
	if (!group_tokens_into_statements(statements, tokens))
	{
		return false;
	}
	//std::ofstream output_file((evl_file + ".syntax").c_str());    // Output Requirement With Syntax extension 

	for (evl_statements::iterator it = statements.begin();
		it != statements.end(); ++it)
	{

		if ((*it).type == evl_statement::MODULE)
		{
			if (!process_module_statement(module_name, (*it)))
			{
				return false;
			}
		}
		else if ((*it).type == evl_statement::WIRE)
		{
			if (!process_wire_statement(wires, (*it)))
			{
				return false;
			}
		}

		else if ((*it).type == evl_statement::COMPONENT)
		{
			if (!process_component_statement(comps, *it, wires))
			{
				return false;

			}
		}

		else
		{
			return true;
		}
	}
	std::cout << "parse is done" << std::endl;
	return true;
}


