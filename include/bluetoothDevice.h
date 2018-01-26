//--------------------------------------------------------------------------------------------------
// 
///	@project WIN-BLUETOOTH
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
/// @file	bluetoothRadio.h
/// @brief	Description of a single bluetooth radio
//
//--------------------------------------------------------------------------------------------------

#pragma once
#ifndef bluetoothDevice_h__
#define bluetoothDevice_h__

//------------------------------
//	INCLUDES
//------------------------------

#include <string>

//--------------------------------------------------------------------------------------------------
//	BLUETOOTH DEVICE
//--------------------------------------------------------------------------------------------------
class BluetoothDevice
{
public:

	BluetoothDevice(void* radioHandle = nullptr, void* deviceInfo = nullptr);
	virtual ~BluetoothDevice();
	BluetoothDevice(const BluetoothDevice& other);
	BluetoothDevice(BluetoothDevice&& other);
	BluetoothDevice& operator=(const BluetoothDevice& other);
	BluetoothDevice& operator=(BluetoothDevice&& other);

	unsigned long long address() const;
	std::wstring name() const;
	unsigned long classOfDevice() const;

	bool isValid() const;
	bool connected();
	bool remembered();
	bool authenticated();
	std::string_view lastSeen();
	std::string_view lastUsed();

	bool operator==(const std::wstring_view name) const;
	bool operator==(const unsigned long long address) const;

protected:

	 void refresh();

private:

	void*				m_radioHandle	= nullptr;
	void*				m_deviceInfo	= nullptr;
};

#endif // bluetoothDevice_h__