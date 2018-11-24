#pragma once

#include<string>

namespace JsonFTW
{
	class Exception
	{
	public:
		std::string message;

		Exception(std::string message="") : message(message)
		{

		}
	};

	class FileNotFoundException : public Exception
	{
	public:
		FileNotFoundException(std::string msg) : Exception(msg)
		{

		}
	};

	class InvalidIndexException : public Exception
	{
	public:
		InvalidIndexException(std::string msg) : Exception(msg)
		{

		}
	};

	class KeyNotFoundException : public Exception
	{
	public:
		KeyNotFoundException(std::string msg) : Exception(msg)
		{

		}
	};

	class InvalidTypeException : public Exception
	{
	public:
		InvalidTypeException(std::string msg) : Exception(msg)
		{

		}
	};

	class ParsingException : public Exception
	{
	public:
		ParsingException(std::string msg) : Exception(msg)
		{

		}
	};
}