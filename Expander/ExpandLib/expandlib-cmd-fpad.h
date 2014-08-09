
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

/** \file expandlib-cmd-fpad.h
	\brief Headerfile for the 'fpad' command
*/

#ifndef EXPANDLIB_CMD_FPAD_H_INCLUDED
#define EXPANDLIB_CMD_FPAD_H_INCLUDED

#include <boost/lexical_cast.hpp>

#include "expandlib-cmd.h"

namespace expander { // *** Start of namespace

	/// The class implementing the 'fpad' command
	/** Syntax:<br>
		\#fpad(stringtopad,numberofcharstopadto,padchar)	Adds chars to the front of the string
	*/

	template<
		// Used for the expandable string (and the expanded string)
		class StringType = ::std::string
		>
	class expcmd_fpad : public expandcmd<StringType> {
	protected:
	public:
		/// The constructor
		/** This constructor gets a pointer to an expander::expandable object and passes that,
			along with the name of the command, to the expander::expandcmd constructor.
		*/
		expcmd_fpad(expandable<StringType> * const eobj) : expandcmd<StringType>(eobj, _T("fpad")) {};

		virtual void operator()(StringType &dest, const FParamVectorType &params, const FVarMapType &vars, ExpandResultsType &results) const
		{
			if ( params.size() == 3 && params[2].size() >= 1 ) {
				StringType s;
				expobj->expand(s, params[0], vars, results);

				try {
					const StringType::size_type num=boost::lexical_cast<StringType::size_type>(params[1]);

					if ( s.size() < num ) {
						const StringType::value_type padchar=params[2][0];
						dest.append(num-s.size(), padchar);
						dest+=s;
					} else {
						dest+=s;
					}
				} catch(boost::bad_lexical_cast) {
					throw expand_error(_T("expcmd_fpad"),_T("Illegal size"),params[1].c_str());
				}
			} else {
				throw expand_error(_T("expcmd_fpad"),_T("Parameter error"));
			}
		}
	};

} // *** End of namespace

#endif //EXPANDLIB_CMD_FPAD_H_INCLUDED
