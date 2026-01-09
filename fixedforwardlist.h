#ifndef FIXED_FORWARD_LIST_H
#define FIXED_FORWARD_LIST_H

#ifndef ERROR_MSG_LITERAL
#define ERROR_MSG_LITERAL(literal_str) nullptr
#endif

#ifndef FIXED_FORWARD_LIST_ERROR_FN
#define FIXED_FORWARD_LIST_ERROR_FN(msg, index, addr)
#endif

#ifndef FIXED_FORWARD_LIST_TRACEBACK_ENTRY
#define FIXED_FORWARD_LIST_TRACEBACK_ENTRY
#endif

#include <inttypes.h>

template<uint64_t N, typename T, typename idx_t = uint8_t> 
class FixedForwardList{
public:

	using IndexType = idx_t;
	using SizeType = IndexType;
	static constexpr SizeType BUFFER_SIZE = N;
	static constexpr SizeType BUFFER_END_INDEX = BUFFER_SIZE;

	struct Node{
		inline IndexType nextIndex() const {
			return next;
		}
		inline bool isValid() const {
			return !deallocated;
		}
		T value;
	private:
		Node(){}
		Node(const T& value, IndexType nextIndex, bool deallocated) : value(value), next(nextIndex), deallocated(deallocated) {}
		
		IndexType next = BUFFER_END_INDEX;
		bool deallocated = true;

		friend FixedForwardList;
	};

	FixedForwardList(){}
	template<SizeType INPUT_ARRAY_SIZE>
	FixedForwardList(const T (&elements)[INPUT_ARRAY_SIZE]){
		Node* last = begin();
		for(const T& element : elements){
			last = insertAfter(last, element);
		}
	}


	FixedForwardList(const T& beginValue) : nodes{Node(beginValue, BUFFER_END_INDEX, false)}, beginIndex(0), unallocatedIndex(1) {}

	inline Node* begin(){
		return &nodes[beginIndex];
	}

	inline Node* end(){
		return &nodes[BUFFER_END_INDEX];
	}

	inline Node* beforeBegin(){
		return nullptr;
	}

	inline const Node* cbegin() const {
		return &nodes[beginIndex];
	}

	inline const Node* cend() const {
		return &nodes[BUFFER_END_INDEX];
	}

	inline const Node* beforeCBegin() const {
		return end();
	}

	Node* insertAfter(Node* beforeNode, const T& value){
		FIXED_FORWARD_LIST_TRACEBACK_ENTRY
		if(unallocatedIndex >= BUFFER_END_INDEX){
			FIXED_FORWARD_LIST_ERROR_FN("ForwardList:insertAfter => OutOfMemory", unallocatedIndex, &nodes[unallocatedIndex]);
			return end();
		}

		bool isBeforeBegin = beforeNode == beforeBegin();

		if(!isBeforeBegin && (isForeignNode(beforeNode) || beforeNode->deallocated) ){
			FIXED_FORWARD_LIST_ERROR_FN("ForwardList:insertAfter => InvalidInputNode", indexByNode(beforeNode), beforeNode);
			return end();
		}
		
		IndexType newNodeIndex = allocateNode();

		Node* newNodePtr = &nodes[newNodeIndex];
		newNodePtr->value = value;
		newNodePtr->deallocated = false;
		if(isBeforeBegin){
			newNodePtr->next = beginIndex;
			beginIndex = newNodeIndex;
		}
		else{
			IndexType nextNodeIndex = beforeNode->next;
			beforeNode->next = newNodeIndex;
			newNodePtr->next = nextNodeIndex;
		}
		return newNodePtr;
		//previousNode->nextIndex
		

	}

	Node* removeAfter(Node* beforeNode){
		FIXED_FORWARD_LIST_TRACEBACK_ENTRY
		IndexType removedNodeIdx;
		bool isBeforeBegin = beforeNode == beforeBegin();
		
		
		if(!isBeforeBegin && (isForeignNode(beforeNode) || beforeNode->deallocated)){
			FIXED_FORWARD_LIST_ERROR_FN("ForwardList:removeAfter => InvalidInputNode", indexByNode(beforeNode), beforeNode);
			return beforeBegin();
		}
		
		Node* nextNode;
		if(isBeforeBegin){
			removedNodeIdx = beginIndex;
			beginIndex = nodes[beginIndex].next;
			nextNode = begin();
		}
		else {
			removedNodeIdx = beforeNode->next;
			beforeNode->next = nodes[removedNodeIdx].next;
			nextNode = nodeByIndex(beforeNode->next);
		}
		
		freeNode(removedNodeIdx);

		return nextNode;
	}

