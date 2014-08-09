
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

/** \file
	\brief Headerfile for the 'expvartype_numeric' traits
*/

#ifndef EXPANDLIB_VARTYPE_NUMERIC_TRAITS_H_INCLUDED
#define EXPANDLIB_VARTYPE_NUMERIC_TRAITS_H_INCLUDED

namespace expander { // *** Start of namespace

	/// A simple traits class
	template<class NumType>
	class expvartype_numeric_traits_normal {
	public:
		static inline NumType defaultvalue() { return 0; }
		static inline NumType validate(const NumType v) { return v; }
	};

	/// A traits class that limits the possible values
	template<class NumType, const NumType minimum, const NumType maximum>
	class expvartype_numeric_traits_limited {
	public:
		static inline NumType defaultvalue() { return validate(0); }
		static inline NumType validate(const NumType v) { return ( v > maximum ? maximum : ( v < minimum ? minimum : v ) ); }
	};

	/// A traits class that limits the possible values
	template<class NumType, const NumType minimum, const NumType maximum, const NumType defaultval>
	class expvartype_numeric_traits_limited_with_default {
	public:
		static inline NumType defaultvalue() { return defaultval; }
		static inline NumType validate(const NumType v) { return ( v > maximum || v < minimum ? defaultval : v ); }
	};

} // *** End of namespace

#endif //EXPANDLIB_VARTYPE_NUMERIC_TRAITS_H_INCLUDED
