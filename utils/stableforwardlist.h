#ifndef STABLE_FORWARD_LIST_H
#define STABLE_FORWARD_LIST_H
#include <inttypes.h>

#include "typehelpers.h"

namespace utils {
template<size_t N, typename T, bool INCLUDE_PREVIOUS_LINK, bool DISABLE_SAFETY_CHECKS = false>
class StableLinkedListBase{
public: // definitions
	using IndexType = typename utils::FittingUnsignedInt<N>::type;
	using SizeType = IndexType;
public: // constants
	static constexpr bool IS_LINKED_LIST  = INCLUDE_PREVIOUS_LINK;
	static constexpr bool IS_FORWARD_LIST = !IS_LINKED_LIST;
	static constexpr SizeType BUFFER_SIZE = N;
	static constexpr SizeType BUFFER_BEGIN_INDEX = 0;
	static constexpr SizeType BUFFER_END_INDEX = BUFFER_SIZE;
	static constexpr IndexType INVALID_INDEX = BUFFER_END_INDEX;

public: // definitions
	
	struct DefaultClear {
    	void operator()(T&) const noexcept {}
	};

	struct Node{
		inline bool isLinked() const {
			return next_ptr != nullptr;
		}

		inline Node* nextNode(){
			return next_ptr;
		}

		inline const Node* nextNode() const {
			return next_ptr;
		}
		
		T value{};		
	private:		
		constexpr Node() {}
		constexpr Node(const T& value, Node* next_ptr) : value(value), next_ptr(next_ptr) {}
		Node* next_ptr = nullptr;


		friend StableLinkedListBase;
	};

	template<typename T_NodeType>
	class NodeIterator
	{
	public:
		using value_type = T_NodeType;
		using pointer = T_NodeType*;
		using reference = T_NodeType&;

		constexpr NodeIterator() : node_ptr(nullptr) {}
		constexpr NodeIterator(const NodeIterator<typename remove_const<value_type>::type>& other) : node_ptr(other.node_ptr) {}
		constexpr explicit NodeIterator(typename copy_const<value_type, Node>::type* node_ptr) : node_ptr(node_ptr) {}

		// dereference
		constexpr reference operator*() const
		{
			return node_ptr->value;
		}

		constexpr pointer operator->() const
		{
			return &node_ptr->value;
		}
		constexpr reference operator*()
		{
			return node_ptr->value;
		}

		constexpr pointer operator->()
		{
			return &node_ptr->value;
		}


		// pre-increment
		NodeIterator& operator++()
		{
			node_ptr = node_ptr->next_ptr;
			return *this;
		}

		// post-increment
		NodeIterator operator++(int)
		{
			NodeIterator tmp(*this);
			node_ptr = node_ptr->next_ptr;
			return tmp;
		}

		// comparison
		constexpr bool operator==(const NodeIterator& other) const
		{
			return node_ptr == other.node_ptr;
		}

		constexpr bool operator!=(const NodeIterator& other) const
		{
			return node_ptr != other.node_ptr;
		}

		constexpr Node* asNode() const {
			return node_ptr;
		}

	protected:
		Node* node_ptr;
		friend StableLinkedListBase;
	};
	


	using Nodes = Node[BUFFER_SIZE];

	// Standard container typedefs
    using value_type             = T;
    using size_type              = SizeType;
    using difference_type        = ptrdiff_t;

    using reference              = T&;
    using const_reference        = const T&;

    using pointer                = T*;
    using const_pointer          = const T*;

    using iterator               = NodeIterator<T>;
    using const_iterator         = NodeIterator<const T>;

	
	public: // member functions
	StableLinkedListBase(){}

	inline iterator begin(){
		return begin_ptr;
	}
	inline iterator end(){
		return iterator(&nodes[BUFFER_END_INDEX]);
	}


	static inline constexpr iterator beforeBegin(){
		return iterator(nullptr);
	}
	inline iterator beforeEnd(){
		return before_end;
	}

	inline const_iterator cbegin() const {
		return const_iterator(begin_ptr.node_ptr);
	}
	
	inline const const_iterator cend() const {
		return const_iterator(&nodes[BUFFER_END_INDEX]);
	}

	inline const const_iterator beforeCBegin() const {
		return const_iterator(nullptr);
	}
	inline const_iterator beforeEnd() const{
		return before_end;
	}

