#pragma once

namespace DungeonZ
{
	enum class Error
	{
		None = 0,
		MultDivIncorrect = 1,
		OperatorPairing = 2,
		NoRValue = 3,
		IncorrectVariable = 4,
		InvalidCharacter = 5,
		MissingVariable = 6,
		Nesting = 7,
		Expression = 8,
		Operation = 9,
		Conditional = 10
	};
}
