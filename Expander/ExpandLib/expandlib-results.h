
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

/** \file expandlib-results.h
	\brief Headerfile for the expandresults class, used to keep track of the expand operation (how many vars were expanded, etc.)
*/

#ifndef EXPANDLIB_RESULTS_H_INCLUDED
#define EXPANDLIB_RESULTS_H_INCLUDED

namespace expander { // *** Start of namespace

	/// Holds the results of the expand action.
	/**	This class is used for counting how many vars were expanded/not found while trying to
		expand the string.

		\todo Add support for more statistics? (like how many commands)
	*/

	class expandresults {
	private:
	public:
		/// The default constructor.
		expandresults() {};

		/// Resets all the vars to their default values (same values as with the default constructor).
		inline void Reset() { MissedVars=0; FoundVars=0; };

		/// Returns the total number of vars processed (missed+found).
		inline int TotalVars() const { return MissedVars.val + FoundVars.val; };

		/// A local class for foundvars and missedvars
		class FMVarsType {
		private:
			/// The value
			int val;

		public:
			/// The default constructor
			FMVarsType() { val=0; };

			/// Operator '()' to return the value
			inline int operator() () const { return val; };

			/// Postfix ++ operator (doesn't return anything)
			inline void operator++(int) { ++val; };
			/// Prefix ++ operator (doesn't return anything)
			inline void operator++() { ++val; };

			/// Assignment operator (for an int)
			inline void operator=(int v) { val=v; };

			/// Makes it easy for expandresults::TotalVars()
			friend expandresults;

		} FoundVars, MissedVars;

	};

	/**	\brief A typedef to provide a slight amount of abstraction.
		This typedef really shouldn't be here, but I made it because the old code used this name
		and it provided me with a very easy way of abstracting this (I wasn't sure yet on how to
		do this).
	*/
	typedef expandresults ExpandResultsType;

} // *** End of namespace

#endif //EXPANDLIB_RESULTS_H_INCLUDED
