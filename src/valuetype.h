#pragma once

namespace DungeonZ
{
	enum class ValueType
	{
		Unknown = 0,  // Unknown or unspecified
		Number,       // Double
		DiceRoll,     // Set of numbers (dice rolls)
		String,       // String / identifier
	};
}
