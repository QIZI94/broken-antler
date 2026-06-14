#ifndef EXTERNAL_DEVICES_H
#define EXTERNAL_DEVICES_H

#include <inttypes.h>

#include "timer.h"
#include "uratmessaging.h"

class UARTMessageHandler{
private: // definitions
	using DeferredRepeatMask = uint16_t;
	using DeferredAtLeastOnceMask = uint8_t;
private: //constants
	static constexpr uint8_t DEFERRED_MESSAGES_COUNT = UniformMessage::MESSAGE_TYPES_COUNT - 1;
	static constexpr uint8_t DEFERRED_REPEAT_MASK = 0b11;
	static constexpr uint8_t DEFERRED_SPECIAL_MESSAGE_REPEAT = 2;
	static constexpr uint16_t DEFERRED_REPEAT_TIME_US = 3000;
public: // constants
	static constexpr uint8_t DEFERRED_DEFAULT_REPEAT_COUNT = 3;
	static constexpr uint8_t DEFERRED_SUCCESSFUL_STOP_REPEAT = 0;
public: // interface
	void begin() {}
	void end() {
		messageRepeatLastTime = 0;
		deferredMessageSendAndAckMask = 0x0000;
	}
	MessageReceptionState handleMessagesReception(UARTMessageDriver& driver);
	MessageTransmissionState handleMessagesTransmission(UARTMessageDriver& driver);
public: // member functions
	void sendDeferredMessage(const UniformMessage& message,  uint8_t repeatCount = DEFERRED_DEFAULT_REPEAT_COUNT);
private: // member functions
	UniformMessage requestHandler(UniformMessage::Type msgType);
	void setDeferredRepeatCountMask(UniformMessage::Type msgType, uint8_t attemptCount);
	uint8_t getDeferredRepeatCount(UniformMessage::Type msgType);
	void setDeferredSendAtLeastOnce(UniformMessage::Type msgType);
	bool clearDeferredSendAtLeastOnce(UniformMessage::Type msgType);
private: // member variables
	UniformMessage::MessageData deferredMessages[DEFERRED_MESSAGES_COUNT];
	uint32_t messageRepeatLastTime = 0;
	DeferredRepeatMask deferredMessageSendAndAckMask = 0x0000;
	DeferredAtLeastOnceMask deferredSendAtLeastOnceMask = 0x00; 

};
inline MessageManager<UARTMessageDriver, UARTMessageHandler> uartMessageManager;

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
inline HTU21DTempHumSensor TemperatureHumiditySensor;

extern void initExternalDevices();
extern void communicateWithExternalDevices();

#endif