
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

/** \file expandlib-cmd-numvars.h
	\brief Headerfile for the 'numvars' command
*/

#ifndef EXPANDLIB_CMD_NUMVARS_H_INCLUDED
#define EXPANDLIB_CMD_NUMVARS_H_INCLUDED

#include <boost/lexical_cast.hpp>

#include "expandlib-cmd.h"

namespace expander { // *** Start of namespace

	/// The class implementing the 'numvars' command
	/** Syntax:<br>
		\#numvars(stringwithvars)	Returns the number of vars in the string
	*/

	template<
		// Used for the expandable string (and the expanded string)
		class StringType = ::std::string
		>
	class expcmd_numvars : public expandcmd<StringType> {
	protected:
	public:
		/// The constructor
		/** This constructor gets a pointer to an expander::expandable object and passes that,
			along with the name of the command, to the expander::expandcmd constructor.
		*/
		expcmd_numvars(expandable<StringType> * const eobj) : expandcmd<StringType>(eobj, _T("numvars")) {};

		virtual void operator()(StringType &dest, const FParamVectorType &params, const FVarMapType &vars, ExpandResultsType &results) const
		{
			if ( params.size() == 1 ) {
				ExpandResultsType r;
				StringType d;
				expobj->expand(d, params[0], vars, r);

				try {
					dest+=boost::lexical_cast<StringType>(r.TotalVars());
				} catch(...) {
					throw general_error(_T("expcmd_numvars"),_T("Error during lexical_cast<StringType>(r.TotalVars())"));
				}
			} else {
				throw expand_error(_T("expcmd_numvars"),_T("Parameter error"));
			}
		}
	};

} // *** End of namespace

#endif //EXPANDLIB_CMD_NUMVARS_H_INCLUDED
