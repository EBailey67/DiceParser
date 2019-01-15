#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>

#include "diceroll.h"
#include "value.h"


namespace DungeonZ
{
	// Constructors
	Value::Value() : Type(ValueType::Unknown), Number(0.0), Dice(DiceRoll(1,6, false)), String("") { }
	Value::Value(double v) : Type(ValueType::Number), Number(v), Dice(DiceRoll(1,6, false)), String("")	{ }
	Value::Value(DiceRoll v) : Type(ValueType::DiceRoll), Number(0.0), Dice(v), String("")  { }
	Value::Value(std::string s) : Type(ValueType::String), Number(0.0), Dice(DiceRoll(1,6,false)), String(s)  { }

	int Value::GetSetSum()
	{
		int sum = 0;

		if (Type != ValueType::DiceRoll)
			return 0;

		for (auto i : Dice.Rolls)
			sum += i;

		return sum;
	}

	std::vector<int> Value::GetSortedSet()
	{
		std::vector<int> sortedSet(Dice.Rolls);
		std::sort(sortedSet.begin(), sortedSet.end());
		return sortedSet;
	}

	double Value::ToDouble()
	{
		switch (Type)
		{
		case ValueType::Number:
			return Number;
		case ValueType::String:
			return std::stod(String);
		case ValueType::DiceRoll:
			return GetSetSum();
		default:
			return 0;
		}

		return 0;
	}

	int Value::ToInt()
	{
		switch (Type)
		{
		case ValueType::Number:
			return static_cast<int>(std::round(Number));
		case ValueType::String:
			return std::stoi(String);
		case ValueType::DiceRoll:
			return GetSetSum();
		default:
			return 0;
		}

		return 0;
	}

	std::string Value::ToString()
	{
		std::stringstream result;

		switch (Type)
		{
			case ValueType::Number:
				result << Number;
				break;
			case ValueType::String:
				result << String;
				break;
			case ValueType::DiceRoll:
			{

				result << "(";
				for (size_t i = 0; i < Dice.Rolls.size(); i++)
				{
					if (Dice.Sides != -1) // Fudge Dice, no highlighting
					{
						int offset = 0;
						if (Dice.IsZeroBased)
							offset = 1;

						result << Dice.Rolls[i];
						if (Dice.Rolls[i] == 1 - offset)
							result << "-";		// Rolled minimum
						else if (Dice.Rolls[i] == Dice.Sides - offset)
							result << "+";		// Rolled maximum
						else if (Dice.Rolls[i] > Dice.Sides - offset)
							result << "++";	// Rolled > max die (exploding dice)
					}
					else
					{
						result << "[" << Dice.Rolls[i] << "]";
					}

					if (i < Dice.Rolls.size() - 1)
						result << ",";
				}
				result << ")";
			}
			break;
			default:
				return "unknown";
		}
		return result.str();
	}

	Value& Value::operator +(const Value& rhs)
	{
		if (this == &rhs)
			return *this;

		this->Type = rhs.Type;
		
		switch (Type)
		{
			case ValueType::String:
				this->String = rhs.String;
				break;
			case ValueType::Number:
				this->Number = rhs.Number;
				break;
			case ValueType::DiceRoll:
				{
				// Append the two sets
				for(auto i : rhs.Dice.Rolls)
					this->Dice.Rolls.push_back(i);
				}
				break;
			default:
				break;
		}

		return *this;
	}

	Value& Value::operator =(double rhs)
	{
		this->Type = ValueType::Number;
		this->Number = rhs;

		return *this;
	}

	Value& Value::operator =(int rhs)
	{
		this->Type = ValueType::Number;
		this->Number = rhs;

		return *this;
	}


	std::ostream& operator <<(std::ostream&os, Value v)
	{
		os << v.ToString();
		return os;
	}
}
