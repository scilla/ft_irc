#ifndef UTILS_HPP
#define UTILS_HPP



#include <vector>
#include <sstream>


void replace_substr(std::string& haystack, std::string needle)
{
	size_t index = 0;
	while (true) {
		/* Locate the substring to replace. */
		index = haystack.find(needle.c_str(), index);
		if (index == std::string::npos) break;

		/* Make the replacement. */
		haystack.replace(index, needle.size(), "\n");

		/* Advance index forward so the next iteration doesn't pick it up as well. */
		index += 1;
	}
}

#endif /* UTILS_HPP */
