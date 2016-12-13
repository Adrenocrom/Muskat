/***********************************************************
 *
 *
 *						MUSKAT SOURCE
 *					 ===================
 *
 *		AUTHOR: Josef Schulz
 *
 *		Implements only the insertZeros function.
 *
 *
 ***********************************************************/

#include "muskat.h"

string insertZeros(uint num_zeros, uint n) {
	string s = to_string(n);
	s = string(num_zeros - s.size(), '0') + s;

	return s;
}
