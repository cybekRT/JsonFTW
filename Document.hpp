#pragma once

#include "Value.hpp"
#include <string>

namespace JsonFTW
{
	class Document : public ValueArray
	{
	protected:
		std::string path;
		bool saveOnClose;

	public:
		Document();
		Document(const std::string &path, bool saveOnClose = false, const std::string &defaultValues = "");
		~Document();

		void Load(const std::string &path, bool saveOnClose = false, const std::string &defaultValues = ""); // defaultValues are used only if saveOnClose is true
		void Save(const std::string &path);

		Value &operator[](unsigned index)
		{
			return ValueArray::operator[](index);
		}

		Value &operator[](const std::string &key)
		{
			return ValueArray::operator[](key);
		}

	protected:
		void Save(std::ostream &, ValueArray &, std::string prefix = "");
		void Trim(char *);
		unsigned Parse(char *, unsigned, ValueArray &, unsigned * = nullptr); // Returns 0 on error, (character next after last read) on success
		void ParseString(char *, unsigned);
	};
}
