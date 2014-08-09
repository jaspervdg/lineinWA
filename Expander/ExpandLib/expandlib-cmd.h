
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

/** \file expandlib-cmd.h
	\brief Headerfile for the (abstract) command base class
*/

#ifndef EXPANDLIB_CMD_H_INCLUDED
#define EXPANDLIB_CMD_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <utility>

#include <tchar.h>

#include "expandlib-expandable.h"

#include "expandlib-results.h"
#include "expandlib-vartype.h"
#include "expandlib-exceptions.h"

namespace expander { // *** Start of namespace

	/// An abstract base class for the commands
	/**	This class is the class that all commands derive from. It's an abstract class
		because a NOP command doesn't make any sense and should therefor not be needed.
		This could change in the future, but for now this works perfectly.

		It also holds a few typedefs that the commands inherit.
	*/

	template<
		// Used for the expandable string (and the expanded string)
		class StringType = ::std::string
		>
	class expandcmd {
	private:
		/// Private constructor to prevent incorrect construction
		expandcmd() {};

	protected:
		typedef ::std::vector<StringType> FParamVectorType; ///< The type used for the parameter vectors (MSVC wouldn't allow me to use template arguments in these typedefs, or I don't know how it should be done)
		typedef ::std::map<StringType, ::expander::expandvartype<StringType>*> FVarMapType; ///< The type used for the map of variables (MSVC wouldn't allow me to use template arguments in these typedefs, or I don't know how it should be done)
		typedef ::std::map<StringType, ::expander::expandcmd<StringType>*> FCmdMapType; ///< The type used for the map of commands (MSVC wouldn't allow me to use template arguments in these typedefs, or I don't know how it should be done)

		/// A const pointer to the expander::expandable object this command belongs to.
		expandable<StringType> * const expobj;
		/// A const string containing the name of this command.
		/** This string is used by expandcmd::Register and initialized by the constructor, which is called
			by the constructor of a derived class (which passes its name to this class, so expandcmd::cmdname
			contains the right name.
		*/
		const StringType cmdname;

	public:
		/// The only constructor allowable.
		/** It stores eobj and sets cmdname to cn.
		*/
		expandcmd(expandable<StringType> * const eobj, const StringType cn) : expobj(eobj), cmdname(cn) {};

		/// Appends the outcome of the command to 'dest'.
		/** The () operator of a command will process the params (using the
			 vars and keeping statistics in results) and APPEND the outcome to dest.
		*/
		virtual void operator()(StringType &dest, const FParamVectorType &params, const FVarMapType &vars, ExpandResultsType &results) const = 0;

		/// Registers this command with 'expobj'.
		/** Register adds itself to the expandable::cmdmap in expobj.
			But since there are no expandcmd instances (it's an abstract class), this
			will only be done by derived classes (which fill cmdname and expobj through the constructor).
		*/
		void Register() { expobj->cmdmap[cmdname] = this; };
	};

} // *** End of namespace

#endif //EXPANDLIB_CMD_H_INCLUDED
