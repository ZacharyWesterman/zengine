#include "scanner.hpp"
#include <z/system/console.h>
#include <z/core/memoryStream.h>

namespace z
{
	namespace compiler
	{
		bool scanner::scanOnce(z::core::inputStream& stream)
		{
			column = (stream.tell() >> 2) + 1;

			for (auto& rule : rules)
			{
				if (rule.matchPattern.match(stream))
				{
					auto text = rule.matchPattern.matched();

					//call the rule's special onMatch function if it exists
					if (rule.onMatch)
					{
						rule.onMatch(text);
					}

					return true;
				}
			}

			return false;
		}

		void scanner::scan(z::core::inputStream& stream)
		{
			if (!stream.good()) return;

			line = column = 1;

			zstring text;
			while(!stream.empty())
			{
				text.readln(stream);

				z::core::memoryStream lineStream (text.wstring(), text.length());
				lineStream.setFormat(z::utf32, true);

				zstring badText;

				while(!lineStream.empty())
				{
					bool matched = scanOnce(lineStream);

					//We didn't match any rule.. that's an error.
					if (!matched)
					{
						if (lineStream.empty()) break;
						badText.append(lineStream.getChar());
					}
					else
					{
						if (badText.length())
						{
							error(zstring("Invalid text \"")+badText+"\".");
							badText.clear();
						}
					}
				}

				if (badText.length()) error(zstring("Invalid text \"")+badText+"\".");

				++line;
			}
		}

		void scanner::error(const zstring& message)
		{
			z::system::console console;

			(zstring("Error (") + line + "," + column + ") : " + message).writeln(console);
		}
	}
}
