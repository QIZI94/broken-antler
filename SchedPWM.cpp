#include "SchedPWM.h"


#include <string.h>

#include "panic.h"


void initScheduledPWM(){
	
}

void ScheduledPWM::panicOnStepError(const char *msg, size_t index, void *addr){
	char indexAndAddrStr[40];
	sprintf(indexAndAddrStr, "PANIC! -> (%p):[%u] -> ", addr, index);
	Serial.print(indexAndAddrStr);
	PANIC_RAW(msg);	
}

void ScheduledPWM::setLedPWM(LedID ledId, BrightnessType brightness)
{
	StepNode* previousNode = steps.begin();
	

	if(brightness == 0){
		unassignLed(ledId, previousNode->value.bitStorage);
	}
	else {
		assignLed(ledId, previousNode->value.bitStorage);
	}
	StepNode* currentNode = steps.nextNode(previousNode);

	StepNode* nodeToInsertAfter = nullptr;
	while(1){

		if(currentNode == steps.end()){
			if(nodeToInsertAfter == nullptr){
				nodeToInsertAfter = previousNode;
			}
			break;
		}
		
		PWMStep& currentStep = currentNode->value;
		PWMStep& previousStep = previousNode->value;

		Serial.print("Input mask: ");
		Serial.print(0x01 << ledId, BIN);
		Serial.print("Step mask: ");
		Serial.println(currentNode->value.bitStorage, BIN);
		if(isLedExclusive(ledId, currentStep.bitStorage)){
			if(nodeToInsertAfter == nullptr){
				nodeToInsertAfter = previousNode;
			}
			Serial.println("removing unique");
			previousStep.nextIsrTime = currentStep.nextIsrTime;
			currentNode = steps.removeAfter(previousNode);
			continue;
		}
		
		

		if(previousStep.nextIsrTime < brightness){
			
			assignLed(ledId, currentStep.bitStorage);

		}
		else if(previousStep.nextIsrTime > brightness){
			if(nodeToInsertAfter == nullptr){
				nodeToInsertAfter = previousNode;
			}
			unassignLed(ledId, currentStep.bitStorage);
		}
		else{
			unassignLed(ledId, currentStep.bitStorage);
		}

		previousNode = currentNode;
		currentNode = steps.nextNode(currentNode);
	}
	if(nodeToInsertAfter != nullptr){
		StepNode* referenceNode = nodeToInsertAfter;
		PWMStep& referenceStep = referenceNode->value;
		
		StepNode* newStepNode = steps.insertAfter(nodeToInsertAfter, referenceStep);
		Serial.println(steps.indexByNode(newStepNode));

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

	const PWMStep& currentStep = currentStepNode->value;
	Serial.print("(");
	Serial.print(size_t(currentStepNode));
	Serial.print(")");
	Serial.print("[");
	Serial.print(steps.indexByNode(currentStepNode));
	Serial.print("]");
	Serial.print(" Next: ");
	Serial.print(currentStepNode->nextIndex());
	processLedStep(currentStep.bitStorage);
	setupNextIsrTime(currentStep.nextIsrTime);
	

	currentStepNode = steps.nextNode(currentStepNode);
	if(currentStepNode == steps.end()){
		currentStepNode = steps.begin();
		Serial.println();
		return true;
	}
	
	return false;
	
}


