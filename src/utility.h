#pragma once
#include <string>

namespace DungeonZ
{
	namespace utility
	{
		// Case-insensitive std::string comparison
		extern bool icompare(std::string const& a, std::string const& b);

		// Removes whitespace from the string and returns the new string.
		extern std::string RemoveWhitespace(std::string input);
	}
}