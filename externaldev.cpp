#include "externaldev.h"
#include "utils/crc.h"

#include <Arduino.h>
#include <Wire.h>


struct UniformMessageData{
	static constexpr uint8_t SYNC_FLAG = 0x5B;
	// header
	uint8_t sync = SYNC_FLAG;
	// message type + data + terminator
	uint8_t data[5];
	uint8_t crc[2];
};
struct UniformMessageDataWithWakeup{
	static constexpr int8_t WAKEUP_BYTE = '\n';
	uint8_t wakeupBytes[1];
	UniformMessageData messageData;
};

void sendAck(){
	Serial.write('\n');
	Serial.write(ACKNOWLEDGE_DATA);
}

Message receiveMessageUART() {

	UniformMessageData uniformMessage;
	static_assert(sizeof(UniformMessageData) == 8);
	if (Serial.available() >= sizeof(UniformMessageData)) {
		if(Serial.peek() != UniformMessageData::SYNC_FLAG){
			Serial.read();	
		}
		else {
			(void)Serial.readBytes((uint8_t*)&uniformMessage, sizeof(UniformMessageData));
			Message::MessageData messageData{.none = {}};

			uint16_t computedCRC = crc16(uniformMessage.data, sizeof(uniformMessage.data));
			uint16_t messageCRC = (uniformMessage.crc[1] << 8) | uniformMessage.crc[0];
			if(computedCRC == messageCRC){
				memcpy(&messageData, uniformMessage.data, sizeof(Message::MessageData));

				return Message{
					.data = messageData,
					.type = Message::Type(uniformMessage.data[sizeof(Message::MessageData)])
				};
			}
			
		}

	}

	return Message{
		.data = {.none = {}},
		.type = Message::Type::NONE
	};
}

void sendMessageUART(const Message &messageIn) {

	UniformMessageDataWithWakeup uniformMessageWithWakeup{.wakeupBytes = {0x5B}};
	UniformMessageData& uniformMessage = uniformMessageWithWakeup.messageData;

	static_assert(sizeof(Message::MessageData) == 4);
	memcpy(&uniformMessage.data[0], &messageIn.data, sizeof(Message::MessageData));
	uniformMessage.data[sizeof(Message::MessageData)] = uint8_t(messageIn.type);
	uint16_t crc = crc16(uniformMessage.data, sizeof(uniformMessage.data));

	uniformMessage.crc[0] = uint8_t(crc);
	uniformMessage.crc[1] = uint8_t(crc>>8);
	Serial.write((const uint8_t*)&uniformMessageWithWakeup, sizeof(uniformMessageWithWakeup));
	Serial.flush();
}

Message handleRequestFunc(Message::Type type){

	switch (type)
	{
		case Message::Type::TIME_SYNC:
			return Message{.data = {.timeSync = Message::TimeSync{.newTime = uint32_t(millis())}}, .type = Message::Type::TIME_SYNC};
		
		default:
			return Message{.data = {.none = {}}, .type = Message::Type::NONE};
	}

}



///====== Temperature Sensor ======///

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
			sendMessageUART(Message{.data = {.alive = Message::Alive{.address = 0xAAAAAAAA}}, .type = Message::Type::ALIVE});
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




void initExternalDevices(){
	Wire.begin();
	Wire.setClock(400000);
	Wire.setTimeout(2);
	TemperatureHumiditySensor.begin();
	/*temperatureHumiditySensor.begin();

	if(!temperatureHumiditySensor.isInitialized()){
		Serial.println(F("AHT10 initialization failed"));
	}*/

}


void communicateWithExternalDevices(){
	MessageManager.handle();
	TemperatureHumiditySensor.communicateWithSensor();
}

