
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

/** \file expandlib-expandable.h
	\brief The most important header, it contains the expandable template class.
*/

/*! \mainpage ExpandLib
 * \author Jasper van de Gronde
 *
 * \section intro Introduction
 *
 * ExpandLib is a "library" that makes it possible (if not very easy) to use expandable strings
 * in your programs, like the ones used to specify what the title should look like in the playlist.
 * Only this program supports more advanced strings, using named variables and commands, it even
 * supports different variable types (standard are normal strings and lists of normal strings, but
 * it isn't hard to create other types).
 * 
 * The syntax is similar to that used by the NullSoft Vorbis input plugin. An example: <code>#if(${title};${album}\\\\\\\\${artist} - #fpad(${tracknumber};2;0) - ${title};${path} - ${filename})</code>.
 * It would output something like this: <code>Beautifull album\\Great artist - 04 - The fourth incredible track</code>.
 * Or if ${title} was undefined: <code>Some kind of path - Some filename</code>.
 * Of course the program using ExpandLib will have to fill-in all these variables, but that's not
 * difficult.
 *
 * \section usage Usage
 * 
 * To use ExpandLib in your program:
 * <ol>
 *  <li>Make sure the expandlib directory is in your include path, or the directory one level up.
 *  <li>Include (expandlib/)expandlib-expandable.h, as well as any vartype headers you want to use (normally you would at least want expandlib-vartype-text.h)
 *  <li>Make an expandable object and fill it with a string to be expanded
 *  <li>Create an expandresults object (to give to the expandto function).
 *  <li>Create a map of variables using the expander::expandable::FVarMapType type, and fill it
 *  <li>Call expander::expandable::expandto
 *  <li>Repeat the previous two steps for every set of variables, you don't need to recreate any objects, just reset them and/or refill them
 * </ol>
 * 
 */

#ifndef EXPANDLIB_EXPANDABLE_H_INCLUDED
#define EXPANDLIB_EXPANDABLE_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <tchar.h>

// This is done because all the commands need to know expandable exists (they all hold a pointer to an expandable class)
namespace expander {
	template <
		class StringType = ::std::string			// Used for the expandable string (and the expanded string)
		> class expandable;
}

#include "expandlib-cmd.h"
#include "expandlib-vartype.h"
#include "expandlib-exceptions.h"

// Commands that will be supported
#include "expandlib-cmd-abbr.h"
#include "expandlib-cmd-bpad.h"
#include "expandlib-cmd-btrunc.h"
#include "expandlib-cmd-fpad.h"
#include "expandlib-cmd-ftrunc.h"
#include "expandlib-cmd-if.h"
#include "expandlib-cmd-knownvars.h"
#include "expandlib-cmd-len.h"
#include "expandlib-cmd-lower.h"
#include "expandlib-cmd-ltrim.h"
#include "expandlib-cmd-numvars.h"
#include "expandlib-cmd-rtrim.h"
#include "expandlib-cmd-trim.h"
#include "expandlib-cmd-unknownvars.h"
#include "expandlib-cmd-upper.h"

// Disables "'this' : used in base member initializer list"
#pragma warning(disable: 4355)

/// The 'expander' namespace

/** \todo Should something be done about that pragma warning(disable: 4355) ? */
/** \todo Use more cpp files (one for every command, containing the code to process the command, for example) But then templates can't be used... */

namespace expander { // *** Start of namespace

	/// The 'expandable' template class, the foundation of it all
	/**	This class holds (among other things) the expandable string and is
		used for calls to expand.
		It also holds a map of commands that can be used with this expandable string.
		The template has one parameter, StringType, normally this would be std::string, but
		it can be anything you want, as long as it is compatible (perhaps std::wstring)

		\todo	Perhaps use a map type that works similar to a rope, with some elements filled in and other
				elements asked for (if it can't find a var it will ask some function/functor for it).
	*/

