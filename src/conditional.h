#pragma once

namespace DungeonZ
{
	enum class Conditional
	{
		None = 0,            // No conditional
		EqualTo,             // = - Conditional Equal
		GreaterThan,         // > - Conditional Greater than
		LessThan,            // < - Conditional Less than
		AtLeast,             // >= - Conditional Greater than or equal
		AtMost,              // <= - Conditional Less than or equal
		NotEqual,            // != or <> - Conditional Not Equal to
	};
}
