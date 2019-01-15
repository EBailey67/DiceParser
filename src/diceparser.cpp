
#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <stack>

#include "diceparser.h"
#include "expression.h"
#include "utility.h"

namespace DungeonZ
{

// Returns the next token in the input stream starting from index
// <param name="input">the input character stream</param>
// <param name="index">starting index (updated with new location</param>
// <returns>The token found or Token.None if nothing was found</returns>
Token DiceParser::NextToken(std::string input, size_t index)
{
	if (static_cast<unsigned int>(index) > input.size())
		return Token::Error;

	if (static_cast<unsigned int>(index) == input.size())
		return Token::None;

	char c = input[index];

	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return Token::Alpha;

	if (c >= '0' && c <= '9')
		return Token::Number;

	switch (c)
	{
	case '%': return Token::Percent;
	case '!': return Token::Not;
	case '=': return Token::EqualTo;
	case '<': return Token::LessThan;
	case '>': return Token::GreaterThan;
	case '-': return Token::Minus;
	case '+': return Token::Plus;
	case '*': return Token::Multiply;
	case '/': return Token::Divide;
	case '(': return Token::BeginGroup;
	case ')': return Token::EndGroup;
	case '.': return Token::Decimal;
	}

	return Token::Error;
}

int DiceParser::RandomRange(int low, int high)
{
	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	static std::mt19937 mtengine(static_cast<unsigned int>(seed));
	std::uniform_int_distribution<int> uniform(low, high);
	return uniform(mtengine);
}

int DiceParser::RollDice(int sides)
{
	return RandomRange(1, sides);
}

int DiceParser::RollZeroDice(int sides)
{
	return RandomRange(0, sides - 1);
}


Value DiceParser::RollMultipleDice(int count, int sides, bool zeroBased)
{
	DiceRoll diceRoll(count, sides, zeroBased);
	diceRoll.Roll();

	return diceRoll;
}

Value DiceParser::RollFudge(int count)
{
	return RollMultipleDice(count, -1);
}

Value DiceParser::DoOperation(Expression leftExp, Expression rightExp, Operand operand, Conditional conditional)
{
	Value answer;
	Value left = leftExp.Content;
	Value right = rightExp.Content;

	switch (operand)
	{
		case Operand::None:
			break;
		case Operand::Error:
			break;
		case Operand::Dice:
		{
			if (left.Type == ValueType::Unknown)
				left = 1;

			if (left.ToInt() == 0)
				return 0;

			if (right.ToInt() > 0)
			{
				answer = RollMultipleDice(left.ToInt(), right.ToInt(), false);
				Output << left << "d" << right << ":" << answer << " = " << answer.GetSetSum();
			}
			else
			{
				answer = RollFudge(left.ToInt());
				Output << left << " Fudge:" << answer;
			}

			Output << std::endl;
		
			return answer;
		}

		case Operand::Zero:
		{
			if (left.Type == ValueType::Unknown)
				left = 1;

			if (left.ToInt() == 0)
				return 0;

			if (right.ToInt() > 0)
			{
				answer = RollMultipleDice(left.ToInt(), right.ToInt(), true);
				Output << left << "z" << right << ":" << answer << " = " << answer.GetSetSum();
			}
			else
			{
				answer = RollFudge(left.ToInt());
				Output << left << " Fudge:" << answer;
			}

			Output << "\n";
			return answer;
		}

		case Operand::Fudge:
		{
			if (left.Type == ValueType::Unknown)
				left = 1;

			if (left.ToInt() == 0)
				return 0;

			answer = RollFudge(left.ToInt());
			Output << left << " Fudge : ";

			for(auto i : answer.Dice.Rolls)
			{
				switch (i)
				{
					case -1: Output << "[-]"; break;
					case 0: Output << "[ ]"; break;
					case 1: Output << "[+]"; break;
					default: 
						Output.width(3);
						Output << right << i;
						break;
				}
			}
			Output << " = " << answer << "\n";
			return answer;
		}

		case Operand::Lowest:
		case Operand::Highest:
		case Operand::Keep:
		{
			if (left.Type != ValueType::DiceRoll)
			{
				ErrorCode = Error::Operation;  // Bad operation
				return right;
			}

			DiceRoll result(left.Dice.Count, left.Dice.Sides);

			int num = right.ToInt();
			if (num == 0)
				num = 1;

			if (left.Dice.Count > num)
			{
				std::vector<int> sorted = left.GetSortedSet();
				result.Rolls.clear();
				if (operand == Operand::Lowest)
				{
					for (int i = 0; i < num; ++i)
						result.Rolls.push_back(sorted[i]);
				}
				else
				{
					for (auto i = sorted.size() - num; i < sorted.size(); ++i)
						result.Rolls.push_back(sorted[i]);
				}
			}
			answer = result;

			if (operand == Operand::Keep)
				Output << "Keeping ";
			else if (operand == Operand::Highest)
				Output << "Highest ";
			else
				Output << "Lowest ";

			Output << right << " dice: " << answer << " = " << answer.ToInt() << "\n";
			return answer;
		}
		case Operand::Plus:
			answer = left.ToDouble() + right.ToDouble();
			Output << "Add:" << left << "+" << right << "=" << answer << "\n";
			return answer;
		case Operand::Minus:
			answer = left.ToDouble() - right.ToDouble();
			Output << "Sub:" << left << "-" << right << "=" << answer << "\n";
			return answer;
		case Operand::Multiply:
			answer = left.ToDouble() * right.ToDouble();
			Output << "Mul:" << left << "*" << right << "=" << answer << "\n";
			return answer;
		case Operand::Divide:
			if (right.ToDouble() == 0)
			{
				Output << "Div:" << left << "/0=undefined!" << "\n";
			}
			else
			{
				answer = left.ToDouble() / right.ToDouble();
				Output << "Div:" << left << "/" << right << "=" << answer << "\n";
				return answer;
			}
			break;
		case Operand::DistributedMultiply:
			answer = left.ToDouble() * right.ToDouble();
			Output << "DistMul:" << left << "x" << right << "=" << answer << "\n";
			return answer;
		case Operand::Explode:
		{
			if (conditional == Conditional::None || left.Type != ValueType::DiceRoll)
			{
				ErrorCode = Error::Operation;  // Bad operation
				return right;
			}

			DiceRoll result = DiceRoll(left.Dice.Count, left.Dice.Sides);

			for (size_t i = 0; i < left.Dice.Rolls.size(); ++i)
			{
				result.Rolls[i] = left.Dice.Rolls[i];
				switch (conditional)
				{
				case Conditional::None:
					break;
				case Conditional::EqualTo:
					if (left.Dice.Rolls[i] == right.ToDouble())
						result.Rolls[i] = left.Dice.Rolls[i] + RollDice(left.Dice.Sides);
					break;
				case Conditional::GreaterThan:
					if (left.Dice.Rolls[i] > right.ToDouble())
						result.Rolls[i] = left.Dice.Rolls[i] + RollDice(left.Dice.Sides);
					break;
				case Conditional::LessThan:
					if (left.Dice.Rolls[i] < right.ToDouble())
						result.Rolls[i] = left.Dice.Rolls[i] + RollDice(left.Dice.Sides);
					break;
				case Conditional::NotEqual:
					if (left.Dice.Rolls[i] != right.ToDouble())
						result.Rolls[i] = left.Dice.Rolls[i] + RollDice(left.Dice.Sides);
					break;
				case Conditional::AtLeast:
					if (left.Dice.Rolls[i] >= right.ToDouble())
						result.Rolls[i] = left.Dice.Rolls[i] + RollDice(left.Dice.Sides);
					break;
				case Conditional::AtMost:
					if (left.Dice.Rolls[i] <= right.ToDouble())
						result.Rolls[i] = left.Dice.Rolls[i] + RollDice(left.Dice.Sides);
					break;
				}
			}
			Value outVal(result);
			Output << "Exploding rolls that are " << GetConditionalFriendlyName(conditional) << " " << right << " is " << outVal << "\n";
			return outVal;
		}
		case Operand::Reroll:
		{
			if (conditional == Conditional::None || left.Type != ValueType::DiceRoll)
			{
				ErrorCode = Error::Operation;  // Bad operation
				return right;
			}

			DiceRoll result(left.Dice.Count, left.Dice.Sides);

			for (size_t i = 0; i < left.Dice.Rolls.size(); i++)
			{
				result.Rolls.push_back(left.Dice.Rolls[i]);
				switch (conditional)
				{
				case Conditional::None:
					break;
				case Conditional::EqualTo:
					if (left.Dice.Rolls[i] == right.ToDouble())
						result.Rolls[i] = RollDice(left.Dice.Sides);
					break;
				case Conditional::GreaterThan:
					if (left.Dice.Rolls[i] > right.ToDouble())
						result.Rolls[i] = RollDice(left.Dice.Sides);
					break;
				case Conditional::LessThan:
					if (left.Dice.Rolls[i] < right.ToDouble())
						result.Rolls[i] = RollDice(left.Dice.Sides);
					break;
				case Conditional::NotEqual:
					if (left.Dice.Rolls[i] != right.ToDouble())
						result.Rolls[i] = RollDice(left.Dice.Sides);
					break;
				case Conditional::AtLeast:
					if (left.Dice.Rolls[i] >= right.ToDouble())
						result.Rolls[i] = RollDice(left.Dice.Sides);
					break;
				case Conditional::AtMost:
					if (left.Dice.Rolls[i] <= right.ToDouble())
						result.Rolls[i] = RollDice(left.Dice.Sides);
					break;
				}
			}
			Value outVal(result);
			Output << "Rerolling that is " << GetConditionalFriendlyName(conditional) << " " << right << " is " << outVal << "\n";
			return outVal;
		}
		case Operand::Count:
		{
			if (conditional == Conditional::None)
			{
				ErrorCode = Error::Operation;  // Bad operation
				return right;
			}

			if (left.Type == ValueType::DiceRoll)
			{
				DiceRoll result(left.Dice.Count, left.Dice.Sides);
				for (size_t i = 0; i < left.Dice.Rolls.size(); i++)
				{
					result.Rolls.push_back(0);
					switch (conditional)
					{
					case Conditional::None:
						break;
					case Conditional::EqualTo:
						if (left.Dice.Rolls[i] == right.ToDouble())
							result.Rolls[i] = 1;
						break;
					case Conditional::GreaterThan:
						if (left.Dice.Rolls[i] > right.ToDouble())
							result.Rolls[i] = 1;
						break;
					case Conditional::LessThan:
						if (left.Dice.Rolls[i] < right.ToDouble())
							result.Rolls[i] = 1;
						break;
					case Conditional::NotEqual:
						if (left.Dice.Rolls[i] != right.ToDouble())
							result.Rolls[i] = 1;
						break;
					case Conditional::AtLeast:
						if (left.Dice.Rolls[i] >= right.ToDouble())
							result.Rolls[i] = 1;
						break;
					case Conditional::AtMost:
						if (left.Dice.Rolls[i] <= right.ToDouble())
							result.Rolls[i] = 1;
						break;
					}
				}
				Value outVal(result);
				Output << "Count that is " << GetConditionalFriendlyName(conditional) << " " << right << " is " << outVal.ToInt() << "\n";
				return outVal;
			}
			else
			{
				switch (conditional)
				{
				case Conditional::None:
					break;
				case Conditional::EqualTo:
					if (left.ToDouble() == right.ToDouble())
						return 1;
					else
						return 0;
				case Conditional::GreaterThan:
					if (left.ToDouble() > right.ToDouble())
						return 1;
					else
						return 0;
				case Conditional::LessThan:
					if (left.ToDouble() < right.ToDouble())
						return 1;
					else
						return 0;
				case Conditional::NotEqual:
					if (left.ToDouble() != right.ToDouble())
						return 1;
					else
						return 0;
				case Conditional::AtLeast:
					if (left.ToDouble() >= right.ToDouble())
						return 1;
					else
						return 0;
				case Conditional::AtMost:
					if (left.ToDouble() <= right.ToDouble())
						return 1;
					else
						return 0;
				}
			}
			break;
		}
	}

	// Error if we got here.  Bad Math
	ErrorCode = Error::Operation;  // Bad operation
	return right;
}

std::string DiceParser::GetConditionalFriendlyName(Conditional conditional)
{
	switch (conditional)
	{
		case Conditional::EqualTo:
			return "equal to";
		case Conditional::GreaterThan:
			return "greater than";
		case Conditional::LessThan:
			return "less than";
		case Conditional::NotEqual:
			return "not equal to";
		case Conditional::AtLeast:
			return "at least";
		case Conditional::AtMost:
			return "at most";
		case Conditional::None:
			return "none";
	}

	return "bad conditional";
}

std::vector<Expression> DiceParser::SolveConditional(std::vector<Expression> expressions)
{
	if (expressions.size() == 0)
	{
		ErrorCode = Error::Expression;
		return std::vector<Expression>();
	}

	return expressions;
}

// Solves distributed multipliers (loops)
std::vector<Expression> DiceParser::SolveDistributedMultiply(std::vector<Expression> expressions)
{
	if (expressions.size() == 0)
	{
		ErrorCode = Error::Expression;
		return std::vector<Expression>();
	}

	Operand operand = Operand::None;
	std::vector<Expression> outExpressions;
	Expression rightExpression;
	Expression leftExpression;

	size_t index = 0;

	while (index < expressions.size())
	{
		switch (expressions[index].Type)
		{
		case ExpressionType::DistributedMultiply:
			if (operand != Operand::None)
				rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, Conditional::None));

			operand = Operand::DistributedMultiply;

			leftExpression = rightExpression;
			rightExpression = Expression();
			break;
		default:
			outExpressions.push_back(expressions[index]);
			break;
		}
		index++;
	}

	if (operand != Operand::None)
		rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, Conditional::None));

	if (rightExpression.Type != ExpressionType::None)
		outExpressions.push_back(rightExpression);

	return outExpressions;
}



