#pragma once

#include <vector>

namespace DungeonZ
{

	class DiceRoll
	{
	public:
		int Count;
		int Sides;
		bool IsZeroBased;
		std::vector<int> Rolls;

		DiceRoll();

		// Because external functions can modify the list of Rolls,
		// always calculate this value.
		int GetTotal();

		std::vector<int> GetSortedRolls();

		// Constructor for a dice roll
		//   count - the number of dice to be rolled
		//   sides - if negative 1 (-1), will be treated as Fudge dice
		DiceRoll(int count, int sides, bool zeroBased = false);
		
		
		int Roll();
		

		// Produces a random number between 1..sides
		//   sides - The highest number to produce from the die
		//   returns - 1..sides
		int RollDice(int sides);
		
		// Rerolls the die at a specific index.
		int Reroll(int index);
	};
}
