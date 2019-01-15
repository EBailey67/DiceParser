#pragma once

namespace DungeonZ
{
	enum class ExpressionType
	{
		Error = -1,    // Error parsing a token
		None = 0,      // No token found
		Alpha,         // Alpha (A-Z, a-z)
		Number,        // 0-9 and decimals
		DiceRoll,      // a dice roll result
		Dice,          // D - ones based dice
		Zero,          // Z - zero based dice
		Fudge,         // F - fudge dice (-1, 0, 1)
		Keep,          // K Keep the highest # of dice
		Plus,          // + - addition
		Minus,         // - - subtraction
		Multiply,      // * - multiplication
		Divide,        // / - division
		DistributedMultiply, // X - distributed multiplication 6 X (2d6 + 2)
		Percent,       // % - can replace 100 in d100 (e.g. d%)
		BeginGroup,    // (
		EndGroup,      // )
		Lowest,        // L - Keep lowest 'x' dice
		Highest,       // H - Keep highest 'x' dice
		Count,         // C - Count of dice meeting conditional
		Reroll,        // R - Reroll dice if...
		Explode,       // E - Explode
		EqualTo,       // = - Conditional Equal
		GreaterThan,   // > - Conditional Greater than
		LessThan,      // < - Conditional Less than
		AtLeast,       // >= - Conditional Greater than or equal
		AtMost,        // <= - Conditional Less than or equal
		NotEqual,      // != or <> - Conditional Not Equal to
	};
}
