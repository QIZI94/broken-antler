#include "externaldev.h"
#include "utils/crc.h"
#include "timer.h"

#include <Arduino.h>
#include <Wire.h>


///====== High level UART Communication ======///

MessageReceptionState UARTMessageHandler::handleMessagesReception(UARTMessageDriver &driver) {
	UniformMessage receivedMessage;
	MessageReceptionState messageReceiveState = driver.receiveMessage(receivedMessage);
	if(messageReceiveState == MessageReceptionState::DONE){
		UniformMessage acknowledge = UniformMessage::Acknowledge{.acknowledgedMessage = receivedMessage.type};
		switch(receivedMessage.type){
			case UniformMessage::Type::NONE:
				break;
			case UniformMessage::Type::ACKNOWLEDGE:
				setDeferredRepeatCountMask(receivedMessage.data.acknowledge.acknowledgedMessage, DEFERRED_SUCCESSFUL_STOP_REPEAT);
				break;
			case UniformMessage::Type::REQUEST:
				acknowledge = requestHandler(receivedMessage.data.request.requestedMessageType);
			default:
				driver.sendMessage(acknowledge);
				if(receivedMessage.isResponse){
					setDeferredRepeatCountMask(UniformMessage::Type::REQUEST, DEFERRED_SUCCESSFUL_STOP_REPEAT);
				}
				switch (receivedMessage.type){
				
					case UniformMessage::Type::ALIVE:
						sendDeferredMessage(UniformMessage::Request{.requestedMessageType = UniformMessage::Type::TIME_SYNC});
						break;
					case UniformMessage::Type::TIME_SYNC:
						setRTC(receivedMessage.data.timeSync.newTime);
						break;

					default:
						break;
				}
		}
	}
	
	return messageReceiveState;
}

MessageTransmissionState UARTMessageHandler::handleMessagesTransmission(UARTMessageDriver &driver) {
	uint32_t microsNow = micros();
	bool shouldRepeat = (microsNow - messageRepeatLastTime) >= DEFERRED_REPEAT_TIME_US;
	if(shouldRepeat || deferredSendAtLeastOnceMask != 0){
		//Serial.println(deferredMessageSendAndAckMask, BIN);
		for(uint8_t messageTypeIndex = 0; messageTypeIndex < DEFERRED_MESSAGES_COUNT; ++messageTypeIndex) {
			if(!clearDeferredSendAtLeastOnce(UniformMessage::Type(messageTypeIndex)) && !shouldRepeat){
				continue;
			}
			uint8_t repeatCount = getDeferredRepeatCount(UniformMessage::Type(messageTypeIndex));
			if(repeatCount == 0x00){
				continue;
			}

			setDeferredRepeatCountMask(UniformMessage::Type(messageTypeIndex), --repeatCount);

			driver.sendMessage(UniformMessage(deferredMessages[messageTypeIndex], UniformMessage::Type(messageTypeIndex)));
			messageRepeatLastTime = microsNow;
   		}
		
	}
	return deferredMessageSendAndAckMask == 0 ? MessageTransmissionState::DONE : MessageTransmissionState::IN_PROGRESS;
}

void UARTMessageHandler::sendDeferredMessage(const UniformMessage& message, uint8_t repeatCount) {
	deferredMessages[uint8_t(message.type)] = message.data;
	setDeferredRepeatCountMask(message.type, repeatCount);
	setDeferredSendAtLeastOnce(message.type);
}

UniformMessage UARTMessageHandler::requestHandler(UniformMessage::Type msgType) {
	switch (msgType){
		case UniformMessage::Type::TIME_SYNC:
			return UniformMessage{UniformMessage::TimeSync{.newTime = rtcNow()}, true};
		
		default:
			return UniformMessage::Acknowledge{.acknowledgedMessage = UniformMessage::Type::REQUEST};
	}
}

void UARTMessageHandler::setDeferredRepeatCountMask(UniformMessage::Type msgType, uint8_t attemptCount) {
	uint8_t shift = uint8_t(msgType) << 1;
	deferredMessageSendAndAckMask = (deferredMessageSendAndAckMask & ~(DEFERRED_REPEAT_MASK << shift)) | (attemptCount << shift);

}

uint8_t UARTMessageHandler::getDeferredRepeatCount(UniformMessage::Type msgType) {
	uint8_t shift = uint8_t(msgType) << 1;
	return (deferredMessageSendAndAckMask >> shift) & DEFERRED_REPEAT_MASK;
}

void UARTMessageHandler::setDeferredSendAtLeastOnce(UniformMessage::Type msgType) {
	deferredSendAtLeastOnceMask |= 0x01 << uint8_t(msgType);
}

bool UARTMessageHandler::clearDeferredSendAtLeastOnce(UniformMessage::Type msgType) {
	const uint8_t msgTypeMask = 0x01 << uint8_t(msgType);
	const bool wasSet = (deferredSendAtLeastOnceMask & msgTypeMask) != 0;
	deferredSendAtLeastOnceMask &= ~msgTypeMask;

	return wasSet;
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
	uartMessageManager.begin();
	delay(100);
	uartMessageManager.handler.sendDeferredMessage(UniformMessage{UniformMessage::Alive{.who = 0xAD328}});
	/*temperatureHumiditySensor.begin();

	if(!temperatureHumiditySensor.isInitialized()){
		Serial.println(F("AHT10 initialization failed"));
	}*/

}


void communicateWithExternalDevices(){
	//messageManager.handle();
	//delay(1);
	while(uartMessageManager.run() != MessageReceptionState::IDLE);
	TemperatureHumiditySensor.communicateWithSensor();
}

