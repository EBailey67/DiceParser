#pragma once

namespace DungeonZ
{
	enum class Operand
	{
		Error = -1,          // Error parsing a token
		None = 0,            // No token found
		Dice,                // D - ones based dice
		Zero,                // Z - zero based dice
		Fudge,               // F - fudge dice (-1, 0, 1)
		Keep,                // K Keep the highest # of dice
		Plus,                // + - addition
		Minus,               // - - subtraction
		Multiply,            // * - multiplication
		Divide,              // / - division
		DistributedMultiply, // X - distributed multiplication
		Lowest,              // L - Keep lowest 'x' dice
		Highest,             // H - Keep highest 'x' dice
		Reroll,              // R - Reroll
		Count,               // C - Count
		Explode,             // E - Explode
	};
}
