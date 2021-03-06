#include <bluetoothException.h>
#include <windows.h>

BluetoothException::BluetoothException(HRESULT result)
{
	FormatMessage(
		// use system message tables to retrieve error text
		FORMAT_MESSAGE_FROM_SYSTEM
		// allocate buffer on local heap for error text
		| FORMAT_MESSAGE_ALLOCATE_BUFFER
		// Important! will fail otherwise, since we're not 
		// (and CANNOT) pass insertion parameters
		| FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
		result,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&m_what,  // output 
		0, // minimum size for output buffer
		NULL);   // arguments - see note 

	if (!m_what)
	{
		m_what = _strdup("Unknown Bluetooth Error");
		m_free = true;
	}
	else 
	{
		m_free = false;
		m_what[strlen(m_what) - 2] = '\0';	// eat "\n\r"
	}
}

BluetoothException::BluetoothException(const char* what)
{
	m_what = _strdup(what);
	m_free = true;
}

//--------------------------------------------------------------------------------------------------
//	BluetoothException (public ) []
//--------------------------------------------------------------------------------------------------
BluetoothException::BluetoothException(QString what)
{
	m_what = _strdup(what.toLocal8Bit().constData());
	m_free = true;
}

BluetoothException::~BluetoothException() noexcept
{
	if (m_free)
		free(m_what);
	else
		LocalFree(m_what);
}

char const* BluetoothException::what() const
{
	return m_what;
}

