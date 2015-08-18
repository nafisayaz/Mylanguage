

#include<iostream>
#include<fstream>
#include<vector>


void ltrim(std::string &);
void rtrim(std::string &);

void rtrim(std::string & line)
{
	for(int i=line.size(); i>0; ++i)
	{
		if(line[i] != ' ')
			break;
		line.erase(i,1);
	}
}

void ltrim(std::string & line)
{
	for(int i=0; i<line.size(); ++i)
	{
		if(line[i] != ' ')
			break;
		line.erase(i,1);
	}
}

void trim(std::string & line)
{
	ltrim(line);
	rtrim(line);
}

std::vector<std::string> tokenize(const std::string & line, bool & is_semicolon)
{
	if(line.back() != ';' )
	{
		is_semicolon = false;
		return {};
	}
	std::vector<std::string> tokens;
	std::string str;
	int i=0;
	auto skipws = [&] {
		for(++i; line[i] == ' '; ++i);
	};
	while( i<line.size() )
	{
		switch(line[i])
		{
			case ' ':
				if(not str.empty())
					tokens.push_back(str);
				str = "";
				skipws();
				--i;
				break;

			case '=':
				if( not str.empty() )
					tokens.push_back(str);
				tokens.push_back("=");
				str = "";
				break;

			case '(':
				if( not str.empty() )
					tokens.push_back(str);
				tokens.push_back("(");
				str = "";
				break;

			case ')':
				if( not str.empty() )
					tokens.push_back(str);
				tokens.push_back(")");
				str = "";
				break;

			case '"':
				if( not str.empty() )
					tokens.push_back(str);
				str.clear();
				str = '"';
				++i;
				while(i<line.size())
				{
					str += line[i];
					if(line[i] == '"')
						break;
					++i;
				}
				tokens.push_back(str);
				str = "";
				break;

			default:
				str += line[i];
		}
		++i;		
	}
	return tokens;
}

bool is_print(const std::vector<std::string> & tokens)
{
	if(tokens.size() != 4)
		return false;
	
	return (tokens[0] == "print" or tokens[0] == "println" and tokens[1] == "(" and tokens[3] == ")" );
}

bool is_scan(const std::vector<std::string> & tokens)
{
	if(tokens.size() != 6)
		return false;
	
	return (tokens[1] == "=" and tokens[2] == "scan" and tokens[3] == "(" and tokens[5] == ")" );

}

bool print_function(const std::vector<std::string> & tokens, std::ofstream & out_file)
{
	if (not is_print(tokens) )
		return false;
	out_file << "\n\tstd::cout<<"<<tokens[2];
	if(tokens[0] == "println")
		out_file << "<<std::endl;";
	else
		out_file << ';';
	return true;
}

bool scan_function(const std::vector<std::string> & tokens, std::ofstream & out_file)
{
	if ( not is_scan(tokens))
		return false;
	if(tokens[4] == "string")
		out_file << "\n\tstd::string " << tokens[0] << ';';
	else if( tokens[4] == "int")
		out_file << "\n\tint "<< tokens[0] << ';';
	else if( tokens[4] == "float")
		out_file << "\n\tfloat " << tokens[0] << ';';
	return true;
}
int main()
{
	std::ofstream out_file;
	out_file.open("cpp_mylang.cpp");
	out_file<<"\n#include<iostream>\n\nint main()\n{";
	
	std::ifstream in_file;
	in_file.open("hello-world.mylang");

	std::string line;
	int lno = 1;
	while(getline(in_file, line))
	{
		trim(line);
		if( not line.empty() )
		{
			bool is_semicolon = true;
			std::vector<std::string> tokens = tokenize(line, is_semicolon);

			if(!is_semicolon) {std::cout<<"hello-world.mylang : error "<<lno<<" expected ;\n"; break; } 
			if(not (print_function(tokens, out_file) or scan_function(tokens, out_file)) )
			{
				for(auto s : tokens)
					std::cout<<s;
				std::cout<<"hello-world.mylang :  error line : "<<lno<<"\n" ; 
				break;
			}
		}
		++lno;
	}
	out_file << "\n}";
}
