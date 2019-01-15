#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <string>
#include <vector>

#include "valuetype.h"

namespace DungeonZ
{
	class Value
	{
	public:
		ValueType Type;
		double Number;
		DiceRoll Dice;
		std::string String;

		Value();
		Value(double v);
		Value(DiceRoll v);
		Value(std::string s);
		int GetSetSum();
		std::vector<int> GetSortedSet();
		double ToDouble();
		int ToInt();
		std::string ToString();

		Value& operator +(const Value& rhs);
		Value& operator =(double rhs);
		Value& operator =(int rhs);

		friend std::ostream& operator <<(std::ostream&os, Value v);
	};
}

#endif