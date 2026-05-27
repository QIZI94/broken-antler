#include "externaldev.h"
#include "utils/crc.h"

#include <Arduino.h>
#include <Wire.h>


struct UniformMessageData{
	static constexpr uint8_t SYNC_FLAG = 0x5B;
	// header
	uint8_t sync = SYNC_FLAG;
	// message type + data + terminator
	uint8_t data[sizeof(Message)];
	uint8_t crc[2];
};

void sendAck(){
	Serial.write('\n');
	Serial.write(ACKNOWLEDGE_DATA);
}

bool validateMessage(const UniformMessageData& msg)
{
    uint16_t receivedCrc =
        (msg.crc[1] << 8) | msg.crc[0];

    uint16_t computedCrc =
        crc16(
            reinterpret_cast<const uint8_t*>(&msg.data),
            sizeof(msg.data)
        );

    return receivedCrc == computedCrc;
}

MessageReceiveState receiveMessageUART(Message& messageOut){

    static uint8_t buffer[sizeof(UniformMessageData)];
    static size_t index = 0;

    while (Serial.available())
    {
		
        uint8_t byte = Serial.read();

        // ---------------------------------------------------------------------
        // Waiting for SYNC
        // ---------------------------------------------------------------------
        if (index == 0)
        {
            if (byte != UniformMessageData::SYNC_FLAG)
            {
                continue;
            }
        }

        buffer[index++] = byte;

        // ---------------------------------------------------------------------
        // Full message received
        // ---------------------------------------------------------------------
        if (index == sizeof(UniformMessageData))
        {
			
			UniformMessageData uniformMessage;
            memcpy(&uniformMessage, buffer, sizeof(UniformMessageData));

            index = 0;

            // Validate CRC
            if (validateMessage(uniformMessage))
            {
				//Serial.println("here");
				Message::MessageData messageData{.none = {}};

		
				memcpy(&messageData, uniformMessage.data, sizeof(Message::MessageData));

				Message message = Message{
					messageData,
					Message::Type(uniformMessage.data[sizeof(Message::MessageData)])
				};
				memcpy(&messageOut, &message, sizeof(Message));
				return MessageReceiveState::DONE;
                //return true;
            }

            // -----------------------------------------------------------------
            // CRC failed
            //
            // IMPORTANT:
            // Try to recover sync immediately instead of discarding everything.
            // This prevents desync when SYNC appears inside stream.
            // -----------------------------------------------------------------
            for (size_t i = 1; i < sizeof(UniformMessageData); ++i)
            {
                if (buffer[i] == UniformMessageData::SYNC_FLAG)
                {
                    memmove(buffer, &buffer[i], sizeof(UniformMessageData) - i);
                    index = sizeof(UniformMessageData) - i;
					return MessageReceiveState::IN_PROGRESS;
                }
            }

            index = 0;
        }
    }

    return index == 0 ? MessageReceiveState::IDLE : MessageReceiveState::IN_PROGRESS;
}

void sendMessageUART(const Message &messageIn) {

	UniformMessageData uniformMessage;

	static_assert(sizeof(Message::data) == 5);
	memcpy(&uniformMessage.data[0], &messageIn.data, sizeof(Message::MessageData));
	uniformMessage.data[sizeof(Message::MessageData)] = uint8_t(messageIn.type);
	uint16_t crc = crc16(uniformMessage.data, sizeof(uniformMessage.data));

	uniformMessage.crc[0] = uint8_t(crc);
	uniformMessage.crc[1] = uint8_t(crc>>8);
	Serial.write('\n');
	Serial.write((const uint8_t*)&uniformMessage, sizeof(uniformMessage));
	Serial.flush();
}

Message handleRequestFunc(Message::Type type){

	switch (type)
	{
		case Message::Type::TIME_SYNC:
			return Message{Message::TimeSync{.newTime = uint32_t(millis())}};
		
		default:
			return Message{};
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
			sendMessageUART(Message{Message::Alive{.address = 0xAAAAAAAA}});
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

