#include "MultiMap.h"

// Must be O(1)
MultiMap::Iterator::Iterator()
{
	m_valid = false;
}

// Must be O(1)
MultiMap::Iterator::Iterator(Node *init)
{
	m_ptrToNode = init;
	m_ptrToNodeList = m_ptrToNode->val;
	m_currentDuplicateValue = m_ptrToNode->val->value;
	m_valid = true;
}

// Constructor for valid find equal or predecessor
MultiMap::Iterator::Iterator(Node *init, int x)
{
	m_ptrToNode = init;
	// Initializes duplicate value to the end value
	m_ptrToNodeList = m_ptrToNode->tail;
	m_currentDuplicateValue = m_ptrToNode->tail->value;
	m_valid = true;
}

bool MultiMap::Iterator::valid() const
{
	return m_valid;
}

// Must be O(1)
std::string MultiMap::Iterator::getKey() const
{
	if (!valid())
		return "ERROR";

	return m_ptrToNode->key;
}

unsigned int MultiMap::Iterator::getValue() const
{
	if (!valid())
		return -1;

	return m_currentDuplicateValue;
}

bool MultiMap::Iterator::next()
{
	if (!valid())
		return false;

	while (m_ptrToNode != nullptr)
	{
		// check is cur->duplicate counter is greater than 0
		// if it is, check where this (next or prev) function last left off
		if (m_ptrToNode->duplicateTotal > 0)
		{
			if (m_ptrToNodeList->next != nullptr)
			{
				m_ptrToNodeList = m_ptrToNodeList->next;
				m_currentDuplicateValue = m_ptrToNodeList->value;
				return true;
			}
		}

		// Check non nullptr right child
		if (m_ptrToNode->right != nullptr)
		{
			m_ptrToNode = m_ptrToNode->right;
			// Now check for left children
			if (m_ptrToNode->left == nullptr)
			{
				m_ptrToNodeList = m_ptrToNode->val;
				m_currentDuplicateValue = m_ptrToNodeList->value;
				return true;
			}
			else
			{
				while (m_ptrToNode->left != nullptr)
					m_ptrToNode = m_ptrToNode->left;

				m_ptrToNodeList = m_ptrToNode->val;
				m_currentDuplicateValue = m_ptrToNodeList->value;
				return true;
			}
		}

		// If right child nullptr, check parent (loop up through parents)
		if (m_ptrToNode->right == nullptr)
		{
			std::string tempKey = m_ptrToNode->key;
			while (m_ptrToNode->parent != nullptr)
			{
				m_ptrToNode = m_ptrToNode->parent;
				if (m_ptrToNode->key > tempKey)
				{
					m_ptrToNodeList = m_ptrToNode->val;
					m_currentDuplicateValue = m_ptrToNodeList->value;
					return true;
				}
			}
			// Invalidates iterator to prevent undefined behavior
			invalidateIterator();
			return false;
		}
	}
	invalidateIterator();
	return false;
}

bool MultiMap::Iterator::prev()
{
	if (!valid())
		return false;

	while (m_ptrToNode != nullptr)
	{
		// First check for duplicate values beginning from current tail pointer
		if (m_ptrToNode->duplicateTotal > 0)
		{
			if (m_ptrToNodeList->prev != nullptr)
			{
				m_ptrToNodeList = m_ptrToNodeList->prev;
				m_currentDuplicateValue = m_ptrToNodeList->value;
				return true;
			}
		}

		// If left child is not nullptr go down that branch 
		// and check if that child has a right child
		if (m_ptrToNode->left != nullptr)
		{
			m_ptrToNode = m_ptrToNode->left;

			// If that child has a right child, loop down the right child branch
			// until we hit a nullptr
			if (m_ptrToNode->right != nullptr)
			{
				while (m_ptrToNode->right != nullptr)
					m_ptrToNode = m_ptrToNode->right;
				m_ptrToNodeList = m_ptrToNode->tail;
				m_currentDuplicateValue = m_ptrToNodeList->value;
				return true;
			}

			// Otherwise if right child is nullptr, we are at the right (prev) spot
			else
			{
				m_ptrToNodeList = m_ptrToNode->tail;
				m_currentDuplicateValue = m_ptrToNodeList->value;
				return true;
			}
		}

		// If left child IS a nullptr, loop through parents until we find
		// the first node that is less than the current key or until
		// we hit a parent nullptr (return false in this case)
		if (m_ptrToNode->left == nullptr)
		{
			std::string tempKey = m_ptrToNode->key;
			while (m_ptrToNode->parent != nullptr)
			{
				m_ptrToNode = m_ptrToNode->parent;
				if (m_ptrToNode->key < tempKey)
				{
					m_ptrToNodeList = m_ptrToNode->tail;
					m_currentDuplicateValue = m_ptrToNodeList->value;
					return true;
				}
			}
			invalidateIterator();
			return false;
		}
	}
	invalidateIterator();
	return false;
}

// Must be O(1)
MultiMap::MultiMap()
{
	m_root = nullptr;
}

// Must be O(N)
MultiMap::~MultiMap()
{
	clear();
}

