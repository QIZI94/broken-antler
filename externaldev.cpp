#include "externaldev.h"

#include <Arduino.h>
#include <Wire.h>

void initExternalDevices(){
	Wire.begin();
	Wire.setClock(400000);
	Wire.setTimeout(2);
	temperatureHumiditySensor.begin();
	/*temperatureHumiditySensor.begin();

	if(!temperatureHumiditySensor.isInitialized()){
		Serial.println(F("AHT10 initialization failed"));
	}*/

}


void communicateWithExternalDevices(){
	temperatureHumiditySensor.communicateWithSensor();
}





void HTU21DTempHumSensor::begin() volatile {
	Wire.beginTransmission(HTU21D_ADDR);
    Wire.write(CMD_SOFT_RESET);
    Wire.endTransmission();

    delay(20);
	lastRequest = NO_REQUEST;
}

void HTU21DTempHumSensor::end() volatile {
	lastRequest = UNINITIALIZED;
}



void HTU21DTempHumSensor::communicateWithSensor(){
	if(!requestDelay.isDown()){
		return;
	}
	uint16_t raw;
	switch (lastRequest)
	{
		case Request::NO_REQUEST:
			if(sendRequest(Request::CMD_TEMP_NOHOLD, 50)){
				return;
			}
			break;
		case Request::CMD_TEMP_NOHOLD:
			if(readRequest(raw)){			
				lastTemperature10 = -47 + (((int32_t)176 * raw) >> 16);
				if(sendRequest(Request::CMD_HUM_NOHOLD, 20)){
					return;
				}
			}
			break;
			
		case Request::CMD_HUM_NOHOLD:
			if(readRequest(raw)){
				lastHumidity10 = -6 + (((int32_t)125 * raw) >> 16);
			}
			break;
		case Request::UNINITIALIZED:
			return;
			break;
	}

	requestDelay.restart(1000);
	lastRequest = NO_REQUEST;
}

bool HTU21DTempHumSensor::sendRequest(Request request, uint16_t waitTime)
{
	
    Wire.beginTransmission(sensorAddress);
    Wire.write(request);

    if (Wire.endTransmission() != 0)
        return false;
	lastRequest = request;
	requestDelay.restart(waitTime);
}

bool HTU21DTempHumSensor::readRequest(uint16_t& raw)
{
	Wire.requestFrom(sensorAddress, (uint8_t)3);

    if (Wire.available() < 3)
        return false;

    raw = (Wire.read() << 8);
    raw |= Wire.read();

    Wire.read(); // ignore CRC

    raw &= 0xFFFC;

	return raw;
}
