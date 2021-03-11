#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <sha256.h>
#include <ESP8266WiFi.h>

#include "config.h"
#include "sensateiotclient.h"
#include "cert.h"
#include "util.h"

#ifndef DEBUG
#define DBG_STREAM(fmt, ...) Serial.printf(fmt, ## __VA_ARGS__) 
#else
#define DBG_STREAM(...)
#endif

static String url  = "https://enbst3j0j8mqu2k.m.pipedream.net/network/v1/gateway/messages";
//static String url = "https://api.sensateiot.com/network/v1/gateway/messages";
static X509List certChain(rootCa);

namespace sensateiot
{
	Client::Client() : Client("")
	{
	}

	Client::Client(const String& key) : m_key(key), m_client(), m_http(), m_doc(BufferSize)
	{
	}

	Client::~Client()
	{
		this->end();
	}

	void Client::end()
	{
		this->m_http.end();
	}

	void Client::setApiKey(const String& key)
	{
		this->m_key = key;
	}

	bool Client::begin()
	{
		return this->internalBegin();
	}

	bool Client::internalBegin()
	{
		this->m_client.setTrustAnchors(&certChain);
		this->m_client.setBufferSizes(2048, 2048);
		this->m_http.setReuse(true);

		if(!this->m_http.begin(this->m_client, url)) {
			DBG_STREAM("Unable to start HTTP client!\n");
			return false;
		}

		return true;
	}

	String Client::buildJson(const Message &msg)
	{
		this->m_doc.clear();
		this->m_doc["sensorId"] = msg.sensorId;
		this->m_doc["secret"]   = msg.secret;
		this->m_doc["data"]     = base64Encode(msg.data);
		this->m_doc["longitude"] = msg.longitude;
		this->m_doc["latitude"]  = msg.latitude;
		this->m_doc["Encoding"] = "Base64";

		String result;
		serializeJson(this->m_doc, result);

		return result;
	}

	String computeSignature(String json)
	{
		String result = "$";

		result.concat(sha::sha256_compute(json));
		result.concat("==");
		return result;
	}

	void Client::send(const Message& msg)
	{
		auto json = this->buildJson(msg);
		auto signature = computeSignature(json);

		json.replace(msg.secret, signature);
		DBG_STREAM("Message: %s\n", json.c_str());

		this->m_http.addHeader("Content-Type", "application/json");
		this->m_http.addHeader("X-ApiKey", this->m_key);

		auto result = this->m_http.POST(json);

		DBG_STREAM("Post result: %i\n", result);
	}

	Client httpClient;
}
