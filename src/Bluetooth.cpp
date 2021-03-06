#include <Bluetooth.h>
#include <bluetoothException.h>
#include <bluetoothRadio.h>
#include <bluetoothUuids.h>

#include <utility>
#include <cassert>
#include <algorithm>
#include <QHostInfo>
#include <array>

#include <winsock2.h>
#include <ws2bth.h>
#include <BluetoothAPIs.h>

#pragma comment(lib, "Bthprops.lib")
#pragma comment(lib, "Ws2_32.lib")

//------------------------------
//	MACROS
//------------------------------

#define ERR HRESULT_FROM_WIN32(GetLastError())

//------------------------------
//	STATIC MEMBERS
//------------------------------

std::unordered_map<QString, BluetoothRadio> Bluetooth::m_localRadios;
std::unordered_map<QString, BluetoothDevice> Bluetooth::m_remoteDevices;
BluetoothDevice Bluetooth::m_invalidDevice;
BluetoothRadio Bluetooth::m_invalidRadio;
QString Bluetooth::m_hostname(QHostInfo::localHostName().toUpper());

//--------------------------------------------------------------------------------------------------
//	RAII WRAPPERS
//--------------------------------------------------------------------------------------------------

class BluetoothFindRadioHandle
{
public:

	BluetoothFindRadioHandle(HBLUETOOTH_RADIO_FIND handle) noexcept : m_handle(handle) {}
	~BluetoothFindRadioHandle() 
	{ 
		// in theory this can fail, but not much we can do about it
		if (!BluetoothFindRadioClose(m_handle))
			assert(true);	// if you hit this assert, you probably need to do some serious debugging
	}
	operator HBLUETOOTH_RADIO_FIND() noexcept { return m_handle; }

private:

	HBLUETOOTH_RADIO_FIND m_handle;
};

class BluetoothFindDeviceHandle
{
public:

	BluetoothFindDeviceHandle(HBLUETOOTH_DEVICE_FIND handle) noexcept : m_handle(handle) {}
	~BluetoothFindDeviceHandle()
	{
		// in theory this can fail, but not much we can do about it
		if (!BluetoothFindDeviceClose(m_handle))
			assert(true);	// if you hit this assert, you probably need to do some serious debugging
	}
	operator HBLUETOOTH_DEVICE_FIND() noexcept { return m_handle; }

private:

	HBLUETOOTH_DEVICE_FIND m_handle;
};

//--------------------------------------------------------------------------------------------------
//	METHODS
//--------------------------------------------------------------------------------------------------
bool Bluetooth::enumerateLocalRadios(bool refreshList /*= false*/)
{
	if(refreshList || m_localRadios.empty())
	{
		// this thing is required by the API but is pointless for our purposes. Probably used to
		// differentiate x86/x64 systems
		BLUETOOTH_FIND_RADIO_PARAMS btfrp;
		btfrp.dwSize = sizeof(BLUETOOTH_FIND_RADIO_PARAMS);

		HANDLE radio;

		// Get the first local radio
		BluetoothFindRadioHandle radioFindHandle = BluetoothFindFirstRadio(&btfrp, &radio);
		if (radio) 
		{
			BluetoothRadio r(radio);
			m_localRadios[r.name()] = std::move(r);
		}
		else
		{
			// this PC doesn't have any radios
			if (GetLastError() == ERROR_NO_MORE_ITEMS)
				return false;
			else
				throw BluetoothException(ERR);
		}

		// get the rest of the local radios
		while (BluetoothFindNextRadio(radioFindHandle, &radio))
		{
			BluetoothRadio r(radio);
			m_localRadios[r.name()] = std::move(r);
		}

		if (GetLastError() != ERROR_NO_MORE_ITEMS)
			throw BluetoothException(ERR);
	}

	return m_localRadios.size();
}

bool Bluetooth::enumerateRemoteDevices(bool refreshList /*= false*/)
{
	if (refreshList || m_remoteDevices.empty())
	{
		// this thing is required by the API but is pointless for our purposes. Probably used to
		// differentiate x86/x64 systems
		BLUETOOTH_DEVICE_SEARCH_PARAMS btdsp;
		btdsp.fIssueInquiry = true;
		btdsp.fReturnAuthenticated = true;
		btdsp.fReturnConnected = true;
		btdsp.fReturnRemembered = true;
		btdsp.fReturnUnknown = true;
		btdsp.cTimeoutMultiplier = 1;
		btdsp.dwSize = sizeof(btdsp);

		BLUETOOTH_DEVICE_INFO btdi;
		btdi.dwSize = sizeof(btdi);

		for (const auto& [radioName, radio] : localRadios())
		{
			btdsp.hRadio = (HANDLE)radio.handle();
	
			// Get the first local radio
			BluetoothFindDeviceHandle device = BluetoothFindFirstDevice(&btdsp, &btdi);
			if (device) 
			{
				m_remoteDevices.emplace(std::piecewise_construct,
					std::forward_as_tuple(QString::fromWCharArray(btdi.szName)), 
					std::forward_as_tuple(btdsp.hRadio, &btdi));
			}
			else
			{
				// this PC doesn't have any radios
				if (GetLastError() == ERROR_NO_MORE_ITEMS)
					return false;
				else
					throw BluetoothException(ERR);
			}
	
			// get the rest of the local radios
			while (BluetoothFindNextDevice(device, &btdi))
			{
				m_remoteDevices.emplace(std::piecewise_construct,
					std::forward_as_tuple(QString::fromWCharArray(btdi.szName)),
					std::forward_as_tuple(btdsp.hRadio, &btdi));
			}
	
			if (GetLastError() != ERROR_NO_MORE_ITEMS)
				throw BluetoothException(ERR);
		}
	}

	return !m_remoteDevices.empty();
}

BluetoothRadio& Bluetooth::localRadio(const QString& name, bool refreshList /*= false*/)
{
	enumerateLocalRadios(refreshList);
	return m_localRadios.count(name) ? m_localRadios[name] : m_invalidRadio;
}

BluetoothRadio& Bluetooth::localRadio(bool refreshList /*= false*/)
{
	return localRadio(m_hostname, refreshList);
}

std::unordered_map<QString, BluetoothRadio>& Bluetooth::localRadios(bool refreshList /*= false*/)
{
	enumerateLocalRadios(refreshList);
	return m_localRadios;
}

BluetoothDevice& Bluetooth::remoteDevice(const QString& name, bool refreshList /*= false*/)
{
	enumerateRemoteDevices(refreshList);
	return m_remoteDevices.count(name) ? m_remoteDevices[name] : m_invalidDevice;
}

std::unordered_map<QString, BluetoothDevice>& Bluetooth::remoteDevices(bool refreshList /*= false*/)
{
	enumerateRemoteDevices(refreshList);
	return m_remoteDevices;
}

//--------------------------------------------------------------------------------------------------
//	name (public ) [static ]
//--------------------------------------------------------------------------------------------------
QString Bluetooth::name(const BluetoothAddress& address)
{
	for (const auto& [name, radio] : m_localRadios)
	{
		if (radio.address() == address)
			return name;
	}
	for (const auto&[name, device] : m_remoteDevices)
	{
		if (device.address() == address)
			return name;
	}

	return "INVALID";
}