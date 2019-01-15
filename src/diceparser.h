#pragma once

#include <map>
#include <regex>
#include <string>
#include <sstream>

#include "conditional.h"
#include "error.h"
#include "expression.h"
#include "operand.h"
#include "token.h"
#include "value.h"

namespace DungeonZ
{
	class DiceParser
	{
	public:
		/// Error code results and strings
		Error ErrorCode;
		std::stringstream Output;
		double FinalResult;

		std::map<Error, std::string> ErrorStrings = 
		{
			{ Error::None, "None" },
			{ Error::MultDivIncorrect, "'*' or '\\' is used incorrectly!" },
			{ Error::OperatorPairing, "Operators have been paired incorrectly!" },
			{ Error::NoRValue, "An operator has no rValue!" },
			{ Error::IncorrectVariable, "A variable is being used incorrectly!" },
			{ Error::InvalidCharacter, "Invalid Character!" },
			{ Error::MissingVariable, "The expression contains an invalid variable!" },
			{ Error::Nesting, "Nesting Error" },
			{ Error::Expression, "Invalid Expression" },
			{ Error::Operation, "Invalid Operation" },
			{ Error::Conditional, "Invalid Conditional" },
		};


		// Finds the next token in the input stream starting from index
		// Returns the token found or Token::None if nothing was found
		Token NextToken(std::string input, size_t index);

		// Produces a random number between 1..sides
		//   sides : The highest number to produce from the die</param>
		//   returns : 1..sides
		int RollDice(int sides);

		// Produces a random number between 0..sides-1
		// <param name="sides">The number of faces on the dice</param>
		// <returns>0..sides-1</returns>
		int RollZeroDice(int sides);
		
		// Rolls a number of dice with the specified sides.
		// <param name="number">number of dice to roll</param>
		// <param name="sides">highest number to produce from a single die</param>
		Value RollMultipleDice(int count, int sides, bool zeroBased = false);

		// Rolls a Fudge die (-1, 0, 1)
		// <param name="number">Number of dice to roll</param>
		// <returns>The sum of the die rolls</returns>
		Value RollFudge(int count);
		
		// Parses dice algebraic notation
		// <param name="diceNotation">string with the notation</param>
		// <returns>null or sum value</returns>
		std::string Parse(std::string diceNotation);

	protected:
		// Solves the expressions list in the proper order of operations
		std::vector<Expression> SolvePriority(std::vector<Expression> expressions);
		
		// Solves all addition and subtraction statements
		// <param name="expressions"></param>
		// <returns></returns>
		std::vector<Expression> SolveAdd(std::vector<Expression> expressions);

		// Solves all multiplication and division statements
		// <param name="expressions"></param>
		std::vector<Expression> SolveMult(std::vector<Expression> expressions);

		// Solves Dice statements
		std::vector<Expression> SolveDice(std::vector<Expression> expressions);

		// Solves distributed multipliers (loops)
		std::vector<Expression> SolveDistributedMultiply(std::vector<Expression> expressions);

		// Solves conditionals
		std::vector<Expression> SolveConditional(std::vector<Expression> expressions);

		Value DoOperation(Expression leftExp, Expression rightExp, Operand operand, Conditional conditional);

	private:
		// Produce a random number where n is low <= n <= high
		int RandomRange(int low, int high);

		std::string GetConditionalFriendlyName(Conditional conditional);
	};
}