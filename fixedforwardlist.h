#ifndef FIXED_FORWARD_LIST_H
#define FIXED_FORWARD_LIST_H


#ifndef FIXED_FORWARD_LIST_ERROR_FN
#define FIXED_FORWARD_LIST_ERROR_FN(msg, index, addr)
#endif

#ifndef FIXED_FORWARD_LIST_TRACEBACK_ENTRY
#define FIXED_FORWARD_LIST_TRACEBACK_ENTRY
#endif

#include <inttypes.h>

namespace detail{
template<size_t N, int Tag>
struct FittingUnsignedIntImpl;

template<size_t N>
struct FittingUnsignedInt {
    static constexpr int tag =
        (N <= UINT8_MAX) ? 0 :
        (N <= UINT16_MAX) ? 1 :
        (N <= UINT32_MAX)  ? 2 : 3;

    using type = typename FittingUnsignedIntImpl<N, tag>::type;
};

/* specializations */
template<size_t N>
struct FittingUnsignedIntImpl<N, 0> { using type = uint8_t; };

template<size_t N>
struct FittingUnsignedIntImpl<N, 1> { using type = uint16_t; };

template<size_t N>
struct FittingUnsignedIntImpl<N, 2> { using type = uint32_t; };

template<size_t N>
struct FittingUnsignedIntImpl<N, 3> { using type = uint64_t; };
	
} // detail

template<size_t N, typename T, bool DISABLE_SAFETY_CHECKS = false> 
class FixedForwardList{
public:

	using IndexType = typename detail::FittingUnsignedInt<N>::type;
	using SizeType = IndexType;
	static constexpr SizeType BUFFER_SIZE = N;
	static constexpr SizeType BUFFER_BEGIN_INDEX = 0;
	static constexpr SizeType BUFFER_END_INDEX = BUFFER_SIZE;

	struct Node{
		inline bool isValid() const {
			return !deallocated;
		}

		inline Node* nextNode(){
			return next_ptr;
		}

		inline const Node* nextNode() const {
			return next_ptr;
		}

		T value;
		
		
		
	private:
		Node(){}
		Node(const T& value, Node* next_ptr, bool deallocated) : value(value), next_ptr(next_ptr), deallocated(deallocated) {}
		Node* next_ptr = nullptr;
		bool deallocated = true;

		friend FixedForwardList;
	};

	FixedForwardList(){}
	/*template<SizeType INPUT_ARRAY_SIZE>
	FixedForwardList(const T (&elements)[INPUT_ARRAY_SIZE]){
		Node* last = begin();
		for(const T& element : elements){
			last = insertAfter(last, element);
		}
	}*/


	//FixedForwardList(const T& beginValue) : nodes{Node(beginValue, BUFFER_END_INDEX, false)}, beginIndex(0), unallocatedIndex(1) {}

	inline Node* begin(){
		return begin_ptr;
	}

	inline Node* end(){
		return &nodes[BUFFER_END_INDEX];
	}

	inline Node* beforeBegin(){
		return nullptr;
	}

	inline const Node* cbegin() const {
		return begin_ptr;
	}

	inline const Node* cend() const {
		return &nodes[BUFFER_END_INDEX];
	}

	inline const Node* beforeCBegin() const {
		return nullptr;
	}

	Node* insertAfter(Node* beforeNode, const T& value){
		FIXED_FORWARD_LIST_TRACEBACK_ENTRY
		if constexpr (!DISABLE_SAFETY_CHECKS){		
			if(unallocatedListHead == end()){
				FIXED_FORWARD_LIST_ERROR_FN("ForwardList:insertAfter => OutOfMemory", BUFFER_END_INDEX, unallocatedListHead);
				return end();
			}
		}

		bool isBeforeBegin = beforeNode == beforeBegin();
		if constexpr (!DISABLE_SAFETY_CHECKS){
			if(!isBeforeBegin && (isForeignNode(beforeNode) || beforeNode->deallocated) ){
				FIXED_FORWARD_LIST_ERROR_FN("ForwardList:insertAfter => InvalidInputNode", indexByNode(beforeNode), beforeNode);
				return end();
			}
		}
		
		//IndexType newNodeIndex = allocateNode();
		//Node* newNodePtr = &nodes[newNodeIndex];
		Node* newNode = allocateNode();
		newNode->value = value;
		newNode->deallocated = false;
		if(isBeforeBegin){
			//newNodePtr->next = beginIndex;
			newNode->next_ptr = begin_ptr;
			begin_ptr = newNode;
		}
		else{
			//IndexType nextNodeIndex = beforeNode->next;
			Node* nextNode = beforeNode->next_ptr;
			//beforeNode->next = newNodeIndex;
			beforeNode->next_ptr = newNode;
			//newNodePtr->next = nextNodeIndex;
			newNode->next_ptr = nextNode;
		}
		return newNode;
		//previousNode->nextIndex
		

	}