	template <
		class StringType = ::std::string			// Used for the expandable string (and the expanded string)
		>
	class expandable {
	public:
		typedef ::std::vector<StringType> FParamVectorType; ///< The type used for the parameter vectors (MSVC wouldn't allow me to use template arguments in these typedefs, or I don't know how it should be done)
		typedef ::std::map<StringType, ::expander::expandvartype<StringType>*> FVarMapType; ///< The type used for the map of variables (MSVC wouldn't allow me to use template arguments in these typedefs, or I don't know how it should be done)
		typedef ::std::map<StringType, ::expander::expandcmd<StringType>*> FCmdMapType; ///< The type used for the map of commands (MSVC wouldn't allow me to use template arguments in these typedefs, or I don't know how it should be done)

	protected:
		/// Boolean determining what to do with expand errors
		const bool InlineExpandErrors;

		/// Boolean determining what to do with general errors
		const bool InlineGeneralErrors;

		/// The expandable string
		StringType expstr;

		/// The map of commands
		FCmdMapType cmdmap;

		/**	\name Expand Commands
			\brief A list of all the commands this expandable will support
			This list should be updated every time a command is added.
			The friend declarations are used because all these classes need access to protected
			members of this class (primarily the expand function). The expand function could
			probably be public, but it wasn't meant that way, so for now this should do fine.
		*/
		//@{
		friend class expandcmd<StringType>; ///< This is needed because of the RegisterWith in expandcmd
		friend class expcmd_abbr<StringType>;
		expcmd_abbr<StringType> cmd_abbr;
		friend class expcmd_bpad<StringType>;
		expcmd_bpad<StringType> cmd_bpad;
		friend class expcmd_btrunc<StringType>;
		expcmd_btrunc<StringType> cmd_btrunc;
		friend class expcmd_fpad<StringType>;
		expcmd_fpad<StringType> cmd_fpad;
		friend class expcmd_ftrunc<StringType>;
		expcmd_ftrunc<StringType> cmd_ftrunc;
		friend class expcmd_if<StringType>;
		expcmd_if<StringType> cmd_if;
		friend class expcmd_knownvars<StringType>;
		expcmd_knownvars<StringType> cmd_knownvars;
		friend class expcmd_len<StringType>;
		expcmd_len<StringType> cmd_len;
		friend class expcmd_lower<StringType>;
		expcmd_lower<StringType> cmd_lower;
		friend class expcmd_ltrim<StringType>;
		expcmd_ltrim<StringType> cmd_ltrim;
		friend class expcmd_numvars<StringType>;
		expcmd_numvars<StringType> cmd_numvars;
		friend class expcmd_rtrim<StringType>;
		expcmd_rtrim<StringType> cmd_rtrim;
		friend class expcmd_trim<StringType>;
		expcmd_trim<StringType> cmd_trim;
		friend class expcmd_unknownvars<StringType>;
		expcmd_unknownvars<StringType> cmd_unknownvars;
		friend class expcmd_upper<StringType>;
		expcmd_upper<StringType> cmd_upper;
		//@}

		/// Will expand this expandable into dest, using vars and putting the results in results
		bool expand(StringType &dest, const StringType &src, const FVarMapType &vars, ExpandResultsType &results);

		/// A function to convert from a hexadecimal string to a char (used by expandable::expand)
		inline const StringType::value_type ConvHexToChar(const StringType &h)
		{
			if ( h.size() != 2 ) throw general_error(_T("ConvHexToChar"),_T("Wrong size!"),h.c_str());

			if ( ( h[0] < _T('0') || h[0] > _T('9') ) && ( h[0] < _T('a') || h[0] > _T('f') ) && ( h[0] < _T('A') || h[0] > _T('F') ) ) throw expand_error(_T("ConvHexToChar"),_T("Not a hexadecimal number!"),h.c_str());
			if ( ( h[1] < _T('0') || h[1] > _T('9') ) && ( h[1] < _T('a') || h[1] > _T('f') ) && ( h[1] < _T('A') || h[1] > _T('F') ) ) throw expand_error(_T("ConvHexToChar"),_T("Not a hexadecimal number!"),h.c_str());

			StringType::value_type ch;

			if ( h[0] > _T('F') ) {			// a-f
				ch=(h[0] - _T('a') + 10)*16;
			} else if ( h[0] > _T('9') ) {	// A-F
				ch=(h[0] - _T('A') + 10)*16;
			} else {					// 0-9
				ch=(h[0] - _T('0'))*16;
			}

			if ( h[1] > _T('F') ) {			// a-f
				ch+=h[1] - _T('a') + 10;
			} else if ( h[1] > _T('9') ) {	// A-F
				ch+=h[1] - _T('A') + 10;
			} else {					// 0-9
				ch+=h[1] - _T('0');
			}

			return ch;
		}

