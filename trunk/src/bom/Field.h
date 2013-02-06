#ifndef _BOM_FIELD_H
#define _BOM_FIELD_H

class TFieldUndef 
{
	public:
		TFieldUndef() {};
};

static const TFieldUndef FieldUndef;

template<typename T>
class Field
{
	public:
		// Constructor from a value. If we have a value, then the field is valid.
		Field(T value) 
		{ 
			_defined = true; 
			_value = value;
		};

		// Constructor of an invalid field, using the constant defined above.
		Field(TFieldUndef unused)
		{
			_defined = false;
		};

		bool isDefined() { return _defined; };

		// Implicit conversion
		operator T () { return _value; };

		// A function to be able to print a string containing the value only if field is defined
		std::string toStream(std::string before = "", std::string after = "") 
		{ 
			if (_defined) 
			{
				std::ostringstream oss;
				oss << before << _value << after;
				return oss.str(); 
			}
			return "";
		}
		

	private:
		T _value;
		bool _defined;
};

#endif
