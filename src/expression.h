#pragma once

#include <memory>
#include <string>

#include "diceroll.h"
#include "expressiontype.h"
#include "value.h"

namespace DungeonZ
{
	class Expression
	{
	public:
		ExpressionType Type;
		Value Content;

		Expression();
		Expression(double number);
		Expression(std::string str);
		Expression(DiceRoll diceRoll);
		Expression(Value value);
	};
}

