#include"Document.hpp"
#include<fstream>
#include<cstring>
#include<strings.h>
#include<map>

namespace JsonFTW
{
	Document::Document() : ValueArray("")
	{

	}

	Document::Document(const std::string &path, bool saveOnClose, const std::string &defaultValues) : ValueArray(path)
	{
		Load(path, saveOnClose, defaultValues);
	}

	Document::~Document()
	{
		if (saveOnClose)
		{
			Save(path);
		}
	}

	void Document::Load(const std::string &path, bool saveOnClose, const std::string &defaultValues)
	{
		this->path = path;
		this->saveOnClose = saveOnClose;

#if _WIN32 && _DEBUG
		LARGE_INTEGER hpetFreq, hpetStart, hpetStop;
		QueryPerformanceFrequency(&hpetFreq);
		QueryPerformanceCounter(&hpetStart);
#endif

		char *data;
		std::ifstream f(path.c_str(), std::ios::binary);
		if (!f)
		{
			if (saveOnClose)
			{
				unsigned dataSize = defaultValues.length() + 1;
				data = new char[dataSize];
				memcpy(data, defaultValues.c_str(), dataSize);
			}
			else
				throw FileNotFoundException(path);
		}
		else
		{
			f.seekg(0, std::ios::end);
			unsigned fSize = (unsigned)f.tellg();
			f.seekg(0, std::ios::beg);

			data = new char[fSize + 1];
			f.read(data, fSize);
			data[fSize] = 0;

			f.close();
		}

		//Trim(data);

		/*std::ofstream of("test_of.json", std::ios::binary);
		of.write(data, strlen(data));
		of.close();*/

		//if (!Parse(data, 0, *this))
		//	return false;
		Parse(data, 0, *this);

#if _DEBUG
		PrintElements();
		QueryPerformanceCounter(&hpetStop);
		
		LARGE_INTEGER diff;
		diff.QuadPart = hpetStop.QuadPart - hpetStart.QuadPart;
		printf("Loading time of %s is %lfms\n", path.c_str(), diff.QuadPart / (double)hpetFreq.QuadPart * 1000.0);
#endif

		//return true;
	}

	void Document::Save(const std::string &path)
	{
		this->path = path;

		std::ofstream f(path.c_str());// , std::ios::binary);
		Save(f, *this);
		f.close();
	}

	void Document::Save(std::ostream &f, ValueArray &array, std::string prefix)
	{
		auto WriteString = [&f](const std::string &text)
		{
			for (auto v : text)
			{
				if (v == '"')
					f << "\\\"";
				else
					f << v;
			}
		};

		if (array.parent != nullptr && ((ValueArray*)array.parent)->unnamedChildren)
			f << prefix;

		if (array.unnamedChildren)
			f << "[\n";
		else
			f << "{\n";

		prefix += "\t";
		for (auto itr = array.childNodes.begin(); ;)
		{
			Value *v = *itr;
			if (!array.unnamedChildren)
			{
				f << prefix << "\"";
				WriteString(v->name);
				f << "\": ";
			}

			switch (v->GetType())
			{
				case Value::Type::Null:
					f << "null";
					break;
				case Value::Type::Bool:
					if (v->GetBool())
						f << "true";
					else
						f << "false";
					break;
				case Value::Type::Double:
					f << v->GetDouble();
					break;
				case Value::Type::String:
					f << "\"";
					WriteString(v->GetString());
					f << "\"";
					break;
				case Value::Type::Array:
					ValueArray *vArray = (ValueArray*)v;
					Save(f, *vArray, prefix);
					break;
			}

			itr++;
			if (itr != array.childNodes.end())
			{
				f << ",\n";
			}
			else
			{
				f << "\n";
				break;
			}
		}
		prefix.erase(prefix.end() - 1);

		if (array.unnamedChildren)
			f << prefix << "]";
		else
			f << prefix << "}";
	}

	void Document::Trim(char *text)
	{
		bool isEscaped = false;
		unsigned toMove = 0;

		for (unsigned a = 0; ; ++a)
		{
			if (text[a] == '"' && text[a - 1] != '\\')
			{
				text[a - toMove] = text[a];
				isEscaped = !isEscaped;
			}
			else
			{
				text[a - toMove] = text[a];
				if (!isEscaped)
				{
					if (text[a] == ' ' || text[a] == '\r' || text[a] == '\n' || text[a] == '\t')
						toMove++;
				}
			}

			if (text[a] == 0)
				break;
		}
	}

