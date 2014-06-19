#ifndef MULTIMAP_H
#define MULTIMAP_H

#include <string>
#include <iostream>

//template <typedef key, typedef value>
class MultiMap
{
public:

	struct NodeList
	{
		NodeList(unsigned int valueInput)
		{
			next = prev = nullptr;
			value = valueInput;
		}
		unsigned int value;
		NodeList *next, *prev;
	};

	struct Node
	{
		Node(std::string keyInput, unsigned int valueInput) 
		{ 
			key = keyInput;
			val = tail = new NodeList(valueInput);
			duplicateTotal = 0;
			left = right = parent = nullptr;
		}
		std::string key;
		NodeList *val, *tail;
		unsigned int duplicateTotal;
		Node *left, *right, *parent;
	};

	class Iterator
	{
	public:
		Iterator();
		Iterator(Node *init);
		Iterator(Node *init, int x);
		bool valid() const;
		std::string getKey() const;
		unsigned int getValue() const;
		bool next();
		bool prev();

		// Test printing
		void testIteratorPrint();

	private:
		// Private methods
		void invalidateIterator();

		// Private data members
		Node *m_ptrToNode;
		NodeList *m_ptrToNodeList;
		bool m_valid;
		unsigned int m_currentDuplicateValue;
	};

	MultiMap();
	~MultiMap();
	void clear();
	void insert(std::string key, unsigned int value);
	Iterator findEqual(std::string key) const;
	Iterator findEqualOrSuccessor(std::string key) const;
	Iterator findEqualOrPredecessor(std::string key) const;

	// Test printing
	void testPrintInit();
	void testPrintBST(Node* cur);

private:
	// Prevents MultiMaps from being copied or assigned
	MultiMap(const MultiMap& other);
	MultiMap& operator=(const MultiMap& rhs);

	// Private methods
	void clearBST(Node *cur) const;
	void clearNodeList(Node *cur) const;

	// Private data members
	Node* m_root;

};

#endif  // MULTIMAP_H