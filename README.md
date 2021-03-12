# Sensate IoT - Smart Energy

The Sensate IoT Smart Energy project implements an IoT solution for (Dutch)
Smart Meters. The project consists of several repository's:

- DSMR parser;
- DSMR web client (implementator of this service);
- Several customer facing apps;
- DSMR firmware (this repo).

## DSMR

DSMR stands for Dutch Smart Meter Requirements. This standard describes how smart meters
should export data for external applications. This project serves as a proxy between
the serial port on the smart meter and a data platform in the cloud.

## Environment

In order to perform beter analysis of energy data, the energy and power data is expanded
with environmental measurements:

- Temperature;
- Air pressure;
- Relative humidity.

## Processing

This application is not responsible for data processing. Data is presented as-is to the
message gateway of Sensate IoT.