	inline bool link_after(iterator beforeIt, iterator linkedIt){
		iterator endIt = end();
		if(!DISABLE_SAFETY_CHECKS){
			if(isInvalidNode(linkedIt)){
				return false;
			}
		}
		
		if(beforeIt != beforeBegin()){
			if(!DISABLE_SAFETY_CHECKS){
				if(isInvalidNode(beforeIt)){
					return false;
				}
				if(beforeIt.asNode()->isLinked() == false){
					return false;
				}
				if(linkedIt.asNode()->isLinked()){
					unlink(linkedIt);
				}
			}

			Node* nextNode = linkedIt.asNode()->next_ptr = beforeIt.asNode()->next_ptr;
			beforeIt.asNode()->next_ptr = linkedIt.asNode();

			//IndexType linkedIndex = indexByNodeUnchecked(linkedIt.asNode());

			IndexType linkedIndex = indexByNodeUnchecked(linkedIt.asNode());

			if constexpr(IS_LINKED_LIST){
				if(nextNode != endIt.asNode()){
					IndexType nextIndex = indexByNodeUnchecked(nextNode);
					previousNodeByIndex[linkedIndex] = previousNodeByIndex[nextIndex];
					previousNodeByIndex[nextIndex] = linkedIndex;
				}
				else {
					previousNodeByIndex[linkedIndex] = indexByNodeUnchecked(beforeIt.asNode());
					before_end = linkedIt;
				}
			}
			else {
				if(nextNode == endIt.asNode()){
					before_end = linkedIt;
				}
			}
		}
		else {
			if(!DISABLE_SAFETY_CHECKS){
				if(linkedIt.asNode()->isLinked()){
					unlink(linkedIt);
				}
			}
			Node* nextNode = linkedIt.asNode()->next_ptr = begin_ptr.asNode();
			begin_ptr = linkedIt;
			if constexpr(IS_LINKED_LIST){
				if(nextNode != endIt.asNode()){
					previousNodeByIndex[indexByNodeUnchecked(linkedIt.asNode())] = previousNodeByIndex[indexByNodeUnchecked(nextNode)];
				}
				else {
					before_end = linkedIt;
				}
			}
			else{
				if(nextNode == endIt.asNode()){
					before_end = linkedIt;
				}
			}
		}
		++linkedCount;
		return true;
		
	}

	inline iterator unlink_after(iterator beforeIt){
		iterator endIt = end();
		iterator nextIt;
		if(beforeIt != beforeBegin()){
			if(!DISABLE_SAFETY_CHECKS){
				if(isInvalidNode(beforeIt)){
					return endIt;
				}
			}
			iterator linkedIt = iterator(beforeIt.asNode()->next_ptr);
			if(linkedIt == endIt){
				return endIt;
			}
			
			Node* nextNode = beforeIt.asNode()->next_ptr = linkedIt.asNode()->next_ptr;
			nextIt = iterator(nextNode);
			//Node* nextNode = linkedIt.asNode()->next_ptr;
			if constexpr(IS_LINKED_LIST){
				IndexType linkedIndex = indexByNodeUnchecked(linkedIt.asNode());
				if(nextNode != endIt.asNode()){
					IndexType nextIndex = indexByNodeUnchecked(nextNode);
					previousNodeByIndex[nextIndex] = previousNodeByIndex[linkedIndex];
				}
				else {
					before_end = beforeIt;
				}
				previousNodeByIndex[linkedIndex] = INVALID_INDEX;
			}
			else {
				if(nextNode == endIt.asNode()){
					before_end = beforeIt;
				}
			}
			

			linkedIt.asNode()->next_ptr = nullptr;
		}
		else {
			iterator linkedIt = begin_ptr;
			if(linkedIt == endIt){
				return endIt;
			}
			nextIt = begin_ptr = iterator(linkedIt.asNode()->next_ptr);
			if constexpr(IS_LINKED_LIST){
				if(nextIt != endIt){
					IndexType nextIndex = indexByNodeUnchecked(nextIt.asNode());
					previousNodeByIndex[nextIndex] = INVALID_INDEX;
				}
				else {
					before_end = begin_ptr;
				}
			}
			else {
				if(nextIt == endIt){
					before_end = begin_ptr;
				}
			}

			linkedIt.asNode()->next_ptr = nullptr;
		}
		
		
		--linkedCount;
		return nextIt;
		//unlink_after()
	}
	iterator unlink(iterator linkedIt){
		iterator endIt = end();
		if constexpr(IS_LINKED_LIST){
			IndexType linkedIndex = indexByIterator(linkedIt);
			if(linkedIndex == INVALID_INDEX){
				return endIt;
			}
			IndexType perviousIndex = previousNodeByIndex[linkedIndex];
			if(perviousIndex == INVALID_INDEX){
				return endIt;
			}
			unlink_after(perviousIndex == INVALID_INDEX ? begin() : iterator(&nodes[perviousIndex]));
		}
		else {
			
			if(isInvalidNode(linkedIt)){
				return endIt;
			}
			if(!linkedIt.asNode()->isLinked()){
				return endIt;
			}
			
			
			for(iterator beforeIt = begin(); beforeIt != endIt; ++beforeIt){
				if(beforeIt.asNode()->next_ptr == linkedIt.asNode()){
					return unlink_after(beforeIt);
				}
			}
		}
		
		return endIt;

	}

