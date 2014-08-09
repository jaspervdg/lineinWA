
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

/** \file expandlib-cmd-if.h
	\brief Headerfile for the 'if' command
*/

#ifndef EXPANDLIB_CMD_IF_H_INCLUDED
#define EXPANDLIB_CMD_IF_H_INCLUDED

#include <locale>

#include <boost/lexical_cast.hpp>

#include "expandlib-cmd.h"

namespace expander { // *** Start of namespace

	/// The class implementing the 'if' command
	/** Syntax:<br>
		\#if(expandablestring,truepart,falsepart)<br>
			Checks wether all the vars in expandablestring exist<br>
		\#if(operand1,operator,operand2,truepart,falsepart)<br>
			Performs the requested comparison, for example: #if(2,>!,3,Two is greater than three.,Two is smaller than\\, or equal to\\, three.) returns: Two is smaller than, or equal to, three.
	*/

	template<
		// Used for the expandable string (and the expanded string)
		class StringType = ::std::string
		>
	class expcmd_if : public expandcmd<StringType> {
	protected:

		/// Determines wether ch is a valid first character for an operand.
		static inline bool IsValidOpStartChar(const StringType::value_type ch) throw()
		{
			return ( ch == _T('<') || ch == _T('>') || ch == _T('=') || ch == _T('!') );
		};

		/// Determines wether ch is a valid operand character.
		static inline bool IsValidOpChar(const StringType::value_type ch) throw()
		{
			return ( ch == _T('<') || ch == _T('>') || ch == _T('=') || ch == _T('!') || ch == _T('@') );
		};

		static inline bool logical_xor(const bool x, const bool y)
		{
			return !(x && y) && (x || y);
		}

		/// Used for textual comparison
		const std::locale loc;
		/// Used for textual comparison
//		const std::collate<StringType::value_type> &col;

	public:
		/// The constructor
		/** This constructor gets a pointer to an expander::expandable object and passes that,
			along with the name of the command, to the expander::expandcmd constructor.
		*/
		expcmd_if(expandable<StringType> * const eobj) : expandcmd<StringType>(eobj, _T("if")), loc("") {};

