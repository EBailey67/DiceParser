#include <cctype>
#include <regex>

#include "utility.h"

namespace DungeonZ
{
	namespace utility
	{
		// Case-insensitive comparitor
		bool icompare_pred(unsigned char a, unsigned char b)
		{
			return std::tolower(a) == std::tolower(b);
		}

		// Perform a case-insensitive std::string comparison
		bool icompare(std::string const& a, std::string const& b)
		{
			if (a.length() == b.length())
			{
				return std::equal(b.begin(), b.end(), a.begin(), icompare_pred);
			}

			return false;
		}

		// Removes whitespace from the string and returns the new string.
		// <param name="input">input character stream to be evaluated</param>
		std::string RemoveWhitespace(std::string input)
		{
			auto output = std::regex_replace(input, std::regex("\\s+"), "");
			return output;
		}
	}
}