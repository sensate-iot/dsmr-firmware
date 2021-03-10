#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

namespace sensateiot
{
	static constexpr int BufferSize = 4096;

	struct Message {
		String sensorId;
		String data;
		double latitude;
		double longitude;
		String encoding;
		String secret;
	};

	void writeMessage(const Message& msg);

	class Client {
	public:
		explicit Client();
		explicit Client(const String& key);
		virtual ~Client();

		Client(const Client& other) = delete;
		Client& operator=(const Client& other) = delete;

		bool begin();
		void end();

		void send(const Message& msg);
		void setApiKey(const String& key);

		void verify();

	private:
		String m_key;
		BearSSL::WiFiClientSecure m_client;
		HTTPClient m_http;
		DynamicJsonDocument m_doc;

		// Methods
		String buildJson(const Message& message);
		bool internalBegin();
		bool internalSend(const Message& msg);
	};

	extern Client httpClient;
}