		/** This performs the if command, it does this by splitting its first parameter into
			operand1, operator and operand2. It simply puts anything it finds in operand1, until it
			encounters an operator start character (see expcmd_if::IsValidOpStartChar). Then it puts
			everything into the operator, with a maximum of three chars and it stops before that if it
			encounters a non-operator char (see expcmd_if::IsValidChar). After that everything goes
			into operand2.

			I am know using an alternative to the "let the command parse the parameters" strategy to do less extra work, I simply use more than one syntax: \#if(operand1,operator,operand2,truepart,falsepart) and \#if(expstrtocheck,truepart,falsepart), and perhaps even \#if(operand1-1,operator1,operand1-2,booloperator1,operand2-1,operator2,operand2-2,truepart,falsepart), but that would be a lot nicer if some kind of tree structure could be used.

			\todo Make textual comparison work (using locale's/facets?).
		*/
		virtual void operator()(StringType &dest, const FParamVectorType &params, const FVarMapType &vars, ExpandResultsType &results) const
		{
			if ( params.size() == 3 ) {
				ExpandResultsType result;	// Used to verify that all vars were found

				{
					StringType d;
					expobj->expand(d, params[0], vars, result);	// Expand operand 1
				}

				{
					//StringType d;
					if ( result.MissedVars() == 0 ) {
						/*if ( */expobj->expand(dest, params[1], vars, results);// ) dest+=d;
					} else {
						/*if ( */expobj->expand(dest, params[2], vars, results);// ) dest+=d;
					}
				}
			} else if ( params.size() == 5 ) {
				if ( params[1].size() > 3 ) {
					throw expand_error(_T("expcmd_if"),_T("Illegal operator"),params[1].c_str());
				}

				StringType operand1, operand2;	// The two operands for the comparison
				struct {
					bool equalto;
					bool greaterthan;
					bool lessthan;
					bool not;
				} comp;			// Keeps track of the comparisons needed
				enum {
					nocomp,
					binary,
					textual,
					numeric
				} comptype;		// Keeps track of the comparison type
				ExpandResultsType resultop1, resultop2;	// Used to verify that all vars were found

				expobj->expand(operand1, params[0], vars, resultop1);	// Expand operand 1
				expobj->expand(operand2, params[2], vars, resultop2);	// Expand operand 2

				// Collect info about the comparison
				comp.equalto=false;
				comp.greaterthan=false;
				comp.lessthan=false;
				comp.not=false;
				comptype=nocomp;

				{
					StringType op(params[1]);
					if ( op.size() >= 1 ) {
						comptype=binary;
						if ( op.size() >= 2 ) {
							if ( op[op.size()-1] == _T('@') ) {
								comptype=textual;
								op.erase(op.end() - 1);
							} else if ( op[op.size()-1] == _T('!') ) {
								comptype=numeric;
								op.erase(op.end() - 1);
							} else if ( op.size() == 3 && op[op.size()-1] == _T('=') ) {
								op.erase(op.end() - 1);
							}
						}

						if ( op == _T("==") ) {
							comp.equalto=true;
						} else if ( op == _T(">=") ) {
							comp.equalto=true;
							comp.greaterthan=true;
						} else if ( op == _T("<=") ) {
							comp.equalto=true;
							comp.lessthan=true;
						} else if ( op == _T("!=") ) {
							comp.equalto=true;
							comp.not=true;
						} else if ( op == _T(">") ) {
							comp.greaterthan=true;
						} else if ( op == _T("<") ) {
							comp.lessthan=true;
						} else {
							comptype=nocomp;
						}
					}
				}

				{
					//StringType d;
					bool correct=false;	// correct keeps track of wether or not the comparison is true
										//  It does that by initializing to false and then ORing
										//  with true for each comparison that is true. At the end
										//  it checks wether it needs to NOT the comparison.
										//  The comparison could be done slightly more compact by
										//  either "jumping" as soon as something was true, or by
										//  doing something like: correct=correct || ( operand1 == operand2 )
					switch(comptype) {
					case binary:
						{	// This performs a "binary" comparison (or should perform one at least)
							const int temp_val=operand1.compare(operand2);

							if ( comp.equalto ) {
								if ( temp_val == 0 ) correct = !comp.not;
							}
							if ( comp.greaterthan ) {
								if ( temp_val > 0 ) correct = true;
							}
							if ( comp.lessthan ) {
								if ( temp_val < 0 ) correct = true;
							}

							if ( correct ) {
								/*if ( */expobj->expand(dest, params[3], vars, results);// ) dest+=d;
							} else {
								/*if ( */expobj->expand(dest, params[4], vars, results);// ) dest+=d;
							}
						}
						break;
					case textual:
						{	// This should perform a "textual" comparison (more or less what a dictionary uses)
							using namespace std;
							use_facet< ctype<StringType::value_type> >(loc).toupper(operand1.begin(), operand1.end());
							use_facet< ctype<StringType::value_type> >(loc).toupper(operand2.begin(), operand2.end());

							const int temp_val=operand1.compare(operand2);

							if ( comp.equalto ) {
								if ( temp_val == 0 ) correct = !comp.not;
							} 
							if ( comp.greaterthan ) {
								if ( temp_val > 0 ) correct = true;
							}
							if ( comp.lessthan ) {
								if ( temp_val < 0 ) correct = true;
							}

							if ( correct ) {
								/*if ( */expobj->expand(dest, params[3], vars, results);// ) dest+=d;
							} else {
								/*if ( */expobj->expand(dest, params[4], vars, results);// ) dest+=d;
							}
						}
						break;
					case numeric:
						{	// This performs a numeric conversion (10 is bigger than 8)
							try {
								const int num1=boost::lexical_cast<int>(operand1);//*/atoi(operand1.c_str());
								const int num2=boost::lexical_cast<int>(operand2);//*/atoi(operand2.c_str());

								if ( comp.equalto ) {
									if ( num1 == num2 ) correct= !comp.not;
								}
								if ( comp.greaterthan ) {
									if ( num1 > num2 ) correct= true;
								}
								if ( comp.lessthan ) {
									if ( num1 < num2 ) correct= true;
								}

								if ( correct ) {
									/*if ( */expobj->expand(dest, params[3], vars, results);// ) dest+=d;
								} else {
									/*if ( */expobj->expand(dest, params[4], vars, results);// ) dest+=d;
								}
							} catch(boost::bad_lexical_cast) {
								dest+=_T("#Illegal operand(s)#");
								return;
							}
						}
						break;
					} // End of switch(comptype)
				}
			} else {
				throw expand_error(_T("expcmd_if"),_T("Parameter error"));
			}
		}
	};

} // *** End of namespace

#endif //EXPANDLIB_CMD_IF_H_INCLUDED
