
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

/** \file expandlib-vartype-time.h
	\brief Headerfile for the 'expvartype_time' vartype (a time)
*/

#ifndef EXPANDLIB_VARTYPE_TIME_H_INCLUDED
#define EXPANDLIB_VARTYPE_TIME_H_INCLUDED

#include <boost/lexical_cast.hpp>

#include "expandlib-vartype.h"

namespace expander { // *** Start of namespace

	/// The class that supplies the time vartype.
	/**	This class can be used in the program using ExpandLib for time variables.

		\todo Make it possible to define the way the time is formatted, using parameters
	*/

	template<
		// Used for the expandable string (and the expanded string)
		class StringType = ::std::string,
		long NotimeMSValue = -1
		>
	class expvartype_time : public expandvartype<StringType> {
	protected:

		/// Used to store the time
		class TimeType {
		public:
			TimeType(int H=0, int M=0, int S=0, int MS=0) { h=H; m=M; s=S; ms=MS; }

			int h; ///< Hours
			int m; ///< Minutes
			int s; ///< Seconds
			int ms; ///< Milliseconds
		} Time;

		/// Used to specify NO TIME
		bool notime;

		/// Returns s, padded with zeroes from the left
		inline StringType PadWithZeroes(const StringType &s, const unsigned int num) const {
			const StringType::value_type padchar('0');
			StringType d;

			if ( s.size() < num ) {
				for(unsigned int u=0; u<(num-s.size()); ++u) d+=padchar;
				d+=s;
			} else {
				d=s;
			}

			return d;
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
//		expvartype_time() {}

		/// Constructor with a parameter of type bool, inits the value to H:M:S.MS
		expvartype_time(int H=0, int M=0, int S=0, int MS=0, bool nt=false) : Time(H,M,S,MS) { notime=nt; }

		/// Constructor with a C-style string as parameter, inits the value to 'cs' (converted)
		expvartype_time(const _TCHAR * cs) {
			const StringType s(cs);
			SetValue(s);
		}

		/// Constructor with a string as parameter, inits the value to 's' (converted)
		expvartype_time(const StringType s) {
			SetValue(s);
		}

		virtual void AppendValue(StringType &dest) const {
			if ( notime ) {
				dest+=_T('-');
				return;
			}
			try {
				PadWithZeroesTo(dest, boost::lexical_cast<StringType>(Time.h),2);
			} catch(...) {
				dest+=_T("00");
			}
			dest+=_T(':');
			try {
				PadWithZeroesTo(dest, boost::lexical_cast<StringType>(Time.m),2);
			} catch(...) {
				dest+=_T("00");
			}
			dest+=_T(':');
			try {
				PadWithZeroesTo(dest, boost::lexical_cast<StringType>(Time.s),2);
			} catch(...) {
				dest+=_T("00");
			}
			if ( Time.ms >= 100 ) {
				dest+=_T('.');
				try {
					if ( Time.ms % 100 == 0 ) {
						dest+=boost::lexical_cast<StringType>(Time.ms/100);
					} else if ( Time.ms % 10 == 0 ) {
						dest+=boost::lexical_cast<StringType>(Time.ms/10);
					} else {
						dest+=boost::lexical_cast<StringType>(Time.ms);
					}
				} catch(...) {
					dest+=_T('0');
				}
			} else if ( Time.ms >= 10 ) {
				dest+=_T('.');
				try {
					if ( Time.ms % 10 == 0 ) {
						dest+=PadWithZeroes(boost::lexical_cast<StringType>(Time.ms/10),2);
					} else {
						dest+=PadWithZeroes(boost::lexical_cast<StringType>(Time.ms),3);
					}
				} catch(...) {
					dest+=_T('0');
				}
			} else if ( Time.ms >= 1 ) {
				dest+=_T('.');
				try {
					PadWithZeroesTo(dest, boost::lexical_cast<StringType>(Time.ms),3);
				} catch(...) {
					dest+=_T('0');
				}
			}
		}

		virtual void AppendValue(StringType &dest, const FParamVectorType &params, const FVarMapType &vars, ExpandResultsType &results) const {
			AppendValue(dest);
		}

		virtual void SetValue(const StringType &s) {
			if ( s == _T("-") ) {
				Time.h=0;
				Time.m=0;
				Time.s=0;
				Time.ms=0;
				notime=true;
				return;
			} else {
				notime=false;
			}
			std::vector<int> vals;
			bool dot=false;
			{
				StringType temp;
				for(StringType::const_iterator it(s.begin()); it!=s.end(); ++it) {
					if ( dot ) {
						if ( *it >= _T('0') && *it <= _T('9') ) {
							temp+=*it;
						} else {
							break;
						}
					} else {
						if ( *it >= _T('0') && *it <= _T('9') ) {
							temp+=*it;
						} else if ( *it == _T(':') || *it == _T('.') ) {
							try {
								vals.push_back(boost::lexical_cast<int>(temp));
							} catch(boost::bad_lexical_cast) {
								vals.push_back(0);
							}
							temp=_T("");
							if ( *it == _T('.') ) dot=true;
						} else {
							break;
						}
					}
				}
				if ( dot ) {
					if ( temp.size() > 3 ) {
						temp.resize(3);
					} else {
						temp.append(3-temp.size(), _T('0'));
					}
					try {
						vals.push_back(boost::lexical_cast<int>(temp));
					} catch (boost::bad_lexical_cast) {
						vals.push_back(0);
					}
				} else {
					try {
						vals.push_back(boost::lexical_cast<int>(temp));
					} catch (boost::bad_lexical_cast) {
						vals.push_back(0);
					}
				}
			}

			if ( vals.size() == (unsigned int)3 + (dot?1:0) ) {
					Time.h=vals[0];
					Time.m=vals[1];
					Time.s=vals[2];
					if ( dot ) Time.ms=vals[3]; else Time.ms=0;
			} else if ( vals.size() == (unsigned int)2 + (dot?1:0) ) {
					Time.h=0;
					Time.m=vals[0];
					Time.s=vals[1];
					if ( dot ) Time.ms=vals[2]; else Time.ms=0;
			} else if ( vals.size() == (unsigned int)1 + (dot?1:0) ) {
					Time.h=0;
					Time.m=0;
					Time.s=vals[0];
					if ( dot ) Time.ms=vals[1]; else Time.ms=0;
			} else {
					Time.h=0;
					Time.m=0;
					Time.s=0;
					Time.ms=0;
			}
			if ( Time.h == 24 ) Time.h=0;
			if ( Time.h > 23 ) Time.h=23;
			if ( Time.h < 0 ) Time.h=0;
			if ( Time.m > 59 ) Time.m=59;
			if ( Time.m < 0 ) Time.m=0;
			if ( Time.s > 59 ) Time.s=59;
			if ( Time.s < 0 ) Time.s=0;
			if ( Time.ms > 999 ) Time.ms=999;
			if ( Time.ms < 0 ) Time.ms=0;
		}

		/// Conversion to long (ms)
		inline long get_milliseconds() {
      return (notime?NotimeMSValue:(Time.h*3600000)+(Time.m*60000)+(Time.s*1000)+Time.ms);
    }

		/// Conversion to string
		inline StringType get_string() {
			StringType s;
			AppendValue(s);
			return s;
		}
	};

} // *** End of namespace

#endif //EXPANDLIB_VARTYPE_TIME_H_INCLUDED
