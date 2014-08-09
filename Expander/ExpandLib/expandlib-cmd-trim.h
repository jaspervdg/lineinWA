
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

/** \file expandlib-cmd-trim.h
	\brief Headerfile for the 'trim' command
*/

#ifndef EXPANDLIB_CMD_TRIM_H_INCLUDED
#define EXPANDLIB_CMD_TRIM_H_INCLUDED

#include "expandlib-cmd.h"

namespace expander { // *** Start of namespace

	/// The class implementing the 'trim' command
	/** Syntax:<br>
		\#trim(stringtotrim,charstotrim)	Trims a string
	*/

	template<
		// Used for the expandable string (and the expanded string)
		class StringType = ::std::string
		>
	class expcmd_trim : public expandcmd<StringType> {
	protected:
	public:
		/// The constructor
		/** This constructor gets a pointer to an expander::expandable object and passes that,
			along with the name of the command, to the expander::expandcmd constructor.
		*/
		expcmd_trim(expandable<StringType> * const eobj) : expandcmd<StringType>(eobj, _T("trim")) {};

		/** \todo Write more optimal version */
		virtual void operator()(StringType &dest, const FParamVectorType &params, const FVarMapType &vars, ExpandResultsType &results) const
		{
			if ( params.size() == 2 ) {
				StringType d;
				expobj->expand(d, params[0], vars, results);

				dest.append(d.begin()+d.find_first_not_of(params[1]), d.begin()+d.find_last_not_of(params[1])+1);
			} else {
				throw expand_error(_T("expcmd_trim"),_T("Parameter error"));
			}
		}
	};

} // *** End of namespace

#endif //EXPANDLIB_CMD_TRIM_H_INCLUDED
