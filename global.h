/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * based on FreeUDFLibC - copyright (c) 1999 Gregory Deatz
 *
 * File Name:   global.h
 * Description: This header contains the global prototype definitions
 *  
 * This file is part of FreeAdhocUDF.
 * FreeAdhocUDF is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FreeAdhocUDF is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with FreeAdhocUDF.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

// ct 2008-11-18
// dirty trick to "patch" ibase.h from InterBase because they don't know Watcom
//#if defined(__WATCOMC__)
//#define __BORLANDC__
//#endif

#include "ibase.h"

//#if defined(__WATCOMC__)
//#undef __BORLANDC__
//#endif
// end dirty trick

#define LOC_STRING_SIZE 256
#define MAX_STRINGBUFFER_SIZE 32768

// ct 2010-03-17 use of 64bit in 64bit systems
#if defined(__BORLANDC__) || defined(_MSC_VER) || defined(__WATCOMC__)
#define WINDOWS
  #if defined(_WIN64)
  #else
  #endif
#else
  #if defined(_LP64)
  #else
  #endif
#endif

#ifdef _MSC_VER
// tt 2010-12-06 MSVC wants you to use ISO conformant standard c library function names. 
//               To avoid MSVC from emmitting warnings and linke errors, 
//               we re-direct them here
#define snprintf _snprintf
#define getpid _getpid
#endif

// ct 2007-12-03 readded ib_util for ib_util_malloc 
#include "ib_util.h"
#define MALLOC ib_util_malloc
// pm 2006-09-24
// Workaround for Firebird bug (buffer overflow):
// Firebird writes data after end of returned UDF data.
// Allocation of the string buffer maximum of >= 32762 Bytes
// for all strings solves this problem.
// n is dummy referenced to avoid "unused variable" warning.
// FB_API_VER is only defined in ibase.h of FireBird
// #ifdef FB_API_VER
// #define MALLOC(n) (ib_util_malloc((n) - (n) + MAX_STRINGBUFFER_SIZE))
// #else
// #define MALLOC ib_util_malloc
// #endif

// ct 2011-11-30 raise of exactness
#ifndef M_PI
#define M_PI 3.1415926535897932
#endif

// ct 2011-11-30 raise of exactness
#ifndef M_E
#define M_E 2.7182818284590452
#endif

#ifndef M_C
#define M_C 299792458
#endif

#ifndef M_G
#define M_G 9.80665
#endif

#ifndef M_K
#define M_K -273.15
#endif

// to use for the method of epsilonics
// initial point of the problem is how computers interpret/save floatingpoint numbers 
// According to IEEE 754 they convert them to a binary construction
// The outcome sometimes is a repeating sequences  
// simmilar what happens if you count 10 / 3 
// but this is impossible to save because of to much digits
// so the computer rounds the number - and now you have enforced differences
// The epsilonics recognise certain variations. 
// This value is called epsilon - here we use the value from float.h 
// where DBL_EPSILON is the minimum positive floating-point number 
// so that 1.0 + DBL_EPSILON != 1.0
// The method is NOT to test against 0 : a + b == 0
// INSTEAD to test against epsilon : a + b < epsilon
#ifndef DBL_EPSILON
#define DBL_EPSILON 2.2204460492503131e-16 * 100.0
#endif
