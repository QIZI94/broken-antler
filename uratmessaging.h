#ifndef UART_MESSAGING_H
#define UART_MESSAGING_H
#include <inttypes.h>

#include "messagingdefs.h"

#define packet_struct struct __attribute__((packed))
packet_struct  UniformMessage{
	
	enum class Type : uint8_t{
		ACKNOWLEDGE,
		REQUEST,
		ALIVE,
		TIME_SYNC,

		NONE,

	};
	static constexpr uint8_t MESSAGE_TYPES_COUNT = uint8_t(Type::NONE) + 1;


	packet_struct None{};
	
	packet_struct Request{
		Type requestedMessageType;
	};
	packet_struct Alive{
		uint32_t who;
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
		Acknowledge acknowledge;

		uint8_t bytes[5];
	};

	constexpr UniformMessage() : data{.none = {}}, isResponse(false), type(Type::NONE){}
	constexpr UniformMessage(const MessageData& messageData, Type messageType, bool isResponse = false) : data(messageData), isResponse(isResponse), type(messageType){}
	constexpr UniformMessage(const Request& request, bool isResponse = false) : data{.request = request}, isResponse(isResponse), type(Type::REQUEST){}
	constexpr UniformMessage(const Alive& alive, bool isResponse = false) : data{.alive = alive}, isResponse(isResponse), type(Type::ALIVE){}
	constexpr UniformMessage(const TimeSync& timeSync, bool isResponse = false) : data{.timeSync = timeSync}, isResponse(isResponse), type(Type::TIME_SYNC){}
	constexpr UniformMessage(const Acknowledge& acknowledge, bool isResponse = false) : data{.acknowledge = acknowledge}, isResponse(isResponse), type(Type::ACKNOWLEDGE){}
	
	MessageData  data;
	struct{
		uint8_t isResponse	: 4;
		UniformMessage::Type type	: 4;
	};
};

class UARTMessageDriver {
private: // definitions
	union MessageFrame{
		struct Data{
			static constexpr uint8_t SYNC_FLAG = 0x9B;
			// header
			uint8_t sync;
			// message type + data + terminator
			uint8_t data[sizeof(UniformMessage)];
			uint8_t crc[2];
		};

		Data asFrame;
		uint8_t asBuffer[sizeof(Data)];
	};
public: // constants
	static constexpr uint32_t RECEPTION_TIMEOUT_US = 1000;
public: // interface
	void begin() {}
	void end(){
		receptionTimeoutLastTime = 0;
		receptionBufferIndex = 0;
	}
	MessageTransmissionState sendMessage(const UniformMessage& messageIn);
	MessageReceptionState receiveMessage(UniformMessage& messageOut);
private: // member functions
	bool validateMessage(const MessageFrame::Data& uniformMessageData);
private: // member variables
	MessageFrame receptionBuffer{};
	uint32_t receptionTimeoutLastTime = 0;
	uint8_t receptionBufferIndex = 0;
};
#endif