	void clearAfter(Node* beforeNode){
		FIXED_FORWARD_LIST_TRACEBACK_ENTRY
		while(removeAfter(beforeNode) != end());
	}

	void clear() {
		FIXED_FORWARD_LIST_TRACEBACK_ENTRY
		clearAfter(beforeBegin());
	}

	Node* nodeByIndex(IndexType nodeIdx){
		Node* node;
		if(nodeIdx == BUFFER_END_INDEX){
			//_FIXED_FORWARD_LIST_ERROR("ForwardList:nodeByIndex:OutOfRange", nodeIdx, &nodes[nodeIdx]);
			node = end();
		}
		else if(nodeIdx > BUFFER_END_INDEX){
			node = nullptr;
		}
		else {
			node = &nodes[nodeIdx];
			if(node->deallocated){
				node = nullptr;
			}
		}
		return node;
	}


	IndexType indexByNode(const Node* node) const {
		if(node == beforeBegin() || isForeignNode(node) || node->deallocated ){
			//ERROR("ForwardList:indexByNode:InvalidInputNode", -1, node);
			return 0xFFFFFFFFFFFFFFFF;
		}
		size_t nodeAddr = size_t(node);
		size_t arrayBeginAddr = size_t(&nodes[0]);
		return size_t(nodeAddr - arrayBeginAddr) / sizeof(Node);
	}

	inline Node* nextNode(Node* beforeNode){
		return nodeByIndex(beforeNode->next);
	}
	inline const Node* nextNode(const Node* beforeNode) const{
		return nodeByIndex(beforeNode->next);
	}
	template<typename Callable>
	constexpr bool isEqual(const FixedForwardList& other, Callable callable) const {
		const Node* otherNode = other.cbegin();
		const Node* node = cbegin();

		while(node != end() && otherNode != other.end()){

			if(node == end() || otherNode == other.end()){
				return false;
			}

			if(!callable(node->value, otherNode->value)){
				return false;
			}


			otherNode = other.nextNode(otherNode);
			node = nextNode(node);
		}
		return true;

	}

	template<size_t OTHER_N, typename Callable>
	constexpr bool isEqual(const T (&other)[OTHER_N], Callable callable) const {
		const T* otherIt = &other[0];
		const T* otherEnd = &other[OTHER_N];

		const Node* node = cbegin();

		while(node != cend() && otherIt != otherEnd){

			if(node == end() || otherIt == otherEnd){
				return false;
			}

			if(!callable(node->value, *otherIt)){
				return false;
			}


			++otherIt;
			node = nextNode(node);
		}
		return true;

	}

	inline SizeType size(){
		return allocatedCount;
	}

	static constexpr SizeType capacity() {
		return BUFFER_SIZE;
	}
private:
	IndexType allocateNode() {
		IndexType newNodeIdx = unallocatedIndex;
		IndexType newUnallocatedIndex = nodes[newNodeIdx].next;
		
		if(newUnallocatedIndex == BUFFER_END_INDEX){
			unallocatedIndex++;
		}
		else {
			unallocatedIndex = newUnallocatedIndex;
		}
		++allocatedCount;

		return newNodeIdx;
	}

	void freeNode(IndexType nodeIdx){
		Node& node = nodes[nodeIdx];
		node.next = unallocatedIndex;
		node.deallocated = true;
		unallocatedIndex = nodeIdx;
		--allocatedCount;
	}

	bool isForeignNode(const Node* node) const{
		size_t nodeAddr = size_t(node);
		size_t beginAddr = size_t(&nodes[0]);
		size_t endAddr = size_t(end());

		return (nodeAddr < beginAddr || nodeAddr > endAddr);
	}

	Node nodes[BUFFER_SIZE];
	
	IndexType beginIndex = BUFFER_END_INDEX;
	IndexType unallocatedIndex = 0;
	SizeType allocatedCount = 0;

};

#endif