// Must be O(N)
void MultiMap::clear()
{
	Node *temp = m_root;
	clearBST(temp);
}

void MultiMap::insert(std::string key, unsigned int value)
{
	// Check for empty tree
	if (m_root == nullptr)
	{
		m_root = new Node(key, value);
		return;
	}

	Node *cur = m_root;
	for (;;)
	{
		// For duplicate key values
		if (key == cur->key)
		{
			NodeList *duplicateKey = new NodeList(value);
			NodeList *currentVal = cur->val;

			while (currentVal->next != nullptr)
				currentVal = currentVal->next;

			currentVal->next = duplicateKey;
			duplicateKey->prev = currentVal;
			cur->tail = duplicateKey;
			cur->duplicateTotal++;
			return;
		}

		if (key < cur->key)
		{
			if (cur->left != nullptr)
				cur = cur->left;
			else
			{
				cur->left = new Node(key, value);
				cur->left->parent = cur;
				return;
			}
		}

		else if (key > cur->key)
		{
			if (cur->right != nullptr)
				cur = cur->right;
			else
			{
				cur->right = new Node(key, value);
				cur->right->parent = cur;
				return;
			}
		}
	}
}

MultiMap::Iterator MultiMap::findEqual(std::string key) const
{
	Node *cur = m_root;
	Iterator it;

	while (cur != nullptr)
	{
		if (key == cur->key)
		{
			it = cur;
			return it;
		}

		else if (key < cur->key)
			cur = cur->left;

		else
			cur = cur->right;
	}

	return it;
}

MultiMap::Iterator MultiMap::findEqualOrSuccessor(std::string key) const
{
	Node *cur = m_root;
	Iterator it;

	while (cur != nullptr)
	{
		if (key == cur->key)
		{
			Iterator validIt(cur);
			return validIt;
		}

		else if (key < cur->key)
		{
			if (cur->left == nullptr)
			{
				Iterator validIt(cur);
				return validIt;
			}
			cur = cur->left;
		}

		else  // key > cur->key
		{
			if (cur->right == nullptr)
			{
				Node *tempCur = cur;
				while (tempCur->parent != nullptr)
				{
					tempCur = tempCur->parent;
					if (tempCur->key > key)
					{
						Iterator validIt(tempCur);
						return validIt;
					}
				}
			}
			cur = cur->right;
		}
	}

	return it;
}

MultiMap::Iterator MultiMap::findEqualOrPredecessor(std::string key) const
{
	Node *cur = m_root;
	Iterator it;

	while (cur != nullptr)
	{
		if (key == cur->key)
		{
			// Specific constructor to predecessor
			// TODO: CHANGE BECAUSE BAD STYLE FUCNTIONLESS PARAMETER (0)
			Iterator validIt(cur, 0);
			return validIt;
		}

		else if (key < cur->key)
		{
			cur = cur->left;
		}

		else  // key > cur->key
		{
			if (cur->right == nullptr)
			{
				Iterator validIt(cur, 0);
				return validIt;
			}
			else if (cur->right->key > key &&
				cur->right->left == nullptr)
			{
				Iterator validIt(cur, 0);
				return validIt;
			}
			else // go down all the way to the left most child and compare with cur
			{
				Node* tempCur = cur;
				tempCur = tempCur->right;
				while (tempCur->left != nullptr)
					tempCur = tempCur->left;
				if (tempCur->key > key)
				{
					Iterator validIt(cur, 0);
					return validIt;
				}
			}

			cur = cur->right;
		}
	}

	return it;
}

/////////////////////
/* PRIVATE METHODS */
/////////////////////

void MultiMap::Iterator::invalidateIterator()
{
	m_valid = false;
}

void MultiMap::clearBST(Node *cur) const
{
	if (cur == nullptr)
		return;

	clearBST(cur->left);
	clearBST(cur->right);

	clearNodeList(cur);

	delete cur;
}

void MultiMap::clearNodeList(Node *cur) const
{
	NodeList *temp = cur->val;

	while (temp != nullptr)
	{
		NodeList *nextOne;
		nextOne = temp->next;
		delete temp;
		temp = nextOne;
	}
}

////////////////////
/* TEST FUNCTIONS */
////////////////////

void MultiMap::testPrintInit()
{
	Node *temp = m_root;
	testPrintBST(temp);
}

void MultiMap::testPrintBST(Node *cur)
{
	if (cur == nullptr)
		return;

	testPrintBST(cur->left);

	std::string curString = cur->key;
	NodeList* tempNL = cur->val;
	while (tempNL != nullptr)
	{
		unsigned int tempInt = tempNL->value;
		std::cerr << curString << " : " << tempInt << std::endl;
		tempNL = tempNL->next;
	}

	testPrintBST(cur->right);
}

void MultiMap::Iterator::testIteratorPrint()
{
	if (valid())
		std::cerr << m_ptrToNode->key << " : " << m_ptrToNode->val->value << std::endl;
	else
		std::cerr << "Invalid iterator: check if key parameter is valid" << std::endl;
}
