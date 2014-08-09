
/* This file is part of Expandlib.

  Copyright (C) 2002 Jasper van de Gronde

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jasper van de Gronde th.v.d.gronde@hccnet.nl

*/

/** \file expandlib-vartype-numeric.h
	\brief Headerfile for the 'expvartype_numeric' vartype (a number)
*/

#ifndef EXPANDLIB_VARTYPE_NUMERIC_H_INCLUDED
#define EXPANDLIB_VARTYPE_NUMERIC_H_INCLUDED

#include <boost/lexical_cast.hpp>

#include <tchar.h>

#include "expandlib-vartype.h"
#include "expandlib-vartype-numeric-traits.h"

namespace expander { // *** Start of namespace

	/// The class that supplies the numeric vartype.
	/**	This class can be used in the program using ExpandLib for simple numeric variables.	
	*/

	template<
		// Used for the expandable string (and the expanded string)
		class StringType = ::std::string,
		class NumType = int,
		class NumTraits = expvartype_numeric_traits_normal<NumType>
		>
	class expvartype_numeric : public expandvartype<StringType> {
	protected:
		NumType val;
		StringType val_str;

	public:

		/// Default contructor, inits the value to 0
		expvartype_numeric()
		{
			val=NumTraits::defaultvalue();
			try {
				val_str=boost::lexical_cast<StringType>(val);
			} catch(...) {
				throw general_error(_T("expvartype_numeric"),_T("Error during lexical_cast<StringType>(val)."));
			}
		}

		/// Constructor with a parameter of type StringType, inits the value to 's' (converted)
		expvartype_numeric(const StringType &s)
		{
			try {
				set_value(boost::lexical_cast<NumType>(s));
			} catch(...) {
				val=NumTraits::defaultvalue();
			}
		}

		/// Constructor with a C-style string as parameter, inits the value to 's' (converted)
		expvartype_numeric(const _TCHAR * s)
		{
			try {
				set_value(boost::lexical_cast<NumType>(s));
			} catch(...) {
				val=NumTraits::defaultvalue();
			}
		}

		/// Constructor with a number as parameter, inits the value to 's' (converted)
		expvartype_numeric(const NumType v) { set_value(v); };

		virtual void AppendValue(StringType &dest) const throw()
		{
			dest+=val_str;
		}

		virtual void AppendValue(StringType &dest, const FParamVectorType &params, const FVarMapType &vars, ExpandResultsType &results) const throw()
		{
			dest+=val_str;
		}

		virtual void SetValue(const StringType &s) throw()
		{
			try {
				set_value(boost::lexical_cast<NumType>(s));
			} catch(...) {
				val=NumTraits::defaultvalue();
			}
		}

		/// Returns the value
		inline NumType get_value() { return val; }

		/// Sets the value
		inline void set_value(const NumType v)
		{
			val=NumTraits::validate(v);
			try {
				val_str=boost::lexical_cast<StringType>(val);
			} catch(...) {
				throw general_error(_T("expvartype_numeric"),_T("Error during lexical_cast<StringType>(val)."));
			}
		}
	};

} // *** End of namespace

#endif //EXPANDLIB_VARTYPE_NUMERIC_H_INCLUDED
