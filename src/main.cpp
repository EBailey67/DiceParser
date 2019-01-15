#include <iostream>

#include "diceparser.h"
#include "version.h"

namespace DungeonZ
{
	std::vector<std::string> Rolls = 
	{
		"d6",           // Roll a single six-sided die
		"1d6",          // Roll 1 six-sided die
		"4d6",          // Roll 4 six-sided dice
		"4z6",          // Roll 4 zero-based six-sided (0-5) dice
		"5dF",          // Roll 5 Fudge dice [-], [ ], [+]
		"4d6k3",        // Roll 4 six-sided dice and keep the highest 3
		"4d6L3",        // Roll 4 six-sided dice and keep the lowest 3
		"4d6H3",        // Roll 4 six-sided dice and keep the highest 3
		"2*3+10/5",     // Basic math - order of precedence followed
		"2*(3+10)/5",   // Basic math with parenthesis to change order of precedence
		"2d%",          // Roll 2 one-hundred-sided dice (percentile dice)
		"5d20c>10",     // Roll 5 twenty-sided dice and count the number of dice whose total is greater than 10
		"5d20c<=15",    // Roll 5 twenty-sided dice and count the number of dice whose total is less-than-or-equal to 15
		"5d6r=1",       // Roll 5 six-sided dice and reroll any dice which rolled a 1 (only once per die)
		"5d6r!=5",      // Roll 5 six-sided dice and reroll any dice not equal to a 5 (only once per die)
		"10d6E=6",      // Roll 10 six-sided dice and explode the roll (re-roll and add to it) if it was equal to a 6
		"6*(4d6K3+5)",  // Math check.  Note: the inside roll is only rolled once in this case and then multiplied by 6
		"2d4d6",        // Roll 2 four-sided dice, then roll the total from the previous roll six-sided dice
		"6x(3d6+2)"
	};
}

int main(int argc, char *argv[])
{
	std::cout << "Dice Parser Version " << VER_PRODUCT_VERSION_STR << std::endl;

	DungeonZ::DiceParser dp;
	std::string result;

	// Console.Clear();

	if (argc == 1)
	{
		for (auto roll : DungeonZ::Rolls)
		{
			result = dp.Parse(roll);
			std::cout << "==================================================" << std::endl;
			std::cout << "Parsing : \"" << roll << "\"" << std::endl << dp.Output.str() << std::endl;
			if (dp.FinalResult != -1)
				std::cout << "Final Result : " << dp.FinalResult << std::endl;
			else
			{
				std::cout << "ERROR: " << dp.ErrorStrings[dp.ErrorCode] << std::endl;
			}
			dp.Output.str(std::string());
		}
	}
	else
	{
		std::string input(argv[1]);

		result = dp.Parse(input);
		std::cout << "==================================================" << std::endl;
		std::cout << "Parsing : \"" << input << "\"" << std::endl << dp.Output.str() << std::endl;
		if (dp.FinalResult != -1)
			std::cout << "Final Result : " << dp.FinalResult << std::endl;
		else
		{
			std::cout << "ERROR: " << dp.ErrorStrings[dp.ErrorCode] << std::endl;
		}
		dp.Output.str(std::string());

	}
}