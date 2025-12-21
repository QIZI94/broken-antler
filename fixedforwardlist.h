#ifndef FIXED_FORWARD_LIST_H
#define FIXED_FORWARD_LIST_H

#include <inttypes.h>


using ErroFn = void (*)(const char* msg, size_t index, void* addr);

namespace detail{
	inline void emptyErrorFn(const char*, size_t, void*){}
}

template<uint64_t N, typename T, typename idx_t = uint8_t, ErroFn ERROR = detail::emptyErrorFn> 
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

	Node* insertAfter(Node* beforeNode, const T& value){
		if(unallocatedIndex >= BUFFER_END_INDEX){
			ERROR("ForwardList:insertAfter:OutOfMemory", unallocatedIndex, &nodes[unallocatedIndex]);
			return end();
		}

		bool isBeforeBegin = beforeNode == end();

		if(isForeignNode(beforeNode) || (!isBeforeBegin && beforeNode->deallocated) ){
			ERROR("ForwardList:insertAfter:InvalidInputNode", indexByNode(beforeNode), beforeNode);
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
		IndexType removedNodeIdx;
		bool isBeforeBegin = beforeNode == end();
		
		
		if(isForeignNode(beforeNode) || (!isBeforeBegin && beforeNode->deallocated)){
			ERROR("ForwardList:removeAfter:InvalidInputNode", indexByNode(beforeNode), beforeNode);
			return;
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

	Node* nodeByIndex(IndexType nodeIdx){
		Node* node = &nodes[nodeIdx];
		if(nodeIdx >= BUFFER_END_INDEX || node->deallocated){
			return end();
		}
		return node;
	}
	IndexType indexByNode(const Node* node) const {
		if(node == end() || isForeignNode(node) || node->deallocated ){
			//ERROR("ForwardList:indexByNode:InvalidInputNode", -1, node);
			return 0xFFFFFFFFFFFFFFFF;
		}
		size_t nodeAddr = size_t(node);
		size_t arrayBeginAddr = size_t(&nodes[0]);
		return size_t(nodeAddr - arrayBeginAddr) / sizeof(Node);
	}

	inline Node* nextNode(const Node* beforeNode){
		return nodeByIndex(beforeNode->next);
	}

	constexpr SizeType capacity() {
		return BUFFER_SIZE;
	}

	IndexType allocateNode() {
		IndexType newNodeIdx = unallocatedIndex;
		IndexType newUnallocatedIndex = nodes[newNodeIdx].next;
		
		if(newUnallocatedIndex == BUFFER_END_INDEX){
			unallocatedIndex++;
		}
		else {
			unallocatedIndex = newUnallocatedIndex;
		}

		return newNodeIdx;
	}

	void freeNode(IndexType nodeIdx){
		Node& node = nodes[nodeIdx];
		node.next = unallocatedIndex;
		node.deallocated = true;
		unallocatedIndex = nodeIdx;
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

};

#endif