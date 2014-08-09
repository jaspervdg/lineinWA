
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

/** \file expandlib-vartype.h
	\brief Headerfile for the (abstract) vartype base class
*/

#ifndef EXPANDLIB_VAR_H_INCLUDED
#define EXPANDLIB_VAR_H_INCLUDED

#include <string>
#include <vector>
#include <map>

#include "expandlib-results.h"
#include "expandlib-exceptions.h"

namespace expander { // *** Start of namespace

	/// An abstract base class for the vartypes
	/**	This is the class all vartypes derive from.
	*/

	template<
		// Used for the expandable string (and the expanded string)
		class StringType = ::std::string
		>
	class expandvartype {
	protected:
		typedef ::std::vector<StringType> FParamVectorType; ///< The type used for the parameter vectors (MSVC wouldn't allow me to use template arguments in these typedefs, or I don't know how it should be done)
		typedef ::std::map<StringType, ::expander::expandvartype<StringType>*> FVarMapType; ///< The type used for the map of variables (MSVC wouldn't allow me to use template arguments in these typedefs, or I don't know how it should be done)

	public:
		/// Appends the value of the var to a string
		/** This is called by operator+=(StringType &s,::expander::expandvartype&lt;StringType&gt; *v),
			 which is called by 'expandable&lt;StringType&gt;::expand' when a variable without
			 parameters has been found.
		*/
		virtual void AppendValue(StringType &dest) const = 0;

		/// Appends the value of the var to a string, using params and rvars, putting any expanding results into results
		/** This is called directly from 'expandable&lt;StringType&gt;::expand', when a variable with
			 parameters has been found.
		*/
		virtual void AppendValue(StringType &dest, const FParamVectorType &params, const FVarMapType &vars, ExpandResultsType &results) const = 0;

		/// Sets this variable to the string specified
		/** This isn't called from ExpandLib itself, but it can be used by other programs.
		*/
		virtual void SetValue(const StringType &s) = 0;
	};

} // *** End of namespace

#endif //EXPANDLIB_VAR_H_INCLUDED
