#include "externaldev.h"
#include "utils/crc.h"
#include "timer.h"

#include <Arduino.h>
#include <Wire.h>


///====== High level UART Communication ======///

constexpr uint8_t arduinoSignature = 0xA3;

MessageReceptionState UARTMessageHandler::handleMessagesReception(UARTMessageDriver &driver) {
	UniformMessage receivedMessage;
	MessageReceptionState messageReceiveState = driver.receiveMessage(receivedMessage);
	if(messageReceiveState == MessageReceptionState::DONE){
		UniformMessage acknowledgeOrResponse = UniformMessage::Acknowledge{.acknowledgedMessage = receivedMessage.type};
		switch(receivedMessage.type){
			case UniformMessage::Type::NONE:
				break;
			
			case UniformMessage::Type::TIME_SYNC:
					setRTC(receivedMessage.data.timeSync.newTime);
					buildMessage(acknowledgeOrResponse, UniformMessage::Type::TIME_SYNC);
					driver.sendMessage(acknowledgeOrResponse);
					acknowledgeHandler(UniformMessage::Type::TIME_SYNC);
					
					break;
			case UniformMessage::Type::ACKNOWLEDGE:
				setDeferredRepeatCountMask(receivedMessage.data.acknowledge.acknowledgedMessage, DEFERRED_SUCCESSFUL_STOP_REPEAT);
				acknowledgeHandler(receivedMessage.data.acknowledge.acknowledgedMessage);
				break;
			case UniformMessage::Type::REQUEST:
				if(buildMessage(acknowledgeOrResponse, receivedMessage.data.request.requestedMessageType)){
					acknowledgeOrResponse.isResponse = true;
				}
			default:
				driver.sendMessage(acknowledgeOrResponse);
				if(receivedMessage.isResponse){
					setDeferredRepeatCountMask(UniformMessage::Type::REQUEST, DEFERRED_SUCCESSFUL_STOP_REPEAT);
				}
				switch (receivedMessage.type){
				
					case UniformMessage::Type::ALIVE:
						//Serial.println("ALIVE");
						setRTC(receivedMessage.data.alive.time);
						initialized = false;
						sendDeferredMessage(UniformMessage::Type::ALIVE);
						break;

					case UniformMessage::Type::LATENCY:
						transmissionLatency = receivedMessage.data.latency.times_us;
						//Serial.print("Lat: ");
						//Serial.println(transmissionLatency);
						initialized = true;
						break;

					case UniformMessage::Type::TIMED_EVENT:
						// restart current animation or something
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
		UniformMessage deferredMessage;
		for(uint8_t messageTypeIndex = 0; messageTypeIndex < DEFERRED_MESSAGES_COUNT; ++messageTypeIndex) {
			if(!clearDeferredSendAtLeastOnce(UniformMessage::Type(messageTypeIndex)) && !shouldRepeat){
				continue;
			}
			uint8_t repeatCount = getDeferredRepeatCount(UniformMessage::Type(messageTypeIndex));
			if(repeatCount == 0x00){
				continue;
			}
			/*if(repeatCount < 3){
				Serial.println("RE");
			}*/

			if(buildMessage(deferredMessage, UniformMessage::Type(messageTypeIndex))){
				setDeferredRepeatCountMask(UniformMessage::Type(messageTypeIndex), --repeatCount);
				
				driver.sendMessage(deferredMessage);
				messageRepeatLastTime = microsNow;
			}
			else {
				// no message filled
				setDeferredRepeatCountMask(UniformMessage::Type(messageTypeIndex), DEFERRED_SUCCESSFUL_STOP_REPEAT);
			}
   		}

	}
	return deferredMessageSendAndAckMask == 0 ? MessageTransmissionState::DONE : MessageTransmissionState::IN_PROGRESS;
}

void UARTMessageHandler::sendDeferredMessage(UniformMessage::Type inType, uint8_t repeatCount) {
	//deferredMessages[uint8_t(message.type)] = message.data;
	setDeferredRepeatCountMask(inType, repeatCount);
	setDeferredSendAtLeastOnce(inType);
}

void UARTMessageHandler::requestDeferredMessage(UniformMessage::Type inType, uint8_t repeatCount) {
	deferredRequestType = inType;
	sendDeferredMessage(UniformMessage::Type::REQUEST, repeatCount);
}

bool UARTMessageHandler::buildMessage(UniformMessage &messageOut, UniformMessage::Type inType) {
	bool assembled = true;
	uint32_t currentRTC = rtcNow();
	switch (inType){
		case UniformMessage::Type::LATENCY:
			messageOut.data.latency.times_us = 0;
			break;
		case UniformMessage::Type::REQUEST:
			if(deferredRequestType != UniformMessage::Type::NONE) {
				messageOut.data.request.requestedMessageType = deferredRequestType;
			}
			else {
				assembled = false;
			}
			break;
		case UniformMessage::Type::ALIVE:
			messageOut.data.alive = UniformMessage::Alive{.who = arduinoSignature, .time = currentRTC};
			break;
		case UniformMessage::Type::TIME_SYNC:
			messageOut.data.timeSync.newTime = currentRTC;
			break;
		case UniformMessage::Type::TIMED_EVENT:
			messageOut.data.timedLedEvent.atTime = currentRTC;
			break;
		
		default:
			assembled = false;
			break;
	}
	if(assembled){
		messageOut.type = inType;
	}
	return assembled;
}

void UARTMessageHandler::acknowledgeHandler(UniformMessage::Type msgType)
{
	(void)msgType;
}

void UARTMessageHandler::setDeferredRepeatCountMask(UniformMessage::Type msgType, uint8_t attemptCount)
{
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
			//uartMessageManager.handler.requestDeferredMessage(UniformMessage::Type::TIME_SYNC);
			uartMessageManager.handler.sendDeferredMessage(UniformMessage::Type::TIMED_EVENT);
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
	uartMessageManager.handler.sendDeferredMessage(UniformMessage::Type::ALIVE);
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

