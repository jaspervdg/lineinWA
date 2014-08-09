
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

/** \file expandlib-cmd-lower.h
	\brief Headerfile for the 'lower' command
*/

#ifndef EXPANDLIB_CMD_LOWER_H_INCLUDED
#define EXPANDLIB_CMD_LOWER_H_INCLUDED

#include <locale>

#include "expandlib-cmd.h"

namespace expander { // *** Start of namespace

	/// The class implementing the 'lower' command
	/** Syntax:<br>
		\#lower(stringtolow)	Makes a string lowercase
	*/

	template<
		// Used for the expandable string (and the expanded string)
		class StringType = ::std::string
		>
	class expcmd_lower : public expandcmd<StringType> {
	protected:
		/// Used for the conversion
		const std::locale loc;

	public:
		/// The constructor
		/** This constructor gets a pointer to an expander::expandable object and passes that,
			along with the name of the command, to the expander::expandcmd constructor.
		*/
		expcmd_lower(expandable<StringType> * const eobj) : expandcmd<StringType>(eobj, _T("lower")), loc("") {};

		virtual void operator()(StringType &dest, const FParamVectorType &params, const FVarMapType &vars, ExpandResultsType &results) const
		{
			if ( params.size() == 1 ) {
				const StringType::iterator u=dest.end();
				expobj->expand(dest, params[0], vars, results);

				std::use_facet< std::ctype<StringType::value_type> >(loc).tolower(u, dest.end());
			} else {
				throw expand_error(_T("expcmd_lower"),_T("Parameter error"));
			}
		}
	};

} // *** End of namespace

#endif //EXPANDLIB_CMD_LOWER_H_INCLUDED
