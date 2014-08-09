
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

/** \file expandlib-vartype-bool.h
	\brief Headerfile for the 'expvartype_bool' vartype (a boolean)
*/

#ifndef EXPANDLIB_VARTYPE_BOOL_H_INCLUDED
#define EXPANDLIB_VARTYPE_BOOL_H_INCLUDED

#include "expandlib-vartype.h"

namespace expander { // *** Start of namespace

	/// The class that supplies the bool vartype.
	/**	This class can be used in the program using ExpandLib for simple boolean variables.
		
		It accepts both yes/no and true/false (as well as TrueString/FalseString), and it outputs according to the TrueString and
		FalseString variables.
	*/

	template<
		// Used for the expandable string (and the expanded string)
		class StringType = ::std::string,
		bool DefaultValue = false
		>
	class expvartype_bool : public expandvartype<StringType> {
	protected:
		bool val;
		const StringType TrueString;
		const StringType FalseString;

	public:

		/// Default contructor, inits the value to DefaultValue
		expvartype_bool() : TrueString(_T("true")), FalseString(_T("false")) { val=DefaultValue; };

		/// Constructor with just TrueString and FalseString, inits the value to DefaultValue
		expvartype_bool(const StringType & ts, const StringType & fs) : TrueString(ts), FalseString(fs) { val=DefaultValue; };

		/// Constructor with a parameter of type bool, inits the value to 'b' (converted)
		expvartype_bool(const bool b) : TrueString(_T("true")), FalseString(_T("false")) { val=b; };

		/// Constructor with a parameter of type bool, inits the value to 'b' (converted) and TrueString and FalseString
		expvartype_bool(const bool b, const StringType & ts, const StringType & fs) : TrueString(ts), FalseString(fs) { val=b; };

		/// Constructor with a C-style string as parameter, inits the value to 's' (converted)
		expvartype_bool(const _TCHAR * cs) : TrueString(_T("true")), FalseString(_T("false"))
		{
			const StringType s(cs);
			SetValue(s);
		};

		/// Constructor with a C-style string as parameter, inits the value to 's' (converted) and TrueString and FalseString
		expvartype_bool(const _TCHAR * cs, const StringType & ts, const StringType & fs) : TrueString(ts), FalseString(fs)
		{
			const StringType s(cs);
			SetValue(s);
		};

		/// Constructor with a string as parameter, inits the value to 's' (converted)
		expvartype_bool(const StringType s) : TrueString(_T("true")), FalseString(_T("false"))
		{
			SetValue(s);
		};

		/// Constructor with a string as parameter, inits the value to 's' (converted)
		expvartype_bool(const StringType s, const StringType & ts, const StringType & fs) : TrueString(ts), FalseString(fs)
		{
			SetValue(s);
		};

		virtual void AppendValue(StringType &dest) const { dest+=( val ? TrueString : FalseString ); };

		virtual void AppendValue(StringType &dest, const FParamVectorType &params, const FVarMapType &vars, ExpandResultsType &results) const { dest+=( val ? TrueString : FalseString ); };

		virtual void SetValue(const StringType &s) throw()
		{
			if ( s == _T("yes") || s == _T("true") || s == TrueString ) {
				val=true;
			} else if ( s == _T("no") || s == _T("false") || s == FalseString ) {
				val=false;
			} else {
				val=DefaultValue;
			}
		};

		inline void operator=(const StringType &s)
		{
			if ( s == _T("yes") || s == _T("true") ) {
				val=true;
			} else if ( s == _T("no") || s == _T("false") ) {
				val=false;
			} else {
				val=DefaultValue;
			}
		};

		/// Conversion to bool
		inline bool& get_value() { return val; }
	};

} // *** End of namespace

#endif //EXPANDLIB_VARTYPE_BOOL_H_INCLUDED