std::vector<Expression> DiceParser::SolveDice(std::vector<Expression> expressions)
{
	std::vector<Expression> emptyResult;

	if (expressions.size() == 0)
	{
		ErrorCode = Error::Expression;
		return emptyResult;
	}

	Operand operand = Operand::None;
	Conditional conditional = Conditional::None;
	std::vector<Expression> outExpressions;
	Expression rightExpression;
	Expression leftExpression;

	size_t index = 0;

	while (index < expressions.size())
	{
		switch (expressions[index].Type)
		{
		case ExpressionType::DiceRoll:
		case ExpressionType::Number:
			rightExpression = expressions[index];
			break;
		case ExpressionType::Percent:
			if (operand != Operand::None && rightExpression.Type == ExpressionType::None)
				rightExpression = Expression(100);
			break;
		case ExpressionType::Fudge:
			if (operand != Operand::None && rightExpression.Type == ExpressionType::None)
				rightExpression = Expression(0);

			if (operand != Operand::None)
				rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, conditional));

			operand = Operand::Fudge;
			leftExpression = rightExpression;
			rightExpression = Expression();

			break;
		case ExpressionType::Keep:
			if (operand != Operand::None)
				rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, conditional));

			operand = Operand::Keep;
			leftExpression = rightExpression;
			rightExpression = Expression();
			break;
		case ExpressionType::Highest:
			if (operand != Operand::None)
				rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, conditional));

			operand = Operand::Highest;
			leftExpression = rightExpression;
			rightExpression = Expression();
			break;
		case ExpressionType::Lowest:
			if (operand != Operand::None)
				rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, conditional));

			operand = Operand::Lowest;
			leftExpression = rightExpression;
			rightExpression = Expression();
			break;
		case ExpressionType::Dice:
			if (operand != Operand::None)
				rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, conditional));

			operand = Operand::Dice;
			leftExpression = rightExpression;
			rightExpression = Expression();
			break;
		case ExpressionType::Zero:
			if (operand != Operand::None)
				rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, conditional));

			operand = Operand::Zero;
			leftExpression = rightExpression;
			rightExpression = Expression();
			break;
		case ExpressionType::GreaterThan:
			if (operand != Operand::Count && operand != Operand::Reroll && operand != Operand::Explode)
			{
				ErrorCode = Error::Expression;
				return emptyResult;
			}
			conditional = Conditional::GreaterThan;
			rightExpression = Expression();
			break;
		case ExpressionType::LessThan:
			if (operand != Operand::Count && operand != Operand::Reroll && operand != Operand::Explode)
			{
				ErrorCode = Error::Expression;
				return emptyResult;
			}
			conditional = Conditional::LessThan;
			rightExpression = Expression();
			break;
		case ExpressionType::EqualTo:
			if (operand != Operand::Count && operand != Operand::Reroll && operand != Operand::Explode)
			{
				ErrorCode = Error::Expression;
				return emptyResult;
			}
			conditional = Conditional::EqualTo;
			rightExpression = Expression();
			break;
		case ExpressionType::NotEqual:
			if (operand != Operand::Count && operand != Operand::Reroll && operand != Operand::Explode)
			{
				ErrorCode = Error::Expression;
				return emptyResult;
			}
			conditional = Conditional::NotEqual;
			rightExpression = Expression();
			break;
		case ExpressionType::AtLeast:
			if (operand != Operand::Count && operand != Operand::Reroll && operand != Operand::Explode)
			{
				ErrorCode = Error::Expression;
				return emptyResult;
			}
			conditional = Conditional::AtLeast;
			rightExpression = Expression();
			break;
		case ExpressionType::AtMost:
			if (operand != Operand::Count && operand != Operand::Reroll && operand != Operand::Explode)
			{
				ErrorCode = Error::Expression;
				return emptyResult;
			}
			conditional = Conditional::AtMost;
			rightExpression = Expression();
			break;
		case ExpressionType::Count:
			if (operand != Operand::None)
				rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, conditional));

			operand = Operand::Count;
			leftExpression = rightExpression;
			rightExpression = Expression();
			break;
		case ExpressionType::Reroll:
			if (operand != Operand::None)
				rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, conditional));

			operand = Operand::Reroll;
			leftExpression = rightExpression;
			rightExpression = Expression();
			break;
		case ExpressionType::Explode:
			if (operand != Operand::None)
				rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, conditional));

			operand = Operand::Explode;
			leftExpression = rightExpression;
			rightExpression = Expression();
			break;
		case ExpressionType::Plus:
		case ExpressionType::Minus:
			if (index == 0)
				rightExpression = expressions[index];
			else if (operand == Operand::None && rightExpression.Type == ExpressionType::None)
				rightExpression = expressions[index];
			[[fallthrough]];
			case ExpressionType::Multiply:
			case ExpressionType::Divide:
				if (operand != Operand::None)
					rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, conditional));

				outExpressions.push_back(rightExpression);
				outExpressions.push_back(expressions[index]);

				rightExpression = Expression();
				leftExpression = Expression();
				operand = Operand::None;
				break;
				//case ExpressionType::DistributedMultiply:
				//    if (operand != Operand::None)
				//        rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, conditional));

				//    outExpressions.Add(rightExpression);
				//    outExpressions.Add(expressions[index]);

				//    rightExpression = Expression();
				//    leftExpression = Expression();
				//    operand = Operand::None;
				//    break;

			default:
				if (operand != Operand::None)
				{
					rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, conditional));
					leftExpression = Expression();
					operand = Operand::None;
				}
				else
				{
					ErrorCode = Error::Expression;
					return emptyResult;
				}
				break;
		}
		index++;
	}

	if (operand != Operand::None)
		rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, conditional));

	if (rightExpression.Type != ExpressionType::None)
		outExpressions.push_back(rightExpression);

	return outExpressions;
}

