#ifndef STABLE_FORWARD_LIST_H
#define STABLE_FORWARD_LIST_H
#include <inttypes.h>

#include "typehelpers.h"

namespace utils {
class StableForwardList{
	using T = uint16_t;
	static constexpr size_t N = 12;
public: // definitions
	using IndexType = typename utils::FittingUnsignedInt<N>::type;
	using SizeType = IndexType;
public: // constants
	static constexpr IndexType INVALID_INDEX = (utils::NumericLimits<IndexType>::max)();
	static constexpr SizeType BUFFER_SIZE = N;
	static constexpr SizeType BUFFER_BEGIN_INDEX = 0;
	static constexpr SizeType BUFFER_END_INDEX = BUFFER_SIZE;

public: // definitions
	
	

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
		
		T value = ForceNoInit<T>{};		
	private:		
		constexpr Node() {}
		constexpr Node(const T& value, Node* next_ptr) : value(value), next_ptr(next_ptr) {}
		Node* next_ptr = nullptr;


		friend StableForwardList;
	};

	template<typename T>
	class NodeIterator
	{
	public:
		using value_type = T;
		using pointer = T*;
		using reference = T&;

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
		friend StableForwardList;
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
	StableForwardList(){}

	inline iterator begin(){
		return begin_ptr;
	}
	inline iterator end(){
		return iterator(&nodes[BUFFER_END_INDEX]);
	}


	static inline constexpr iterator beforeBegin(){
		return iterator(nullptr);
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

	inline Node* link_after_unchecked(Node* beforeNode, Node* linkedNode){
		linkedNode->next_ptr = beforeNode->next_ptr;
		beforeNode->next_ptr = linkedNode;
		return beforeNode;
	}
	
	inline bool link_after(iterator beforeIt, iterator linkedIt){
		iterator endIt = end();
		if(isInvalidNode(linkedIt.asNode())){
			return false;
		}
		bool isLinked = linkedIt.asNode()->isLinked();
		
		if(beforeIt != beforeBegin()){
			if(isInvalidNode(beforeIt.asNode())){
				return false;
			}
			if(beforeIt.asNode()->isLinked() == false){
				return false;
			}
			if(isLinked){
				unlink(linkedIt);
			}

			Node* nextNode = linkedIt.asNode()->next_ptr = beforeIt.asNode()->next_ptr;
			beforeIt.asNode()->next_ptr = linkedIt.asNode();

			//IndexType linkedIndex = indexByNodeUnchecked(linkedIt.asNode());

			IndexType linkedIndex = indexByNodeUnchecked(linkedIt.asNode());


			if(nextNode != endIt.asNode()){
				IndexType nextIndex = indexByNodeUnchecked(nextNode);
				previousNodeByIndex[linkedIndex] = previousNodeByIndex[nextIndex];
				previousNodeByIndex[nextIndex] = linkedIndex;
			}
			else {
				previousNodeByIndex[linkedIndex] = indexByNodeUnchecked(beforeIt.asNode());
			}
		}
		else {
			if(isLinked){
				unlink(linkedIt);
			}
			Node* nextNode = linkedIt.asNode()->next_ptr = begin_ptr.asNode();
			begin_ptr = linkedIt;
			if(nextNode != endIt.asNode()){
				previousNodeByIndex[indexByNodeUnchecked(linkedIt.asNode())] = previousNodeByIndex[indexByNodeUnchecked(nextNode)];
			}
		}

		return true;
		

		// basically relink
		

		
		/*if(
			beforeIt == linkedIt          ||
			linkedIt.asNode()->isLinked() ||
			isForeignNode(beforeIt)       ||
			isForeignNode(linkedIt)       
		){
			return end();
		}
		*/
		//return iterator(link_after_unchecked(beforeIt.asNode(), linkedIt.asNode()));
	}

	inline bool unlink_after(iterator beforeIt, iterator linkedIt){
		iterator endIt = end();
		if(linkedIt == beforeBegin() || linkedIt == endIt){
			return false;
		}
		
		if(beforeIt != beforeBegin()){
			if(isInvalidNode(beforeIt.asNode())){
				return false;
			}
			if(beforeIt.asNode()->next_ptr != linkedIt.asNode()){
				return false;
			}
			
			
			Node* nextNode = beforeIt.asNode()->next_ptr = linkedIt.asNode()->next_ptr;
			
			//Node* nextNode = linkedIt.asNode()->next_ptr;
			IndexType linkedIndex = indexByNodeUnchecked(linkedIt.asNode());
			if(nextNode != endIt.asNode()){
				IndexType nextIndex = indexByNodeUnchecked(nextNode);
				previousNodeByIndex[nextIndex] = previousNodeByIndex[linkedIndex];
			};
			linkedIt.asNode()->next_ptr = nullptr;
			previousNodeByIndex[linkedIndex] = INVALID_INDEX;
		}
		else {
			if(begin() != linkedIt){
				return false;
			}
			iterator nextIt = begin_ptr = iterator(linkedIt.asNode()->next_ptr);
			if(nextIt != endIt){
				IndexType nextIndex = indexByNodeUnchecked(nextIt.asNode());
				previousNodeByIndex[nextIndex] = INVALID_INDEX;
			}

			linkedIt.asNode()->next_ptr = nullptr;
		}
		
		

		return true;
		//unlink_after()
	}

	bool unlink(iterator linkedIt){
		IndexType linkedIndex = indexByNode(linkedIt);
		if(linkedIndex == INVALID_INDEX){
			return false;
		}
		unlink_after(previousNodeUnchecked(linkedIndex), linkedIt);
		return true;

	}

	iterator previousNodeUnchecked(IndexType linkedIndex){
		return iterator(&nodes[previousNodeByIndex[linkedIndex]]);
	}

	iterator previousNode(IndexType linkedIndex){
		return linkedIndex < BUFFER_SIZE ? previousNodeUnchecked(linkedIndex) : end();
	}
	iterator previousNode(iterator linkedIt){
		IndexType linkedIndex = indexByNode(linkedIt);
		
		return linkedIndex != INVALID_INDEX ? previousNodeUnchecked(linkedIndex) : end();
	}

	bool validateNode(const Node* node){
		return node < &nodes[0] || node >= &nodes[BUFFER_END_INDEX];
	}

	bool isInvalidNode(const Node* node) const{
		return node < &nodes[0] || node >= &nodes[BUFFER_END_INDEX];
	}
	IndexType indexByNodeUnchecked(const Node* node) const {
		return node - &nodes[0];
	}

	IndexType indexByNode(const_iterator it) const {
		if(isInvalidNode(it.asNode())){
			return INVALID_INDEX;
		}
		return indexByNodeUnchecked(it.asNode());
		
	}

	iterator nodeByIndex(IndexType nodeIdx){
		if(nodeIdx >= BUFFER_END_INDEX){
			return end();
		}
		return iterator(&nodes[nodeIdx]);
	}
	const_iterator nodeByIndex(IndexType nodeIdx) const {
		if(nodeIdx >= BUFFER_END_INDEX){
			return cend();
		}
		return const_iterator(&nodes[nodeIdx]);
	}

	const Nodes& getNodesRaw(){
		return nodes;
	}

	//private:
	Nodes nodes;
	iterator begin_ptr = iterator(&nodes[BUFFER_END_INDEX]);
	iterator before_end = iterator(beforeBegin());
	InitializedArrayWrapper<BUFFER_SIZE, IndexType> previousNodeByIndex{INVALID_INDEX};
	
	
};
}// utils




#endif