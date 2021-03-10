#pragma once

#include <Arduino.h>
#include <Stream.h>

namespace stringstream 
{
	class StringStream : public Stream
	{
	public:
		explicit StringStream();
		explicit StringStream(const String &s);

		virtual int available();
		virtual int read() ;
		virtual int peek() ;
		virtual void flush();
		virtual size_t write(uint8_t c);

		void write(const String& data);

	private:
		String m_string;
		std::size_t m_position;
	};
} // namespace StringStream