	template<typename CallableClear = DefaultClear>
	void clearAfter(iterator beforeIt, CallableClear customClear = {}){
		iterator endIt = end();
		iterator removeIt = prepareClear(beforeIt);
				
		while(removeIt != endIt){
			customClear(*removeIt);
			(removeIt++).asNode()->next_ptr = nullptr;
			--linkedCount;
		}
	}

	template<typename CallableClear = DefaultClear>
	void clear(CallableClear customClear = {}){
		clearAfter(beforeBegin(), customClear);
	}



	//template<typename = utils::enable_if_t<IS_LINKED_LIST>>
	iterator previousIterator(IndexType linkedIndex){
		if(linkedIndex < BUFFER_SIZE){
			return end();
		}
		Node* linkedNode = &nodes[linkedIndex];
		if(linkedNode->isLinked()){
			return end();
		}
		if constexpr(IS_LINKED_LIST){
					
			return previousNodeUnchecked(linkedIndex);
		}
		else {
			return previousNodeUnchecked(iterator(linkedNode));
		}
	}
	
	iterator previousIterator(iterator linkedIt){
		if constexpr(IS_LINKED_LIST){
			IndexType linkedIndex = indexByIterator(linkedIt);
			return linkedIndex != INVALID_INDEX ? previousNodeUnchecked(linkedIndex) : end();
		}
		else {
			return isInvalidNode(linkedIt) ? end() : previousNodeUnchecked(linkedIt);
		}
	}


	IndexType indexByIterator(const_iterator it) const {
		if(isInvalidNode(it)){
			return INVALID_INDEX;
		}
		return indexByNodeUnchecked(it.asNode());
		
	}

	iterator iteratorByIndex(IndexType nodeIdx){
		if(nodeIdx >= BUFFER_END_INDEX){
			return end();
		}
		return iterator(&nodes[nodeIdx]);
	}
	const_iterator iteratorByIndex(IndexType nodeIdx) const {
		if(nodeIdx >= BUFFER_END_INDEX){
			return cend();
		}
		return const_iterator(&nodes[nodeIdx]);
	}

	const Nodes& getNodesRaw(){
		return nodes;
	}

	SizeType size() {
		return linkedCount;
	}
	static constexpr SizeType capacity() { return BUFFER_SIZE; }

	public: // operators
	iterator operator[](IndexType index) {
		return iteratorByIndex(index);
	}
	const_iterator operator[](IndexType index) const {
		return iteratorByIndex(index);
	}
	protected: // member functions
	IndexType indexByNodeUnchecked(const Node* node) const {
		return node - &nodes[0];
	}
	iterator previousNodeUnchecked(IndexType linkedIndex){
		if constexpr (IS_LINKED_LIST){
			IndexType previousIndex = previousNodeByIndex[linkedIndex];
			
			return previousIndex != INVALID_INDEX ? iterator(&nodes[previousIndex]) : beforeBegin();
		}
		else {
			return previousNodeUnchecked(&nodes[linkedIndex]);
		}
	}
	iterator previousNodeUnchecked(iterator linkedIt){
		if constexpr(IS_LINKED_LIST){
			return previousNodeUnchecked(indexByNodeUnchecked(linkedIt));
		}
		else {
			if(linkedIt == begin()){
				return beforeBegin();
			}
			iterator endIt = end();
			
			for(iterator beforeIt = begin(); beforeIt != endIt; ++beforeIt){
				if(beforeIt.asNode()->next_ptr == linkedIt.asNode()){
					return beforeIt;
				}
			}
			return endIt;
		}
		
	}
	bool isInvalidNode(const_iterator nodeIt) const{
		return nodeIt.asNode() < &nodes[0] || nodeIt.asNode() >= &nodes[BUFFER_END_INDEX];
	}

	iterator prepareClear(iterator beforeIt){
		iterator beginIt = begin();
		iterator endIt = end();
		iterator removeIt;
		if(beginIt == endIt){
			removeIt = endIt;
		}
		else if(beforeIt == beforeBegin()){
			removeIt = beginIt;
			begin_ptr = endIt;
		}
		else {
			removeIt = iterator(beforeIt.asNode()->next_ptr);
			beforeIt.asNode()->next_ptr = end().asNode();
		}
		return removeIt;
	}


	private: // member varuabkes
	Nodes nodes;
	iterator begin_ptr = iterator(&nodes[BUFFER_END_INDEX]);
	iterator before_end = iterator(beforeBegin());
	SizeType linkedCount = 0;
	InitializedArrayWrapper<IS_LINKED_LIST ? BUFFER_SIZE : 0, IndexType> previousNodeByIndex{INVALID_INDEX};
	
	
};
template<size_t N, typename T, bool DISABLE_SAFETY_CHECKS = false>
using StableForwardList = StableLinkedListBase<N, T, false, DISABLE_SAFETY_CHECKS>;
template<size_t N, typename T, bool DISABLE_SAFETY_CHECKS = false>
using StableLinkedList = StableLinkedListBase<N, T, true, DISABLE_SAFETY_CHECKS>;


}// utils




#endif