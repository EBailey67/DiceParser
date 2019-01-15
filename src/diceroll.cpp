
#include <algorithm>
#include <chrono>
#include <random>

#include "diceroll.h"

namespace DungeonZ
{
		// Default constructor
		DiceRoll::DiceRoll()
		{
			Count = 0;
			Sides = 0;
			IsZeroBased = false;
			Rolls.clear();
		}

		// Because external functions can modify the list of Rolls,
		// always calculate this value.
		int DiceRoll::GetTotal()
		{
			int total = 0;
			for(auto i : Rolls)
				total += i;
			
			return total;
		}

		// Return a std::vector<int> of the dice rolls in a sorted-order
		// Original rolls remain unsorted.
		std::vector<int> DiceRoll::GetSortedRolls()
		{
			std::vector<int> sortedDice(Rolls);
			std::sort(sortedDice.begin(), sortedDice.end());
			return sortedDice;
		}

		// Constructor for a dice roll
		//   count - the number of dice to be rolled
		//   sides - if negative 1 (-1), will be treated as Fudge dice
		DiceRoll::DiceRoll(int count, int sides, bool zeroBased)
		{
			Count = count;
			Sides = sides;
			IsZeroBased = zeroBased;
			Rolls.resize(Count);
		}

		// Rolls the dice based on the count and sides
		//   returns - sum total of the dice rolled
		int DiceRoll::Roll()
		{
			Rolls.clear();
			int roll;

			if (Sides == -1)        // Fudge Dice
			{
				for (int i = 0; i < Count; i++)
				{
					roll = RollDice(3) - 2;
					Rolls.push_back(roll);
				}
			}
			else
			{
				for (int i = 0; i < Count; i++)
				{
					if (IsZeroBased)
						roll = RollDice(Sides) - 1;
					else
						roll = RollDice(Sides);
					Rolls.push_back(roll);
				}
			}

			return GetTotal();
		}

		// Produces a random number between 1..sides
		//   sides - The highest number to produce from the die
		//   returns - 1..sides
		int DiceRoll::RollDice(int sides)
		{
			auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
			static std::mt19937 mtengine(static_cast<unsigned int>(seed));
			std::uniform_int_distribution<int> uniform(1, sides);
			return uniform(mtengine);
		}

		// Rerolls the die at a specific index.
		//   index - index of the die to reroll
		int DiceRoll::Reroll(int index)
		{
			if (index < 0 || static_cast<unsigned int>(index) > Rolls.size())
				return 0;

			Rolls[index] = RollDice(Sides);
			return Rolls[index];
		}
}

