
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

/** \file expandlib-vartype-text.h
	\brief Headerfile for the 'expvartype_text' vartype (a simple string)
*/

#ifndef EXPANDLIB_VARTYPE_TEXT_H_INCLUDED
#define EXPANDLIB_VARTYPE_TEXT_H_INCLUDED

#include "expandlib-vartype.h"

namespace expander { // *** Start of namespace

	/// The class that supplies the text vartype.
	/**	This class can be used in the program using ExpandLib for simple text variables.
		
		It is based on both expandvartype as well as StringType, this makes it very flexible, as
		it inherits all the great stuff you can do with StringType (assignments, all kinds of
		string operations, whatever that type has to offer).

		NOTE: The following is somewhat obsolete:
		
		This class should also be used (at least for now) for numeric variables, just convert them
		to a string. All numeric values are handled like strings at the moment, they are only
		converted to "real" (as in true, not as in floating point) numeric values when needed.
	*/

	template<
		// Used for the expandable string (and the expanded string)
		class StringType = ::std::string
		>
	class expvartype_text : public expandvartype<StringType> {
	protected:
		StringType value;

	public:

		/// Default contructor, inits the string to ""
		expvartype_text() {}

		/// Constructor with a parameter of type StringType, inits the string to 's'
		expvartype_text(const StringType &s) : value(s) {}

		/// Constructor with a C-style string as parameter, inits the string to 's'
		expvartype_text(const _TCHAR * s) : value(s) {}

		virtual void AppendValue(StringType &dest) const { /*dest+=*this; */dest.append(value.begin(), value.end()); }

		virtual void AppendValue(StringType &dest, const FParamVectorType &params, const FVarMapType &vars, ExpandResultsType &results) const { /*dest+=*this; */dest.append(value.begin(), value.end()); }

		virtual void SetValue(const StringType &s) { value=s; }

		/// Returns the string
		inline StringType& get_string() { return value; }
	};

} // *** End of namespace

#endif //EXPANDLIB_VARTYPE_TEXT_H_INCLUDED
