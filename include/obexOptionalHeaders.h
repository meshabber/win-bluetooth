//--------------------------------------------------------------------------------------------------
// 
//	WIN-BLUETOOTH
//
//--------------------------------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), to deal in the Software without 
// restriction, including without limitation the rights to use, copy, modify, merge, publish, 
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or 
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//--------------------------------------------------------------------------------------------------
//
// Copyright (c) 2018 Nic Holthaus
// 
//--------------------------------------------------------------------------------------------------
//
// ATTRIBUTION:
//
//
//--------------------------------------------------------------------------------------------------
//
/// @file	obexOptionalHeaders.h
/// @brief	Vector wrapper for the optional headers with a few convenience functions
//
//--------------------------------------------------------------------------------------------------

#pragma once
#ifndef obexOptionalHeaders_h__
#define obexOptionalHeaders_h__

//-------------------------
//	INCLUDES
//-------------------------

#include <vector> 
#include <obexHeader.h>

//-------------------------
//	FORWARD DECLARATIONS
//-------------------------


//--------------------------------------------------------------------------------------------------
//	OBEXOptionalHeaders
//--------------------------------------------------------------------------------------------------

class OBEXOptionalHeaders : public std::vector<OBEXHeader>
{
	// This looks like it wants to be a map class, but we can't do that
	// because the headers need to be streamed in the order that they were created, not by
	// the key order.

public:

	using Base = std::vector<OBEXHeader>;
	
	// inherit base class constructors
	using Base::Base;

	std::tuple<bool, Base::iterator> contains(OBEXHeader::HeaderIdentifier id);
	std::tuple<bool, Base::const_iterator> contains(OBEXHeader::HeaderIdentifier id) const;
	OBEXHeader& operator[](OBEXHeader::HeaderIdentifier id);
	
	
protected:

	

};


#endif // obexOptionalHeaders_h__
