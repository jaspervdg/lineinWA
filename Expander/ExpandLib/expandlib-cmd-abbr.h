
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

/** \file expandlib-cmd-abbr.h
	\brief Headerfile for the 'abbr' command
*/

#ifndef EXPANDLIB_CMD_ABBR_H_INCLUDED
#define EXPANDLIB_CMD_ABBR_H_INCLUDED

#include <boost/lexical_cast.hpp>

#include "expandlib-cmd.h"

namespace expander { // *** Start of namespace

	/// The class implementing the 'abbr' command
	/** Syntax:<br>
		\#abbr(stringtoabbreviate[,minsizetoabbreviate])	Abbreviates a string (takes the first letter of every word), but only if it's longer than minsizetoabbreviate
	*/

	template<
		// Used for the expandable string (and the expanded string)
		class StringType = ::std::string
		>
	class expcmd_abbr : public expandcmd<StringType> {
	protected:
	public:
		/// The constructor
		/** This constructor gets a pointer to an expander::expandable object and passes that,
			along with the name of the command, to the expander::expandcmd constructor.
		*/
		expcmd_abbr(expandable<StringType> * const eobj) : expandcmd<StringType>(eobj, _T("abbr")) {};

		virtual void operator()(StringType &dest, const FParamVectorType &params, const FVarMapType &vars, ExpandResultsType &results) const
		{
			if ( params.size() == 1 ) {
				StringType s;
				expobj->expand(s, params[0], vars, results);

				{
//					bool storechar=true;
					for(StringType::const_iterator u=s.begin(); u!=s.end(); ++u) {
						dest+=*u;
						for(; u!=s.end(); ++u) {
							if ( *u == _T(' ') || *u == _T(',') || *u == _T('\\') ) break;
						}
/*						if ( storechar ) {
							dest+=*u;
							storechar=false;
						} else if ( *u == _T(' ') || *u == _T(',') || *u == _T('\\') ) {
							storechar=true;
						}*/
					}
				}
			} else if ( params.size() == 2 ) {
				StringType s;
				expobj->expand(s, params[0], vars, results);

				{
					try {
						const StringType::size_type num=boost::lexical_cast<StringType::size_type>(params[1]);

						if ( s.size() > num ) {
		//					bool storechar=true;
							for(StringType::const_iterator u=s.begin(); u!=s.end(); ++u) {
								dest+=*u;
								for(; u!=s.end(); ++u) {
									if ( *u == _T(' ') || *u == _T(',') || *u == _T('\\') ) break;
								}
		/*						if ( storechar ) {
									dest+=*u;
									storechar=false;
								} else if ( *u == _T(' ') || *u == _T(',') || *u == _T('\\') ) {
									storechar=true;
								}*/
							}
						} else {
							dest+=s;
						}
					} catch(boost::bad_lexical_cast) {
						throw expand_error(_T("expcmd_abbr"),_T("Illegal size"),params[1].c_str());
					}
				}
			} else {
				throw expand_error(_T("expcmd_abbr"),_T("Parameter error"));
			}
		}
	};

} // *** End of namespace

#endif //EXPANDLIB_CMD_ABBR_H_INCLUDED
