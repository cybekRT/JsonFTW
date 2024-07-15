#pragma once

#include"Exception.hpp"
#include<string>
#include<vector>
#include<cstdint>

namespace JsonFTW
{
	/*
		Base
	*/
	class Value
	{
	//protected:
	public:
		std::string name;
		Value *parent;

		enum class Type
		{
			Null, Bool, Integer, Double, String, Array
		};
		Type type;

	public:
		Value(Type type, std::string name) : type(type), name(name), parent(nullptr)
		{
		}

		~Value()
		{

		}

		std::string GetAbsoluteName()
		{
			std::string name = this->name;
			Value *ptr = parent;
			while (ptr != nullptr)
			{
				name = ptr->name + "::" + name;
				ptr = ptr->parent;
			}

			return name;
		}

		Type GetType()
		{
			return type;
		}

		virtual void* GetNull()
		{
			throw InvalidTypeException(GetAbsoluteName() + " is not ValueNull");
		};

		virtual bool GetBool()
		{
			throw InvalidTypeException(GetAbsoluteName() + " is not ValueBool");
		}

		virtual int32_t GetInt()
		{
			throw InvalidTypeException(GetAbsoluteName() + " is not ValueInt");
		}

		virtual double GetDouble()
		{
			throw InvalidTypeException(GetAbsoluteName() + " is not ValueDouble");
		}

		virtual std::string GetString()
		{
			throw InvalidTypeException(GetAbsoluteName() + " is not ValueString");
		}

		virtual std::vector<Value*>::iterator begin()
		{
			throw InvalidTypeException(GetAbsoluteName() + " is not ValueArray");
		}

		virtual std::vector<Value*>::iterator end()
		{
			throw InvalidTypeException(GetAbsoluteName() + " is not ValueArray");
		}

		virtual Value &operator[](unsigned index)
		{
			throw InvalidTypeException(GetAbsoluteName() + " is not ValueArray");
		}

		virtual Value &operator[](std::string key)
		{
			throw InvalidTypeException(GetAbsoluteName() + " is not ValueArray");
		}
	};

	/*
		Null
	*/
	class ValueNull : public Value
	{
	public:
		ValueNull(std::string name) : Value(Type::Null, name)
		{
		}

		operator void*()
		{
			return nullptr;
		}

		void *GetNull()
		{
			return nullptr;
		}
	};

	/*
		Bool
	*/
	class ValueBool : public Value
	{
	protected:
		bool value;

	public:
		ValueBool(std::string name, bool value) : Value(Type::Bool, name), value(value)
		{

		}

		operator bool()
		{
			return value;
		}

		ValueBool& operator=(bool value)
		{
			this->value = value;
			return *this;
		}

		bool GetBool()
		{
			return value;
		}
	};

	/*
		Integer
	*/
	class ValueInt : public Value
	{
	protected:
		int32_t value;

	public:
		ValueInt(std::string name, int32_t value) : Value(Type::Integer, name), value(value)
		{

		}

		operator int32_t()
		{
			return value;
		}

		ValueInt& operator=(int32_t value)
		{
			this->value = value;
			return *this;
		}

		int32_t GetInt()
		{
			return value;
		}

		double GetDouble()
		{
			return (double)value;
		}
	};

	/*
		Double
	*/
	class ValueDouble : public Value
	{
	protected:
		double value;

	public:
		ValueDouble(std::string name, double value) : Value(Type::Double, name), value(value)
		{

		}

		operator double()
		{
			return value;
		}

		ValueDouble& operator=(double value)
		{
			this->value = value;
			return *this;
		}

		double GetDouble()
		{
			return value;
		}
	};

	/*
		String
	*/
	class ValueString : public Value
	{
	protected:
		std::string value;

	public:
		ValueString(std::string name, std::string value) : Value(Type::String, name), value(value)
		{

		}

		operator std::string()
		{
			return value;
		}

		ValueString& operator=(std::string value)
		{
			this->value = value;
			return *this;
		}

		std::string GetString()
		{
			return value;
		}
	};

	/*
		Array
	*/
	class ValueArray : public Value
	{
	//protected:
	public:
		std::vector<Value*> childNodes;
		bool unnamedChildren;

	public:
		ValueArray(std::string name, bool unnamedChildren = false) : Value(Type::Array, name), unnamedChildren(unnamedChildren)
		{

		}

		~ValueArray()
		{
			childNodes.clear();
		}

		Value& operator[](unsigned index)
		{
			if (index >= childNodes.size())
				throw InvalidIndexException("Array = " + GetAbsoluteName() + ", Index = " + std::to_string(index) + ", Size = " + std::to_string(childNodes.size()));

			return *childNodes[index];
		}

		Value& operator[](std::string key)
		{
			if (unnamedChildren)
				throw KeyNotFoundException("Array = " + GetAbsoluteName() + ", array has unnamed children");

			for (auto v : childNodes)
			{
				if (v->name == key)
					return *v;
			}

			throw KeyNotFoundException("Array = " + GetAbsoluteName() + ", Key = " + key);
		}

		virtual std::vector<Value*>::iterator begin()
		{
			return childNodes.begin();
		}

		virtual std::vector<Value*>::iterator end()
		{
			return childNodes.end();
		}

		bool Has(const std::string &key)
		{
			if (unnamedChildren)
				throw KeyNotFoundException("Array = " + GetAbsoluteName() + ", array has unnamed children");

			for (auto v : childNodes)
			{
				if (v->name == key)
					return true;
			}

			return false;
		}

		void PrintElements(std::string prefix = "")
		{
			printf("%s%s ->\n", prefix.c_str(), this->name.c_str());
			for (auto v : childNodes)
			{
				switch (v->GetType())
				{
					case Type::Null:
						printf("%s\t- %s = null\n", prefix.c_str(), v->name.c_str());
						break;
					case Type::Bool:
						printf("%s\t- %s = %d\n", prefix.c_str(), v->name.c_str(), v->GetBool());
						break;
					case Type::Double:
						printf("%s\t- %s = %lf\n", prefix.c_str(), v->name.c_str(), v->GetDouble());
						break;
					case Type::String:
						printf("%s\t- %s = %s\n", prefix.c_str(), v->name.c_str(), v->GetString().c_str());
						break;
					case Type::Array:
						((ValueArray*)v)->PrintElements(prefix + "\t");
						break;
				}
			}
		}
	};
}
