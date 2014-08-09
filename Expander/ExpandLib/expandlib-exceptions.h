
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
	\brief Headerfile for the exceptions that can be thrown by expandlib.
*/

#ifndef EXPANDLIB_EXCEPTIONS_H_INCLUDED
#define EXPANDLIB_EXCEPTIONS_H_INCLUDED

namespace expander { // *** Start of namespace

	/// Class for general errors (things like "unable to allocate memory").

	class general_error {
	private:
		const _TCHAR* context_;
		const _TCHAR* error_message_;
		const _TCHAR* about_;

	public:
		general_error (const _TCHAR* context, const _TCHAR* error_message) throw() :
			context_(context),
			error_message_(error_message),
			about_("")
		{}

		general_error (const _TCHAR* context, const _TCHAR* error_message,  const _TCHAR* about) throw() :
			context_(context),
			error_message_(error_message),
			about_(about)
		{}

		inline const _TCHAR* context () const throw()
		{
			return context_;
		}
		inline const _TCHAR* error_message () const throw()
		{
			return error_message_;
		}
		inline const _TCHAR* about () const throw()
		{
			return about_;
		}
	};


	/// Class for expand errors (things like "command not found" or "variable not found").

	class expand_error {
	private:
		const _TCHAR* context_;
		const _TCHAR* error_message_;
		const _TCHAR* about_;

	public:
		expand_error (const _TCHAR* context, const _TCHAR* error_message) throw() :
			context_(context),
			error_message_(error_message),
			about_("")
		{}

		expand_error (const _TCHAR* context, const _TCHAR* error_message,  const _TCHAR* about) throw() :
			context_(context),
			error_message_(error_message),
			about_(about)
		{}

		inline const _TCHAR* context () const throw()
		{
			return context_;
		}
		inline const _TCHAR* error_message () const throw()
		{
			return error_message_;
		}
		inline const _TCHAR* about () const throw()
		{
			return about_;
		}
	};

} // *** End of namespace

#endif //EXPANDLIB_EXCEPTIONS_H_INCLUDED