	public:
		/// Constructor, initializes expstr to value and the cmdmap with a default set of commands
		expandable(const StringType & value = StringType(_T("")), const bool iee=true, const bool ige=false) : cmdmap(), InlineExpandErrors(iee), InlineGeneralErrors(ige),
			cmd_abbr(this),
			cmd_bpad(this),
			cmd_btrunc(this),
			cmd_fpad(this),
			cmd_ftrunc(this),
			cmd_if(this),
			cmd_knownvars(this),
			cmd_len(this),
			cmd_lower(this),
			cmd_ltrim(this),
			cmd_numvars(this),
			cmd_rtrim(this),
			cmd_trim(this),
			cmd_unknownvars(this),
			cmd_upper(this),
			expstr(value)
		{
			cmd_abbr.Register();
			cmd_bpad.Register();
			cmd_btrunc.Register();
			cmd_fpad.Register();
			cmd_ftrunc.Register();
			cmd_if.Register();
			cmd_knownvars.Register();
			cmd_len.Register();
			cmd_lower.Register();
			cmd_ltrim.Register();
			cmd_numvars.Register();
			cmd_rtrim.Register();
			cmd_trim.Register();
			cmd_unknownvars.Register();
			cmd_upper.Register();
		}

		/// Will call expandable::expand, passing the parameters, as well as expandable::expstr (as source string)
		inline bool expandto(StringType &dest, FVarMapType &vars, ExpandResultsType &results) { return expand(dest, expstr, vars, results); }

		/// This will set the expandable string
		inline void set_expstr(const StringType &value) { expstr=value; }

		/// This will return the expandable string
		inline StringType get_expstr() { return expstr; }
	};

	/**	\name Delimiters
		\brief The different delimiters used by 'expandable::expand'
		\todo These should really be declared in expander::expandable, with type StringType::value_type
	*/
	//@{
	const _TCHAR TwoCharVarDelimiter	= _T('%');		///< The character used for specifying a hex value (it used to be used for short vars)
	const _TCHAR RealVarDelimiter		= _T('$');		///< The character used for specifying a variable, originally these were called 'real' vars
	const _TCHAR CommandDelimiter		= _T('#');		///< The character used for specifying a command
	const _TCHAR VarnameOpenChar		= _T('{');		///< The character used for beginning a var name
	const _TCHAR VarnameCloseChar		= _T('}');		///< The character used for ending a var name
	const _TCHAR ParamsOpenChar			= _T('(');		///< The character used for beginning a parameter list
	const _TCHAR ParamsCloseChar		= _T(')');		///< The character used for ending a parameter list
	const _TCHAR ParamsSeperator		= _T(';');		///< The character used for seperating parameters
	//@}

