#pragma once

#define SENSOR_ID_LENGTH           32
#define SENSOR_SECRET_LENGTH       64
#define GATEWAY_LENGTH			  128
#define P1_BAUDRATE_LENGTH         10
#define LATITUDE_LENGTH            16
#define LONGITUDE_LENGTH           16

namespace smartenergy
{
	struct Configuration {
		char     sensor_id[SENSOR_ID_LENGTH];
		char     sensor_secret[SENSOR_SECRET_LENGTH];
		char     api_key[SENSOR_SECRET_LENGTH];
		double   latitude;
		double   longitude;
		int      p1_baudrate;
	};

	extern bool readAppConfig(Configuration *app_config);
	extern bool writeAppConfig();
	extern void initConfiguration();
	extern bool deleteAppConfig();
	extern Configuration& getConfiguration();
}
