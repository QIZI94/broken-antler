#ifndef EXTERNAL_DEVICES_H
#define EXTERNAL_DEVICES_H

#include <inttypes.h>

#include "timer.h"

#define packet_struct struct __attribute__((packed))
inline constexpr uint16_t ACKNOWLEDGE_DATA = 0xBBCC;
packet_struct  Message{
	
	enum class Type : uint8_t{
		NONE,
		REQUEST,
		ALIVE,
		TIME_SYNC,

		ACKNOWLEDGE

	};


	packet_struct None{};
	
	packet_struct Request{
		Type requestedMessageType;
		uint32_t atTime;
	};
	packet_struct Alive{
		uint32_t address;
	};

	packet_struct TimeSync{
		uint32_t newTime;
	};

	packet_struct Acknowledge{
		Type acknowledgedMessage;
	};


	
	
	union MessageData{
		None none;
		Alive alive;
		Request  request;
		TimeSync timeSync;

		uint8_t bytes[5];
	};

	constexpr Message() : data{.none = {}}, type(Type::NONE){}
	constexpr Message(const MessageData& messageData, Type messageType) : data(messageData), type(messageType){}
	constexpr Message(const Request& request) : data{.request = request}, type(Type::REQUEST){}
	constexpr Message(const Alive& alive) : data{.alive = alive}, type(Type::ALIVE){}
	constexpr Message(const TimeSync& timeSync) : data{.timeSync = timeSync}, type(Type::TIME_SYNC){}
	
	MessageData  data;
	Message::Type type;
};
enum class MessageReceiveState{
	IDLE,
	DONE,
	IN_PROGRESS,
	TIMED_OUT
};

MessageReceiveState receiveMessageUART(Message& messageOut);
void sendMessageUART(const Message &messageIn);




template<Message (*HandleRequestFunc)(Message::Type), void (*HandleMessageFunc)(const Message&)>
class UARTMessageManager{
private: // definitions

public: // 

	
	void sendDeferred(const Message &messageIn);

	bool handle() {
		Message receiveMessage{.data = {.none = Message::None{}}, .type = Message::Type::NONE};
		//Serial.print("peek: ");
		//Serial.println(Serial1.peek(), HEX);
		MessageReceiveState messageReceiveState = receiveMessageUART(receiveMessage);
		//Serial.print("State: ");
		//Serial.println(int(messageReceiveState));
		switch (messageReceiveState)
		{
		case MessageReceiveState::DONE:
			
			switch (receiveMessage.type)
			{
				case Message::Type::REQUEST:
					if(HandleRequestFunc != nullptr){
						Message messageToSendBack = HandleRequestFunc(receiveMessage.data.request.requestedMessageType);
						sendMessageUART(messageToSendBack);
					}
					break;
			
				default:
					if(HandleMessageFunc != nullptr){
						HandleMessageFunc(receiveMessage);
					}
					break;
			}
			return false;
		case MessageReceiveState::IDLE:

			return false;
		default:
			return true;
		}
	}

	
};



extern Message handleRequestFunc(Message::Type type);


inline UARTMessageManager<handleRequestFunc, nullptr> MessageManager;



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