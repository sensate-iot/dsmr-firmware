#include <Arduino.h>
#include <Stream.h>

#include "stringstream.h"

namespace stringstream
{
	StringStream::StringStream() : m_string(), m_position(0)
	{
	}

	StringStream::StringStream(const String &s) : m_string(s), m_position(0)
	{
	}

	int StringStream::available()
	{
		return m_string.length() - m_position;
	}

	int StringStream::read()
	{
		return m_position < m_string.length() ? m_string[m_position++] : -1;
	}

	int StringStream::peek()
	{
		return m_position < m_string.length() ? m_string[m_position] : -1;
	}

	void StringStream::flush()
	{
	}

	size_t StringStream::write(uint8_t c)
	{
		m_string += (char)c;
		return 1;
	}
} // namespace m_stringstream