	unsigned Document::Parse(char *text, unsigned startIndex, ValueArray &array, unsigned *currentLine)
	{
		unsigned _currentLine = 1;
		if (currentLine == nullptr)
			currentLine = &_currentLine;

		auto EndOfString = [text](unsigned startIndex, unsigned &endIndex) -> bool
		{
			if (text[startIndex] != '"')
				return false;

			bool escaped = false;
			for (unsigned a = startIndex + 1;; ++a)
			{
				if (text[a] == '\\')
					escaped = true;
				else if (!escaped && text[a] == '"')
				{
					endIndex = a;
					return true;
				}
				else if (text[a] == 0)
					return false;
				else
					escaped = false;
			}

			return false;
		};

		auto EndOfDouble = [text](unsigned startIndex, unsigned &endIndex) -> bool
		{
			if ((text[startIndex] > '9' || text[startIndex] < '0') && text[startIndex] != '.')
			{
				return false;
			}

			for (unsigned a = startIndex;; ++a)
			{
				if ((text[a + 1] > '9' || text[a + 1] < '0') && text[a + 1] != '.')
				{
					endIndex = a;
					break;
				}
				else if (text[a + 1] == 0)
				{
					return false;
				}
			}

			return true;
		};

		auto NextChar = [text, currentLine](unsigned &index)
		{
			for (; text[index] != 0; index++)
			{
				if (text[index] == ' ' || text[index] == '\r' || text[index] == '\t') // White spaces
				{
					continue;
				}
				else if (text[index] == '\n') // New line
				{
					(*currentLine)++;
					continue;
				}
				else
					break;
			}
		};

		NextChar(startIndex);
		char bracket = text[startIndex];
		if (bracket != '{' && bracket != '[')
			throw ParsingException("Bracket expected at " + std::to_string(startIndex));

		unsigned a;
		for (a = startIndex + 1;; ++a)
		{
			NextChar(a);
			if ((bracket == '{' && text[a] == '}') || (bracket == '[' && text[a] == ']'))
				break;
			else if (text[a] == 0)
				throw ParsingException("Unexpected end of file");

			Value *value;
			if (text[a] == '{' || text[a] == '[') // Unnamed array
			{
				value = new ValueArray("");
				a = Parse(text, a, *(ValueArray*)value, currentLine);
				//if (!a)
				//	return 0;
			}
			else // key-value pair
			{
				// Key
				NextChar(a);

				char *key;
				bool hasKey;
				unsigned endIndex;
				if (text[a] == '"')
				{
					hasKey = true;
					if (!EndOfString(a, endIndex))
						throw ParsingException("Identifier not terminated at line " + std::to_string(*currentLine));
				}
				else
				{
					hasKey = false;
					if (!EndOfDouble(a, endIndex))
						throw ParsingException("Identifier not terminated at line " + std::to_string(*currentLine));
				}

				if(hasKey)
				{
					key = new char[endIndex - a];
					memcpy(key, text + a + 1, endIndex - a - 1);
					key[endIndex - a - 1] = 0;
					ParseString(key, endIndex - a);
					a = endIndex + 1;
					NextChar(a);
				}
				else
				{
					key = new char[1];
					key[0] = 0;
				}

				if (hasKey)
				{
					if (text[a] != ':')
						throw ParsingException("Colon expected at line " + std::to_string(*currentLine));
					else
						a++;
				}

				// Value
				NextChar(a);

				//printf("Testing: \"%.*s\"\n", 10, text + a);

				if (text[a] == '"')
				{
					if (!EndOfString(a, endIndex))
						throw ParsingException("Identifier not terminated at line " + std::to_string(*currentLine));

					char *valueStr = new char[endIndex - a + 1];
					memcpy(valueStr, text + a + 1, endIndex - a - 1);
					valueStr[endIndex - a - 1] = 0;
					ParseString(valueStr, endIndex - a);
					a = endIndex + 1;

					value = new ValueString(key, valueStr);
					delete[] valueStr;
				}
				else if (strncasecmp("null", text + a, 4) == 0)
				{
					value = new ValueNull(key);
					a += 4;
				}
				else if (strncasecmp("true", text + a, 4) == 0)
				{
					value = new ValueBool(key, true);
					a += 4;
				}
				else if (strncasecmp("false", text + a, 5) == 0)
				{
					//printf("Found: \"false\"\n");
					value = new ValueBool(key, false);
					a += 5;
				}
				else if (text[a] == '[' || text[a] == '{')
				{
					value = new ValueArray(key, text[a] == '[');
					a = Parse(text, a, *(ValueArray*)value, currentLine);
				}
				else // double
				{
					char *endPtr = nullptr;
					value = new ValueDouble(key, std::strtod(text + a, &endPtr));
					a = endPtr - text;
				}
				// throw ParsingException("Identifier expected at line " + std::to_string(*currentLine));
			}

			value->parent = &array;
			array.childNodes.push_back(value);

			NextChar(a);
			if (text[a] == ',')
				continue;
			else if (text[a] == '}' || text[a] == ']')
				break;
			else
				throw ParsingException((std::string)"Found: \"" + text[a] + "\", closing bracket expected at line " + std::to_string(*currentLine));
		}

		return a + 1;
	}

	void Document::ParseString(char *text, unsigned length)
	{
		if (length <= 1)
			return;

		unsigned offset = 0;
		for (unsigned a = 0; a < length; ++a)
		{
			text[a - offset] = text[a];
			if (text[a] == '\\' && text[a + 1] == '"')
			{
				offset++;
			}
		}

		text[length - offset - 1] = 0;
	}
}
