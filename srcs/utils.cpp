#include "main.hpp"

std::vector<std::string> split_vct(std::string str, char delim)
{
	std::vector<std::string> tmp_vct;
	std::stringstream stream(str);
	std::string temp_str;
	
	while(std::getline(stream, temp_str, delim))
	{
		temp_vct.push_back(temp_str);
	}
	return (temp_vct);
}