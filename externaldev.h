#ifndef EXTERNAL_DEVICES_H
#define EXTERNAL_DEVICES_H

#include <inttypes.h>

#include "timer.h"

class HTU21DTempHumSensor{
private:
	static constexpr uint8_t HTU21D_ADDR = 0x40;
	enum Request : uint8_t{
		UNINITIALIZED = 0x00,
		NO_REQUEST = 0x01,
		CMD_TEMP_NOHOLD = 0xF3,
		CMD_HUM_NOHOLD = 0xF5
	};

	static constexpr uint8_t CMD_SOFT_RESET  = 0xFE;
public:
	HTU21DTempHumSensor(uint8_t sensorAddress = HTU21D_ADDR) : sensorAddress(sensorAddress){}
	void begin() volatile;
	void end() volatile;
	void communicateWithSensor();

	inline bool isInitialized() const volatile {
		return lastRequest != Request::UNINITIALIZED;
	}

	inline bool isBusy() const volatile {
		return !requestDelay.isDown();
	}


	inline int16_t getTemperature10() const {
		return lastTemperature10;
	}

	inline uint16_t getHumidity10() const {
		return lastHumidity10;
	}
private:
	bool sendRequest(Request request, uint16_t waitTime);
	bool readRequest(uint16_t& raw);
private:
	StaticTimer1ms requestDelay;
	volatile int16_t lastTemperature10 = 0;
	volatile int16_t lastHumidity10 = 0;
	uint8_t sensorAddress;
	volatile bool requestedRead = false;
	Request lastRequest = Request::UNINITIALIZED;
};
inline HTU21DTempHumSensor temperatureHumiditySensor;

extern void initExternalDevices();
extern void communicateWithExternalDevices();

#endif