// Solves all multiplication and division statements
std::vector<Expression> DiceParser::SolveMult(std::vector<Expression> expressions)
{
	std::vector<Expression> outExpressions;
	if (expressions.size() == 0)
	{
		ErrorCode = Error::Expression;
		return outExpressions;
	}

	Operand operand = Operand::None;
	Expression rightExpression = Expression();
	Expression leftExpression = Expression();

	size_t index = 0;

	while (index < expressions.size())
	{
		switch (expressions[index].Type)
		{
		case ExpressionType::DiceRoll:
		case ExpressionType::Number:
			rightExpression = expressions[index];
			break;
		case ExpressionType::Plus:
		case ExpressionType::Minus:
			if (index == 0)
				rightExpression = expressions[index];
			else if (operand == Operand::None && rightExpression.Type == ExpressionType::None)
				rightExpression = expressions[index];

			if (operand != Operand::None)
				rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, Conditional::None));

			outExpressions.push_back(rightExpression);
			outExpressions.push_back(expressions[index]);
			rightExpression = Expression();
			leftExpression = Expression();
			operand = Operand::None;
			break;

		case ExpressionType::Multiply:
		case ExpressionType::Divide:
			if (operand != Operand::None)
				rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, Conditional::None));

			if (expressions[index].Type == ExpressionType::Divide)
				operand = Operand::Divide;
			else
				operand = Operand::Multiply;

			leftExpression = rightExpression;
			rightExpression = Expression();
			break;
		default:
			break;
		}
		index++;
	}

	if (operand != Operand::None)
		rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, Conditional::None));

	if (rightExpression.Type != ExpressionType::None)
		outExpressions.push_back(rightExpression);

	return outExpressions;
}

