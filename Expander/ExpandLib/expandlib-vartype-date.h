
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

/** \file expandlib-vartype-date.h
	\brief Headerfile for the 'expvartype_date' vartype (a date)
*/

#ifndef EXPANDLIB_VARTYPE_DATE_H_INCLUDED
#define EXPANDLIB_VARTYPE_DATE_H_INCLUDED

#include <boost/lexical_cast.hpp>

#include "expandlib-vartype.h"

namespace expander { // *** Start of namespace

	/// The class that supplies the date vartype.
	/**	This class can be used in the program using ExpandLib for date variables.

		\todo Make it possible to define the way the date is formatted, using parameters
	*/

	template<
		// Used for the expandable string (and the expanded string)
		class StringType = ::std::string
		>
	class expvartype_date : public expandvartype<StringType> {
	protected:

		/// Used to hold the date
		class DateType {
		public:
			DateType(int y=0, int m=0, int d=0) { year=y; month=m; day=d; }

			int year;
			int month;
			int day;
		} Date;

		/// Used to specify NO DATE
		bool nodate;

		/// Returns s, padded with zeroes from the left
		inline StringType PadWithZeroes(const StringType &s, const unsigned int num) const {
			const StringType::value_type padchar(_T('0'));

			if ( s.size() < num ) {
				StringType d(num-s.size(), padchar);
				//for(unsigned int u=0; u<(num-s.size()); ++u) d+=padchar;
				d+=s;
				return d;
			} else {
				return s;
			}
		}

		/// Pads s from the left (with zeroes), result goes into d
		inline void PadWithZeroesTo(StringType &d, const StringType &s, const unsigned int num) const {
			const StringType::value_type padchar(_T('0'));

			if ( s.size() < num ) {
				d.append(num-s.size(), padchar);
				d+=s;
			} else {
				d+=s;
			}
		}

	public:

//		/// Default contructor, inits to 0:0:0.0
//		expvartype_date() {}

		/// Constructor with a parameter of type bool, inits the value to H:M:S.MS
		expvartype_date(int y=0, int m=0, int d=0) : Time(y,m,d) {}

		/// Constructor with a C-style string as parameter, inits the value to 'cs' (converted)
		expvartype_date(const _TCHAR * cs) {
			const StringType s(cs);
			SetValue(s);
		}

		/// Constructor with a string as parameter, inits the value to 's' (converted)
		expvartype_date(const StringType s) {
			SetValue(s);
		}

		virtual void AppendValue(StringType &dest) const {
			if ( nodate ) {
				dest+=_T('-');
				return;
			}
			try {
				dest+=boost::lexical_cast<StringType>(Date.year);
			} catch(...) {
				dest+=_T("0000");
			}
			dest+=_T('-');
			try {
				PadWithZeroesTo(dest, boost::lexical_cast<StringType>(Date.month),2);
			} catch(...) {
				dest+=_T("00");
			}
			dest+=_T('-');
			try {
				PadWithZeroesTo(dest, boost::lexical_cast<StringType>(Date.day),2);
			} catch(...) {
				dest+=_T("00");
			}
		}

		virtual void AppendValue(StringType &dest, const FParamVectorType &params, const FVarMapType &vars, ExpandResultsType &results) const {
			AppendValue(dest);
		}

		virtual void SetValue(const StringType &s) {
			if ( s == _T("-") ) {
				Date.year=0;
				Date.month=0;
				Date.day=0;
				nodate=true;
				return;
			} else {
				nodate=false;
			}
			std::vector<int> vals;
			{
				StringType temp;
				for(StringType::const_iterator it=s.begin(); it!=s.end(); ++it) {
					if ( *it >= _T('0') && *it <= _T('9') ) {
						temp+=*it;
					} else if ( *it == _T('-') ) {
						try {
							vals.push_back(boost::lexical_cast<int>(temp));
						} catch (boost::bad_lexical_cast) {
							vals.push_back(0);
						}
						temp=_T("");
					} else {
						break;
					}
				}
				try {
					vals.push_back(boost::lexical_cast<int>(temp));
				} catch (boost::bad_lexical_cast) {
					vals.push_back(0);
				}
			}

			if ( vals.size() == 3 ) {
				Date.year=vals[0];
				Date.month=vals[1];
				Date.day=vals[2];
				if ( Date.month > 12 ) Date.month=12;
				if ( Date.month < 1 ) Date.month=1;
				if ( Date.day > 31 ) Date.day=31;
				if ( Date.day < 1 ) Date.day=1;
			}
		}

		/// Returns date as a string
		inline StringType get_string() {
			StringType s;
			AppendValue(s);
			return s;
		}
	};

} // *** End of namespace

#endif //EXPANDLIB_VARTYPE_DATE_H_INCLUDED
