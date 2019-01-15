#pragma once

namespace DungeonZ
{
	// Represents the list of tokens supported by the Dice Parser
	// These tokens are not part of an interface or stored persistently
	// so inserting a new token in the middle is okay
	enum class Token
	{
		Error = -1,    // Error parsing a token
		None = 0,      // No token found
		Alpha,         // a-z, A-Z
		Number,        // 0-9
		Decimal,       // .
		Plus,          // + - addition
		Minus,         // - - subtraction
		Multiply,      // * - multiplication
		Divide,        // / - division
		DistributedMultiply, // X - distributed multiplication 3 X (2d6 + 2)
		Percent,       // % - can replace 100 in d100 (e.g. d%)
		BeginGroup,    // (
		EndGroup,      // )
		EqualTo,       // = - Conditional Equal
		GreaterThan,   // > - Conditional Greater than
		LessThan,      // < - Conditional Less than
		Not,           // ! - Conditional Not
	};
}