// Solves all addition and subtraction statements
std::vector<Expression> DiceParser::SolveAdd(std::vector<Expression> expressions)
{
	std::vector<Expression> outExpressions;

	if (expressions.size() == 0)
	{
		ErrorCode = Error::Expression;
		return outExpressions;
	}

	Operand operand = Operand::None;
	Expression rightExpression = Expression();
	Expression leftExpression = Expression();

	size_t index = 0;

	while (index < expressions.size())
	{
		switch (expressions[index].Type)
		{
		case ExpressionType::DiceRoll:
		case ExpressionType::Number:
			rightExpression = expressions[index];
			break;
		case ExpressionType::Plus:
		case ExpressionType::Minus:
			if (operand != Operand::None)
				rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, Conditional::None));

			if (expressions[index].Type == ExpressionType::Plus)
				operand = Operand::Plus;
			else
				operand = Operand::Minus;

			leftExpression = rightExpression;
			rightExpression = Expression();
			break;
		default:
			break;
		}
		index++;
	}

	if (operand != Operand::None)
		rightExpression = Expression(DoOperation(leftExpression, rightExpression, operand, Conditional::None));

	if (rightExpression.Type != ExpressionType::None)
		outExpressions.push_back(rightExpression);

	return outExpressions;
}

// Solves the expressions list in the proper order of operations
std::vector<Expression> DiceParser::SolvePriority(std::vector<Expression> expressions)
{

	std::vector<Expression> newExpressions(expressions);
	newExpressions = SolveDistributedMultiply(newExpressions);
	newExpressions = SolveDice(newExpressions);
	newExpressions = SolveMult(newExpressions);
	newExpressions = SolveAdd(newExpressions);
	newExpressions = SolveConditional(newExpressions);

	return newExpressions;
}

