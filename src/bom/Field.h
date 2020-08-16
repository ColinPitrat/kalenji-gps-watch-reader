#ifndef _BOM_FIELD_H
#define _BOM_FIELD_H

class TFieldUndef
{
	public:
		TFieldUndef() = default;
};

static const TFieldUndef FieldUndef = TFieldUndef();

template<typename T>
class Field
{
	public:
		// Constructor from a value. If we have a value, then the field is valid.
		Field(T value) : _defined(true), _value(value) {};

		// Constructor of an invalid field, using the constant defined above.
		Field(TFieldUndef unused) : _defined(false), _value(T()) {};

		bool isDefined() const { return _defined; };
		const T& getValue() const { return _value; };

		// Implicit conversion
		operator T () const { return _value; };

		// A function to be able to print a string containing the value only if field is defined
		std::string toStream(const std::string& before = "", const std::string& after = "") const
		{
			if (_defined)
			{
				std::ostringstream oss;
				oss.precision(8);
				oss << before << _value << after;
				return oss.str();
			}
			return "";
		}
		

	private:
		bool _defined;
		T _value;
};

#endif
