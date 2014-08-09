
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

/** \file expandlib-vartype-list.h
	\brief Headerfile for the 'expvartype_list' vartype (a list comprised of other expvartype's)
*/

#ifndef EXPANDLIB_VARTYPE_LIST_H_INCLUDED
#define EXPANDLIB_VARTYPE_LIST_H_INCLUDED

#include <boost/lexical_cast.hpp>

#include <stdarg.h>
#include <tchar.h>

#include "expandlib-vartype.h"

namespace expander { // *** Start of namespace

	/// The class that supplies the list vartype.
	/**	This class can be used in the program using ExpandLib for lists of variables of a specific type.

		Normally this will return a list of the elements separated by a comma and a space. It can
		however also receive parameters, the syntax for this is:
			${listvar([(index|separator|'|')[,extraparams]])}
		If a number is given as the first parameter it is interpreted as an index into the list,
		it this index is -1 it will return the last item in the list. extraparams is passed on to
		each item in the list.
		If a | (pipe) is given as the first parameter extraparams will just be passed on to each
		item in the list.
		If neither a pipe nor a number is given as the first parameter it is used as separator.
		extraparams is passed on to each item in the list.

		Since this class is also derived from std::vector<ItemType> any of its member functions
		can also be used on this type.
	*/

	template<
		// Used for the expandable string (and the expanded string)
		class StringType = ::std::string,
		class ItemType = expvartype_text< StringType >
		>
	class expvartype_list : public expandvartype<StringType> {
	protected:
		::std::vector<ItemType> list;
		const _TCHAR* const ItemSeparator;

	public:

		/// Default contructor, inits the list to nothing
		expvartype_list(const _TCHAR *const is=_T(", ")) : ItemSeparator(is) {}

		/// Constructor with a parameter of type StringType, inits the string to 's'
		expvartype_list(const StringType &s, const _TCHAR *const is=_T(", ")) : ItemSeparator(is) { SetValue(s); }

		/// Constructor with a C-style string as parameter, inits the string to 's'
		expvartype_list(const _TCHAR * s, const _TCHAR *const is=_T(", ")) : ItemSeparator(is) { SetValue(s); }

		/// Constructor using a begin and end iterator
		/** This constructor lets you initialize the list with another list (or in general anything
			that can be behave as an iterator).
		*/
		template<class T> expvartype_list(T beg, const T end, const _TCHAR *const is=_T(", ")) : ItemSeparator(is)
		{
			for(; beg != end; ++beg) {
				list.push_back(ItemType(*beg));
			}
		}

		/// Constructor using a count and a value (generates n elements with value v)
		template<class T> expvartype_list(const ::std::vector<ItemType>::size_type n, const T val, const _TCHAR *const is=_T(", ")) : list(n, ItemType(val)), ItemSeparator(is) {}

		virtual void AppendValue(StringType &dest) const throw()
		{
			{
				for(::std::vector<ItemType>::const_iterator it=list.begin(); it!=list.end(); ++it) {
					if ( it != list.begin() ) dest += ItemSeparator;
					(*it).AppendValue(dest);
				}
			}
		}

		virtual void AppendValue(StringType &dest, const FParamVectorType &params, const FVarMapType &vars, ExpandResultsType &results) const throw()
		{
			if ( params.size() == 0 ) {
				for(::std::vector<ItemType>::const_iterator it=list.begin(); it!=list.end(); ++it) {
					if ( it != list.begin() ) dest += ItemSeparator;
					(*it).AppendValue(dest, params, vars, results);
				}
			} else if ( params[0] == _T("|") ) {
				const FParamVectorType tparams(params.begin()+1, params.end());
				{
					for(::std::vector<ItemType>::const_iterator it=list.begin(); it!=list.end(); ++it) {
						if ( it != list.begin() ) dest += ItemSeparator;
						(*it).AppendValue(dest, tparams, vars, results);
					}
				}
			} else if ( params.size() == 1 ) {
				try {
					list.at(boost::lexical_cast<::std::vector<ItemType>::size_type>(params[0])).AppendValue(dest);
				} catch(boost::bad_lexical_cast) {
					if ( params[0] == _T("-1") ) {
						list.back().AppendValue(dest);
					} else if ( params[0] == _T("size") ) {
						try {
							dest += boost::lexical_cast<StringType>(list.size());
						} catch(...) {
							throw general_error("expvartype_list","Error during lexical_cast<StringType>(list.size()).");
						}
					} else {
						for(::std::vector<ItemType>::const_iterator it=list.begin(); it!=list.end(); ++it) {
							if ( it != list.begin() ) dest += params[0];
							(*it).AppendValue(dest);
						}
					}
				} catch(...) {
					if ( params[0] == _T("-1") ) {
						list.back().AppendValue(dest);
					} else {
						throw general_error(_T("expvartype_list"),_T("Error during at(boost::lexical_cast<size_type>(params[0])).AppendValue(dest)."));
					}
				}
			} else { // First parameter is either index or separator, from the second the parameters are passed on
				const FParamVectorType tparams(params.begin()+1, params.end());
				try {
					list.at(boost::lexical_cast<::std::vector<ItemType>::size_type>(params[0])).AppendValue(dest, tparams, vars, results);
				} catch(boost::bad_lexical_cast) {
					if ( params[0] == _T("-1") ) {
						list.back().AppendValue(dest, tparams, vars, results);
					} else if ( params[0] == _T("size") ) {
						try {
							dest += boost::lexical_cast<StringType>(list.size());
						} catch(...) {
							throw general_error(_T("expvartype_list"),_T("Error during lexical_cast<StringType>(list.size())."));
						}
					} else {
						for(::std::vector<ItemType>::const_iterator it=list.begin(); it!=list.end(); ++it) {
							if ( it != list.begin() ) dest += params[0];
							(*it).AppendValue(dest, tparams, vars, results);
						}
					}
				} catch(...) {
					if ( params[0] == _T("-1") ) {
						list.back().AppendValue(dest);
					} else {
						throw general_error(_T("expvartype_list"),_T("Error during at(boost::lexical_cast<size_type>(params[0])).AppendValue(dest)."));
					}
				}
			}
		}

		/// Sets this variable to the string specified
		/** \todo Have to make it possible to set a list with a string, using a tokenizer(?)
		*/
		virtual void SetValue(const StringType &s) { }

		inline ::std::vector<ItemType>& get_list() { return list; }
	};

} // *** End of namespace

#endif //EXPANDLIB_VARTYPE_LIST_H_INCLUDED