// Parses dice algebraic notation
//   diceNotation - std::string with the notation
//   returns - empty string or sum value
std::string DiceParser::Parse(std::string diceNotation)
{
	std::vector<Expression> expressions;
	std::stack<std::vector<Expression>> stack;

	Output.clear();
	FinalResult = -1;

	// First, remove all the whitespace.  We shouldn't need it.
	std::string input = utility::RemoveWhitespace(diceNotation);
	std::string content = "";

	Expression currentExpression = Expression();

	for (size_t index = 0; index < input.size(); ++index)
	{
		Token token = NextToken(input, index);
		Token nextToken = NextToken(input, index + 1);

		if (token == Token::Error)
		{
			ErrorCode = Error::InvalidCharacter;
			return "";
		}

		char currentChar = input[index];

		// First, see if we need to start a Expression or not
		bool needNewExpression = true;

		switch (currentExpression.Type)
		{
		case ExpressionType::None:
			needNewExpression = false;
			break;
		case ExpressionType::Alpha:
			if (token == Token::Alpha)
				needNewExpression = false;
			break;
		case ExpressionType::Number:
			if (token == Token::Number || token == Token::Decimal)
				needNewExpression = false;
			break;
		default:
			needNewExpression = true;
			break;
		}

		if (needNewExpression)
		{
			if (currentExpression.Type == ExpressionType::Number)
			{
				currentExpression.Content = std::stod(content);
			}
			else if (currentExpression.Type == ExpressionType::Alpha)
			{
				// There are a few keyword types that need to be identified
				if (utility::icompare(content, "DF"))
				{
					currentExpression.Type = ExpressionType::Fudge;
					currentExpression.Content = Value("DF");
				}
				else if (utility::icompare(content, "X"))
				{
					currentExpression.Type = ExpressionType::DistributedMultiply;
					currentExpression.Content = Value("X");
				}
				else if (utility::icompare(content, "D"))
				{
					currentExpression.Type = ExpressionType::Dice;
					currentExpression.Content = Value("D");
				}
				else if (utility::icompare(content, "Z"))
				{
					currentExpression.Type = ExpressionType::Zero;
					currentExpression.Content = Value("Z");
				}
				else if (utility::icompare(content, "C"))
				{
					currentExpression.Type = ExpressionType::Count;
					currentExpression.Content = Value("C");
				}
				else if (utility::icompare(content, "R"))
				{
					currentExpression.Type = ExpressionType::Reroll;
					currentExpression.Content = Value("R");
				}
				else if (utility::icompare(content, "E"))
				{
					currentExpression.Type = ExpressionType::Explode;
					currentExpression.Content = Value("E");
				}
				else if (utility::icompare(content, "K"))
				{
					currentExpression.Type = ExpressionType::Keep;
					currentExpression.Content = Value("K");
				}
				else if (utility::icompare(content, "H"))
				{
					currentExpression.Type = ExpressionType::Highest;
					currentExpression.Content = Value("H");
				}
				else if (utility::icompare(content, "L"))
				{
					currentExpression.Type = ExpressionType::Lowest;
					currentExpression.Content = Value("L");
				}
			}
			else
			{
				currentExpression.Content = Value(content);
			}

			expressions.push_back(currentExpression);
			currentExpression = Expression();
			content = "";
		}

		switch (token)
		{
		case Token::Alpha:
			currentExpression.Type = ExpressionType::Alpha;
			content += currentChar;
			break;
		case Token::Number:
		case Token::Decimal:
			currentExpression.Type = ExpressionType::Number;
			content += currentChar;
			break;
		case Token::Percent:
			currentExpression.Type = ExpressionType::Percent;
			content += currentChar;
			break;
		case Token::Plus:
			currentExpression.Type = ExpressionType::Plus;
			content += currentChar;
			break;
		case Token::Minus:
			currentExpression.Type = ExpressionType::Minus;
			content += currentChar;
			break;
		case Token::Multiply:
			currentExpression.Type = ExpressionType::Multiply;
			content += currentChar;
			break;
		case Token::Divide:
			currentExpression.Type = ExpressionType::Divide;
			content += currentChar;
			break;
		case Token::DistributedMultiply:
			currentExpression.Type = ExpressionType::DistributedMultiply;
			content += currentChar;
			break;
		case Token::GreaterThan:
			if (nextToken == Token::EqualTo)
			{
				currentExpression.Type = ExpressionType::AtLeast;
				content = ">=";
				index++;
			}
			else
			{
				currentExpression.Type = ExpressionType::GreaterThan;
				content += currentChar;
			}
			break;
		case Token::LessThan:
			if (nextToken == Token::EqualTo)
			{
				currentExpression.Type = ExpressionType::AtMost;
				content = "<=";
				index++;
			}
			else
			{
				currentExpression.Type = ExpressionType::LessThan;
				content += currentChar;
			}
			break;
		case Token::EqualTo:
			currentExpression.Type = ExpressionType::EqualTo;
			content += currentChar;
			break;
		case Token::Not:
			if (nextToken == Token::EqualTo)
			{
				currentExpression.Type = ExpressionType::NotEqual;
				content = "!=";
				index++;
			}
			else
			{
				currentExpression.Type = ExpressionType::Error;
				content += currentChar;
			}
			break;
		case Token::BeginGroup:
			{
				stack.push(expressions);
				expressions = std::vector<Expression>();
			}
			break;
		case Token::EndGroup:
			{
				if (stack.size() == 0)
				{
					ErrorCode = Error::Nesting;
					return "";
				}

				std::vector<Expression> answer = SolvePriority(expressions);
				expressions = stack.top();
				stack.pop();
				for (Expression e : answer)
					expressions.push_back(e);
			}
			break;
		default:
			ErrorCode = Error::Expression;
			return "";
		}
	}

	if (currentExpression.Type == ExpressionType::Number)
	{
		currentExpression.Content = Value(std::stod(content));
	}
	else if (currentExpression.Type == ExpressionType::Alpha)
	{
		// There are a few keyword types that need to be identified
		if (utility::icompare(content, "DF"))
		{
			currentExpression.Type = ExpressionType::Fudge;
			currentExpression.Content = Value("DF");
		}
		else if (utility::icompare(content, "X"))
		{
			currentExpression.Type = ExpressionType::DistributedMultiply;
			currentExpression.Content = Value("X");
		}
		else if (utility::icompare(content, "D"))
		{
			currentExpression.Type = ExpressionType::Dice;
			currentExpression.Content = Value("D");
		}
		else if (utility::icompare(content, "Z"))
		{
			currentExpression.Type = ExpressionType::Zero;
			currentExpression.Content = Value("Z");
		}
		else if (utility::icompare(content, "C"))
		{
			currentExpression.Type = ExpressionType::Count;
			currentExpression.Content = Value("C");
		}
		else if (utility::icompare(content, "R"))
		{
			currentExpression.Type = ExpressionType::Reroll;
			currentExpression.Content = Value("R");
		}
		else if (utility::icompare(content, "E"))
		{
			currentExpression.Type = ExpressionType::Explode;
			currentExpression.Content = Value("E");
		}
		else if (utility::icompare(content, "K"))
		{
			currentExpression.Type = ExpressionType::Keep;
			currentExpression.Content = Value("K");
		}
		else if (utility::icompare(content, "H"))
		{
			currentExpression.Type = ExpressionType::Highest;
			currentExpression.Content = Value("H");
		}
		else if (utility::icompare(content, "L"))
		{
			currentExpression.Type = ExpressionType::Lowest;
			currentExpression.Content = Value("L");
		}
	}
	else
	{
		currentExpression.Content = Value(content);
	}

	if (currentExpression.Type != ExpressionType::None)
		expressions.push_back(currentExpression);

	if (stack.size() == 0)
	{
		// Output.Append(String.Format("Solving priority for : {0}\n", expressions[0].ToString()));
		expressions = SolvePriority(expressions);
		if (expressions.size() == 0)
		{
			return "";
		}
		else if (expressions.size() != 1)
		{
			return expressions[0].Content.ToString();
		}
		else
		{
			FinalResult = expressions[0].Content.ToDouble();
			return expressions[0].Content.ToString();
		}
	}

	ErrorCode = Error::Nesting;
	return "";
}
}