	/**
		This function iterates through the expandable string (src), using a const_iterator, and uses
		a mode system for keeping track of what to do.

		If it's in the searching mode, it will check wether the current char is one of the delimiter
		chars, if it isn't it will add the current char to the destination string (dest). If it is
		one of the delimiting chars it will go into the mode corresponding to the char, or rather one
		of the 'chk' modes, which will first check wether the input is legal and then decide wether
		to go in this mode or return to the searching mode (and add the delimiter char, as well as the
		chars that were processed by the chk mode).

		The twocharvar mode converts the two hexadecimal digits it has read to a char and adds that
		to dest. The twocharvar mode doesn't check wether the digits were legal, but the expandable::ConvHexToChar
		function does.

		The other "delimiter" modes (command and realvar)work in essentially the same way, only checking for their own delimiters
		and filling other strings. When they encounter parameters, delimited by '(' and ')', they will
		go to the corresponding 'params' mode. The parameters are seperated by comma's, which
		can be escaped by a backslash. The parameters are first written into the 'param' variable,
		and than put into the 'params' vector. If the parameters contain commands or variables with
		parameters those parameters will be "escaped", the variable 'level' is incremented everytime
		a '(' is found inside a parameter and decreased when a ')' is found, if level>0 just about
		anything is simply written into 'param'.
		
		The escaping works by having a variable 'quotemode' that is set to 'backslash' whenever a backslash
		is found (except when level>0). When quotemode==backslash the normal parsing routine isn't
		performed, the current char is outputted directly to the current string (pointed to by curstring).

		\note The expand function APPENDS data to the string dest. This means that you can put anything in front of the string that you would like, but it also means that if you re-use the dest string and don't empty the string each time you use, it will get very large.

		\warning 'expandable::expand' (and any functions called by it) are NOT allowed to delete from rvars (and/or free) elements of rvars they did not create themselves

		\param dest The string that expandable::expand appends the result of the expand operation to.
		\param src The "source" string containing the string to be expanded.
		\param vars The map of variables that is used for this string.
		\param results The results class that is used to keep track of how many variables were processed (and found).

		\todo This could be cleaned-up/re-organized a little more (make the params modes use a switch?).
		\todo Make a for() loop possible: #for(nameofvartouse,fromval,toval[,step],stringtouse)
				Simple example:
					#for(myvar,0,9,${myvar})
				Would output:
					0123456789
		\todo Introduce the [somethingincludingsomevar] way to do: #if(somevar,something,) ; where it should also be possible to use more than one var (they should be ORed).
		\todo Make it possible to do things like #if(somevar || somevar,a,b)
		\todo NOTE: I have already found a way to work around the following 'todo', but it would nevertheless be interesting.<br>Somehow let the commands parse their parameters (to enable better quoting for example, how else would you quote a comparison operator in the first parameter of an if statement?), it might be good idea to somehow make this optional (for speed as well as convenience, at the moment 'if' is the only command that would really benifit from it) 
		\todo Support something like this: #command(param1,(subparam1,subparam2,(subsubparam1)),param3), this would need some sort of tree structure (an STL-like tree container)
	*/
	template <class StringType> bool expandable<StringType>::expand(StringType &dest, const StringType &src, const FVarMapType &vars, ExpandResultsType &results)
	{
		// NOTE: expand (and any functions called by it) are NOT allowed to delete from rvars (and/or free) elements of rvars they did not create themselves

		// *** All kinds of variable declarations
		enum {
			searching,
			twocharvar,
			realvar,
//			command, // Only commandchk is used
			twocharvarchk,
			realvarchk,
			commandchk,
			commandparams,
			realvarparams,
			realvarafterparams
		} mode=searching;				// Used to keep track of the current "mode"
		enum {
			none,
			backslash
		};								// Used for quotemode, I couldn't use a named enum as template argument, so I chose this solution

		int level=0;					// Used for commandparams, to keep track of how deep we are in nested commands (command)
		StringType::const_iterator s_it;// Used for going through the src string

		bool pendingparams;				// Used for variable params, because they can't be escaped immediately

		StringType varname;				// Used for the name of rvars and for hex numbers (twocharvar, realvar)
		StringType cmdname;				// Used to store the name of the command now processing/reading (command)
		StringType param;				// Used to store the value of the param now reading (command)

		FParamVectorType params;		// Used to store read parameters (command)
		//::std::deque<int> quotemode;
		int quotemode=none;				// Used to keep track of the current quote status, originally it was designed to be much more flexible, that is why a deque is (was) used

		StringType * curstring=&dest;	// Used to keep track of the current string to write to, in case something is quoted

		// *** Start of the mainloop (, after some initing)
		//quotemode.push_front(none);

		for(s_it=src.begin(); s_it!=src.end(); ++s_it) {
//			if ( quotemode[0] == backslash ) {
			if ( quotemode == backslash ) {
				// Quoted by a backslash
				//quotemode.pop_front();
				quotemode=none;
				if ( curstring != NULL ) (*curstring)+=*s_it;
			} else if ( *s_it == _T('\\') ) {
				// Encountering a backslash
				//quotemode.push_front(backslash);
				quotemode=backslash;
				if ( curstring != NULL && level>0 ) (*curstring)+=_T('\\'); // This has to do with not removing backslashes from nested #if's (and similar stuff)
			} else {
				// Not quoted
				switch(mode) {


				// Searching mode (inbetween commands, twocharvars and realvars)

				case searching:
					{
						switch(*s_it) {
						case TwoCharVarDelimiter:
							{
								mode=twocharvarchk;
								varname=_T("");
								curstring=&varname;
							}
							break;
						case RealVarDelimiter:
							{
								mode=realvarchk;
								curstring=NULL;
							}
							break;
						case CommandDelimiter:
							{
								mode=commandchk;
								cmdname=_T("");
								curstring=&cmdname;
							}
							break;
						default:
							dest+=*s_it;
						}
					}
					break;


				// Twochar mode

				case twocharvarchk:
					{
						mode=twocharvar;
						varname+=*s_it;
					}
					break;
				case twocharvar:
					{
						varname+=*s_it;
						if ( varname.size() >= 2 ) {
							mode=searching;
							curstring=&dest;

							{
								try {
									dest+=ConvHexToChar(varname);
								} catch(general_error e) {
									if ( InlineGeneralErrors ) {
										dest+=_T('#');
										dest+=e.error_message();
										dest+=_T('#');
									} else {
										throw;
									}
								} catch(expand_error e) {
									if ( InlineExpandErrors ) {
										dest+=_T('#');
										dest+=e.error_message();
										dest+=_T('#');
									} else {
										throw;
									}
								}
							}
						}
					}
					break;


				// Realvar mode

				case realvarchk:
					{
						if ( *s_it == VarnameOpenChar ) {
							mode=realvar;
							varname=_T("");
							curstring=&varname;
						} else {
							if ( *s_it == RealVarDelimiter ) {
								dest+=RealVarDelimiter;
							} else if ( InlineExpandErrors ) {
								mode=searching;
								curstring=&dest;
								dest+=_T("#Syntax error#");
							} else {
								throw expand_error(_T("expandable"),_T("Syntax error!"));
							}
						}
					}
					break;
				case realvar:
					{
						if ( *s_it == VarnameCloseChar ) {
							mode=searching;
							curstring=&dest;

							{
								const FVarMapType::const_iterator it=vars.find(varname);
								if ( it!=vars.end() ) {
									try {
										(*it).second->AppendValue(dest);
									} catch(general_error e) {
										if ( InlineGeneralErrors ) {
											dest+=_T('#');
											dest+=e.error_message();
											dest+=_T('#');
										} else {
											throw;
										}
									} catch(expand_error e) {
										if ( InlineExpandErrors ) {
											dest+=_T('#');
											dest+=e.error_message();
											dest+=_T('#');
										} else {
											throw;
										}
									}
									++results.FoundVars;
								} else {
									++results.MissedVars;
								}
							}
						} else if ( *s_it == ParamsOpenChar ) {
							mode=realvarparams;
							param=_T("");
							params.clear();
							pendingparams=false;
							curstring=&param;
						} else {
							varname+=*s_it;
						}
					}
					break;
				case realvarparams:
					{
						if ( level > 0 ) {
							param+=*s_it;
							if ( *s_it == CommandDelimiter ) {
								++level;
							} else if ( *s_it == RealVarDelimiter ) {
								pendingparams=true;
							} else if ( pendingparams ) {
								if ( *s_it == ParamsOpenChar ) {
									++level;
									pendingparams=false;
								} else if ( *s_it == VarnameCloseChar ) { // Apparantly there were no parameters with this var
									pendingparams=false;
								}
							} else if ( *s_it == ParamsCloseChar ) {
								--level;
							}
						} else if ( *s_it == ParamsCloseChar ) {
							mode=realvarafterparams;
							curstring=NULL;
							params.push_back(param);

							{
								const FVarMapType::const_iterator it=vars.find(varname);
								if ( it!=vars.end() ) {
									try {
										(*it).second->AppendValue(dest, params, vars, results);
									} catch(general_error e) {
										if ( InlineGeneralErrors ) {
											dest+='#';
											dest+=e.error_message();
											dest+='#';
										} else {
											throw;
										}
									} catch(expand_error e) {
										if ( InlineExpandErrors ) {
											dest+='#';
											dest+=e.error_message();
											dest+='#';
										} else {
											throw;
										}
									}
									++results.FoundVars;
								} else {
									++results.MissedVars;
								}
							}
						} else if ( *s_it == ParamsSeperator ) {
							params.push_back(param);
							param=_T("");
						} else if ( *s_it == CommandDelimiter ) {
							++level;
							param+=*s_it;
						} else if ( *s_it == RealVarDelimiter ) {
							pendingparams=true;
							param+=*s_it;
						} else if ( pendingparams ) {
							param+=*s_it;
							if ( *s_it == ParamsOpenChar ) {
								++level;
								pendingparams=false;
							} else if ( *s_it == VarnameCloseChar ) { // Apparantly there were no parameters with this var
								pendingparams=false;
							}
						} else {
							param+=*s_it;
						}
					}
					break;
				case realvarafterparams:
					{
						if ( *s_it == VarnameCloseChar ) {
							mode=searching;
							curstring=&dest;
						}
					}
					break;


				// Command mode

				case commandchk:
					{
						if ( *s_it == ParamsOpenChar ) {
							mode=commandparams;
							param=_T("");
							params.clear();
							pendingparams=false;
							curstring=&param;
						} else {
							cmdname+=*s_it;
						}
					}
					break;
/*				case command:
					{
						if ( *s_it == ParamsOpenChar ) {
							mode=commandparams;
							param="";
							params.clear();
							pendingparams=false;
							curstring=&param;
						} else {
							cmdname+=*s_it;
						}
					}
					break;*/
				case commandparams:
					{
						if ( level > 0 ) {
							param+=*s_it;
							if ( *s_it == CommandDelimiter ) {
								++level;
							} else if ( *s_it == RealVarDelimiter ) {
								pendingparams=true;
							} else if ( pendingparams ) {
								if ( *s_it == ParamsOpenChar ) {
									++level;
									pendingparams=false;
								} else if ( *s_it == VarnameCloseChar ) { // Apparantly there were no parameters with this var
									pendingparams=false;
								}
							} else if ( *s_it == ParamsCloseChar ) {
								--level;
							}
						} else if ( *s_it == ParamsCloseChar ) {
							mode=searching;
							curstring=&dest;
							params.push_back(param);

							{
								const FCmdMapType::const_iterator it=cmdmap.find(cmdname);
								if ( it != cmdmap.end() ) {
									try {
										(*((*it).second))(dest, params, vars, results);
									} catch(general_error e) {
										if ( InlineGeneralErrors ) {
											dest+='#';
											dest+=e.error_message();
											dest+='#';
										} else {
											throw;
										}
									} catch(expand_error e) {
										if ( InlineExpandErrors ) {
											dest+='#';
											dest+=e.error_message();
											dest+='#';
										} else {
											throw;
										}
									}
								}
							}
						} else if ( *s_it == ParamsSeperator ) {
							params.push_back(param);
							param=_T("");
						} else if ( *s_it == CommandDelimiter ) {
							++level;	// Should also be pendingparams=true, but it doesn't need to be, so why bother
							param+=*s_it;
						} else if ( *s_it == RealVarDelimiter ) {
							pendingparams=true;
							param+=*s_it;
						} else if ( pendingparams ) {
							param+=*s_it;
							if ( *s_it == ParamsOpenChar ) {
								++level;
								pendingparams=false;
							} else if ( *s_it == VarnameCloseChar ) { // Apparantly there were no parameters with this var
								pendingparams=false;
							}
						} else {
							param+=*s_it;
						}
					}
					break;
				} // End of switch(mode)
			} // End of else (from the quotemode if)
		} // End of main for loop

		if ( mode != searching ) {
			if ( InlineExpandErrors ) {
				dest+=_T("#Unexpected end of string#");
			} else {
				throw expand_error(_T("expandable"),_T("Unexpected end of string!"));
			}
		}

		return true;
	}
} // *** End of namespace

#endif //EXPANDLIB_EXPANDABLE_H_INCLUDED
