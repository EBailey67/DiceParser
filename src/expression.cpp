#include "expression.h"
#include "expressiontype.h"
#include "valuetype.h"

namespace DungeonZ
{
	Expression::Expression()
	{
		Type = ExpressionType::None;
		Content = Value();
	}

	Expression::Expression(double number)
	{
		Type = ExpressionType::Number;
		Content = number;
	}

	Expression::Expression(std::string str)
	{
		Type = ExpressionType::Alpha;
		Content = Value(str);
	}

	Expression::Expression(DiceRoll diceRoll)
	{
		Type = ExpressionType::DiceRoll;
		Content = Value(diceRoll);
	}

	Expression::Expression(Value value)
	{
		switch (value.Type)
		{
		case ValueType::Unknown:
			Type = ExpressionType::None;
			break;
		case ValueType::Number:
			Type = ExpressionType::Number;
			break;
		case ValueType::String:
			Type = ExpressionType::Alpha;
			break;
		case ValueType::DiceRoll:
			Type = ExpressionType::DiceRoll;
			break;
		}
		Content = value;
	}
}
