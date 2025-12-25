#include "SchedPWM.h"


#include <string.h>

#include "panic.h"

#define DEBUG_SCHED_PMW

void ScheduledPWM::panicOnStepError(const char *msg, size_t index, void *addr){
	char indexAndAddrStr[40];
	sprintf(indexAndAddrStr, "PANIC! -> (%p):[%u] -> ", addr, index);
	Serial.print(indexAndAddrStr);
	PANIC_RAW(msg);	
}

void ScheduledPWM::setLedPWM(LedID ledId, BrightnessType brightness){
	brightness = minBrightness > brightness ? minBrightness : brightness;
	brightness = maxBrightness < brightness ? maxBrightness : brightness;
	StepNode* previousNode = steps.begin();
	
	StepNode* nodeToInsertAfter;
	if(brightness == 0){
		unassignLed(ledId, previousNode->value.bitStorage);
		nodeToInsertAfter = steps.end();
	}
	else {
		assignLed(ledId, previousNode->value.bitStorage);
		nodeToInsertAfter = nullptr;
	}
	StepNode* currentNode = steps.nextNode(previousNode);
	bool pastInstanceRemoved = false;
	
	while(1){

		if(currentNode == steps.end()){
			if(nodeToInsertAfter == nullptr){
				nodeToInsertAfter = previousNode;
#ifdef DEBUG_SCHED_PMW
				Serial.println("Hint insert after end");
#endif
			}
			break;
		}
		PWMStep& previousStep = previousNode->value;
		PWMStep& currentStep = currentNode->value;
		StepNode* nextNode = steps.nextNode(currentNode);
#ifdef DEBUG_SCHED_PMW
		Serial.print("Input mask: ");
		Serial.print(0x01 << ledId, BIN);
		Serial.print(" Step mask: ");
		Serial.println(currentNode->value.bitStorage, BIN);
#endif
		if(isLedExclusive(ledId, currentStep.bitStorage)){
#ifdef DEBUG_SCHED_PMW
			Serial.println("removing unique");
#endif
			//previousStep.nextIsrTime = currentStep.nextIsrTime;
			currentNode = steps.removeAfter(previousNode);
			continue;
		}
		if(
			!pastInstanceRemoved &&
			!isLedAssigned(ledId, currentStep.bitStorage) && 
			nextNode != steps.end()
		){
#ifdef DEBUG_SCHED_PMW
			Serial.println("removing previous");
#endif
			previousStep.nextIsrTime = currentStep.nextIsrTime;
			currentNode = steps.removeAfter(previousNode);
			pastInstanceRemoved = true;
			continue;
		}
		

		if(previousStep.nextIsrTime < brightness){
			
			assignLed(ledId, currentStep.bitStorage);

		}
		else if(previousStep.nextIsrTime > brightness){
			if(nodeToInsertAfter == nullptr){
				
				nodeToInsertAfter = previousNode;
#ifdef DEBUG_SCHED_PMW
				Serial.print("Hint insert after greater: ");
				Serial.println(previousStep.nextIsrTime);
#endif
			}
			unassignLed(ledId, currentStep.bitStorage);
		}
		else{
			unassignLed(ledId, currentStep.bitStorage);
			nodeToInsertAfter = steps.end();
		}

		previousNode = currentNode;
		currentNode = nextNode;
	}
	if(nodeToInsertAfter != nullptr && nodeToInsertAfter != steps.end()){
		PWMStep& referenceStep = nodeToInsertAfter->value;
		
		StepNode* newStepNode = steps.insertAfter(nodeToInsertAfter, referenceStep);
#ifdef DEBUG_SCHED_PMW
		Serial.println(steps.indexByNode(newStepNode));
#endif
		referenceStep.nextIsrTime = brightness;
		unassignLed(
			ledId,
			newStepNode->value.bitStorage		
		);
	}
	/*


	StepNode* currentNode = steps.begin();

	if(brightness == 0){
		unassignLed(ledId, currentNode->value.bitStorage);
	}
	else {
		assignLed(ledId, currentNode->value.bitStorage);
	}
	
	//StepNode* currentNode = steps.begin();
	currentNode = steps.nextNode(currentNode);;
	StepNode* previousNode = steps.begin();
	StepNode* nodeToInsertAfter = nullptr;
	while(1){
		if(currentNode == steps.end()){
			nodeToInsertAfter = previousNode;
			break;
		}

		PWMStep& currentStep = currentNode->value;
		if(nodeToInsertAfter == nullptr && currentStep.nextIsrTime > brightness){
			nodeToInsertAfter = previousNode;
			unassignLed(ledId, currentStep.bitStorage);
		}
		else if(currentStep.nextIsrTime < brightness) {
			assignLed(ledId, currentStep.bitStorage);
		}
		else {
			unassignLed(ledId, currentStep.bitStorage);
			previousNode = currentNode;
			currentNode = steps.nextNode(currentNode);
			continue;
		}

		if(isLedExclusive(ledId, currentStep.bitStorage)){
			unassignLed(ledId, currentStep.bitStorage);
			currentNode = steps.removeAfter(previousNode);
		}
		else if(!isLedAssigned(ledId, currentStep.bitStorage)){
			break;
		}
		else {
			previousNode = currentNode;
			currentNode = steps.nextNode(currentNode);
		}		
	}
	if(nodeToInsertAfter != nullptr){
		StepNode* referenceNode = nodeToInsertAfter != steps.end() ? nodeToInsertAfter : steps.begin();
		PWMStep& referenceStep = referenceNode->value;
		
		StepNode* newStepNode = steps.insertAfter(nodeToInsertAfter, referenceStep);
		Serial.println(steps.indexByNode(newStepNode));

		referenceStep.nextIsrTime = brightness;
		unassignLed(
			ledId,
			newStepNode->value.bitStorage		
		);
	}
	*/

	


/*
	PWMStep fakeStep{};
	PWMStep* previousStep = &fakeStep;
	//PWMStep* assigmentStep
	PWMStep* finalStep = nullptr;

	PWMStep* lastDimmerStep = &fakeStep;

	
	//assignLed(ledId, currentStep->bitStorage);
	
	
	uint8_t previousIdx;

	bool foundIdenticalStep = false;


	PWMStep& firstStep = steps[0];
	if(brightness == 0){
		unassignLed(ledId, firstStep.bitStorage);
	}
	else {
		assignLed(ledId, firstStep.bitStorage);
	}

	//lastDimmerStep = &firstStep;
	uint8_t stepIdx = firstStep.nextIndex;
	
	while(stepIdx != (LED_COUNT + 1)){
		
		PWMStep& currentStep = steps[stepIdx];


		if(isLedExclusive(ledId, currentStep.bitStorage) && stepIdx != 0){
			stepIdx = deallocateStep(stepIdx, previousIdx);
		}
		else {
			previousIdx = stepIdx;
			stepIdx = currentStep.nextIndex;
		}
		if(previousStep->nextIsrTime < brightness){
			assignLed(ledId, currentStep.bitStorage);
			lastDimmerStep = &currentStep;
			//previousStep = currentStep;
		}
		else if(previousStep->nextIsrTime > brightness){
			unassignLed(ledId, currentStep.bitStorage);
		}
		else{
			unassignLed(ledId, currentStep.bitStorage);
			foundIdenticalStep = false;
		}
		
		previousStep = &currentStep;
	}

	if(foundIdenticalStep){
		return;
	}


	uint8_t newStepIdx = allocateStep();

	
	PWMStep& newStep = steps[newStepIdx];
	newStep.nextIsrTime = lastDimmerStep->nextIsrTime;
	lastDimmerStep->nextIsrTime = brightness;
	newStep.nextIndex = lastDimmerStep->nextIndex;
	lastDimmerStep->nextIndex = newStepIdx;
	newStep.bitStorage |= lastDimmerStep->bitStorage;
	unassignLed(ledId, newStep.bitStorage);

*/
}

bool ScheduledPWM::pwmISR(){
	
	//PANIC("ScheduledPWM::pwmISR");
	if(currentStepNode == steps.begin()){
		onDutyCycleBegin();
	}
	const PWMStep& currentStep = currentStepNode->value;
#ifdef DEBUG_SCHED_PMW
	Serial.print('(');
	Serial.print(size_t(currentStepNode));
	Serial.print(')');
	Serial.print('[');
	Serial.print(steps.indexByNode(currentStepNode));
	Serial.print(']');
	Serial.print(F(" Next: "));
	Serial.print(currentStepNode->nextIndex());
	Serial.print(" LedMask: ");
	Serial.print(currentStep.bitStorage, BIN);
	Serial.print(" Brightness: ");
	Serial.println(currentStep.nextIsrTime);
#endif
	processLedStep(currentStep.bitStorage);
	setupNextIsrTime(currentStep.nextIsrTime);

	

	currentStepNode = steps.nextNode(currentStepNode);
	if(currentStepNode == steps.end()){
		onDutyCycleEnd();
		currentStepNode = steps.begin();
#ifdef DEBUG_SCHED_PMW
		Serial.println();
#endif
		return true;
	}
	
	return false;
	
}