	Node* removeAfter(Node* beforeNode){
		FIXED_FORWARD_LIST_TRACEBACK_ENTRY

		//IndexType removedNodeIdx;
		Node* removedNode;
		bool isBeforeBegin = beforeNode == beforeBegin();
		
		if constexpr (!DISABLE_SAFETY_CHECKS){
			if(!isBeforeBegin && (isForeignNode(beforeNode) || beforeNode->deallocated)){
				FIXED_FORWARD_LIST_ERROR_FN("ForwardList:removeAfter => InvalidInputNode", indexByNode(beforeNode), beforeNode);
				return beforeBegin();
			}
		}
		
		Node* nextNode;
		if(isBeforeBegin){
			//removedNodeIdx = beginIndex;
			removedNode = begin();
			//beginIndex = nodes[beginIndex].next;
			begin_ptr = nextNode = removedNode->next_ptr;
			
			//nextNode = begin();
		}
		else {
			//removedNodeIdx = beforeNode->next;
			removedNode = beforeNode->next_ptr;
			//beforeNode->next = nodes[removedNodeIdx].next;
			nextNode = beforeNode->next_ptr = removedNode->next_ptr;
			//beforeNode->nextPtr = &nodes[beforeNode->next];
			//nextNode = nodeByIndex(beforeNode->next);
		}
		
		//freeNode(removedNodeIdx);
		freeNode(removedNode);

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


			otherNode = otherNode->nextNode;
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
			node = node->nextNode();
		}
		return true;

	}

	inline SizeType size(){
		return allocatedCount;
	}

	static constexpr SizeType capacity() {
		return BUFFER_SIZE;
	}
//private:
	Node* allocateNode() {
		//IndexType newNodeIdx = unallocatedIndex;
		Node* newNode = unallocatedListHead;
		//IndexType newUnallocatedIndex = nodes[newNodeIdx].next;
		Node* newUnallocatedNode = newNode->next_ptr;
		
		
		//if(newUnallocatedIndex == BUFFER_END_INDEX){
		if(newUnallocatedNode == nullptr){
			//unallocatedIndex++;
			unallocatedListHead++;
		}
		else {
			unallocatedListHead = newUnallocatedNode;
			//unallocatedIndex = newUnallocatedIndex;
		}
		++allocatedCount;

		//return newNodeIdx;
		return newNode;
	}

	void freeNode(Node* node){
		//Node& node = nodes[nodeIdx];
		//node.next = unallocatedIndex;
		node->next_ptr = unallocatedListHead;
		//node.deallocated = true;
		node->deallocated = true;
		//unallocatedIndex = nodeIdx;
		unallocatedListHead = node;
		--allocatedCount;
	}

	bool isForeignNode(const Node* node) const{
		size_t nodeAddr = size_t(node);
		size_t beginAddr = size_t(&nodes[0]);
		size_t endAddr = size_t(end());

		return (nodeAddr < beginAddr || nodeAddr >= endAddr);
	}

	Node nodes[BUFFER_SIZE];
	
	//IndexType beginIndex = BUFFER_END_INDEX;
	Node* begin_ptr = &nodes[BUFFER_END_INDEX];
	//IndexType unallocatedIndex = 0;
	Node* unallocatedListHead = &nodes[BUFFER_BEGIN_INDEX];
	SizeType allocatedCount = 0;

};

#endif