#include "Database.h"

// Must be O(1)
Database::Database()
{
	m_validDb = true;
	m_numberOfLines = 0;
}

Database::~Database()
{
	for (unsigned int i = 0; i < m_schemaSize; i++)
		delete m_fieldIndex[i];

	m_fieldIndex.clear();
}

bool Database::specifySchema(const std::vector<FieldDescriptor>& schema)
{
	// First check for existing schema. Reset, if one exists
	if (!m_schema.empty())
		m_schema.clear();

	// Check for no indexed fields. Return false in this case
	int indexedFieldCounter = 0;
	for (unsigned int i = 0; i < schema.size(); i++)
	{
		if (schema[i].index == it_indexed)
			indexedFieldCounter++;
	}

	if (indexedFieldCounter == 0)
	{
		m_validDb = false;
		return false;
	}

	// TODO: Optional checks to implement: empty and duplicate name values

	// Initialize m_fieldIndex vector based on indexed fields
	m_fieldIndex.resize(schema.size());

	// Initialize private data member for schema size
	m_schemaSize = schema.size();

	for (unsigned int i = 0; i < m_schemaSize; i++)
		m_fieldIndex[i] = new MultiMap;

	m_schema = schema;
	return true;
}

bool Database::addRow(const std::vector<std::string>& rowOfData)
{
	// Check for existing schema and valid db. If none, return false;
	if (m_schema.empty() || !validDb())
		return false;

	// Check for mismatching row and schema vector sizes
	if (m_schema.size() != rowOfData.size())
		return false;

	m_rows.push_back(rowOfData);
	
	// Divy up values of row into fieldIndex
	for (unsigned int i = 0; i < rowOfData.size(); i++)
	{
		if (m_schema[i].index == it_indexed)
		{
			// "m_rows.size() - 1" will always be the row number of the most
			// recently added row (rowOfData) to the m_rows vector 
			m_fieldIndex[i]->insert(rowOfData[i], m_rows.size() - 1);
		}
	}
	
	return true;
}

bool Database::loadFromURL(std::string url)
{
	// Clear temp storage variable
	m_loadPageData = "";

	if (HTTP().get(url, m_loadPageData))
	{
		tokenizeFirstLineFromEntire(m_loadPageData);
		return true;
	}

	else
		return false;
}

bool Database::loadFromFile(std::string filename)
{
	std::ifstream infile(filename);

	if (!infile)
		return false;

	else
	{
		// Tokenize the first line to initialize the schema
		std::string line;
		std::getline(infile, line);
		tokenizeFirstLine(line);

		while (std::getline(infile, line))
		{
			// For input from URL equivalent see tokenizeFirstLineFromEntire()
			m_numberOfLines++;
			tokenizeLineIntoVector(line);
		}
		//std::cerr << m_loadPageData << std::endl;

		return true;
	}
}

int Database::getNumRows() const
{
	return m_numberOfLines;
}

bool Database::getRow(int rowNum, std::vector<std::string>& row) const
{
	if (0 <= rowNum && rowNum < m_rows.size())
	{
		row = m_rows[rowNum];
		return true;
	}

	return false;
}

int Database::search(const std::vector<SearchCriterion>& searchCriteria,
	const std::vector<SortCriterion>& sortCriteria,
	std::vector<int>& results)
{
	// Clear out anything in results
	results.clear();

	// Check for empty SearchCriterion
	if (searchCriteria.size() == 0)
		return ERROR_RESULT;

	// Check for mismatched field names and no min/max values
	for (unsigned int i = 0; i < searchCriteria.size(); i++)
	{
		if (searchCriteria[i].minValue.empty() && searchCriteria[i].maxValue.empty())
			return ERROR_RESULT;
		
		unsigned int p = 0;
		bool fieldNameMatch;

		do
		{
			fieldNameMatch = false;
			if (searchCriteria[i].fieldName == m_schema[p].name)
			{
				// searchCriteria will always be in order starting from 0
				m_searchSchemaMap.push_back(p);

				fieldNameMatch = true;

				break;
			}
			p++;
		} 
		while (p < m_schemaSize);

		if (fieldNameMatch == false)
			return ERROR_RESULT;
	}

	// Organize sort criteria into a key to be used by the sorting method (similar to m_searchSchemaMap)
	// Can't use the previous loop because searchCriteria and sortCriteria can have different sizes
	for (unsigned int k = 0; k < sortCriteria.size(); k++)
	{
		for (unsigned int p = 0; p < m_schemaSize; p++)
		{
			if (sortCriteria[k].fieldName == m_schema[p].name)
				m_sortSchemaMap.push_back(p);
		}
	}
	// Sort criteria may not be provided and the search function should still work

	// If we make it here, then that means all the SearchCriterion are valid
	// Now get all the matches and return the vector of results
	if (!getSearchCriteriaMatches(searchCriteria, results))
		return 0;  // Since no mathces found if returned false

	// Sort
	std::string sortMethod;
	while (sortMethod != "m")
	{
		std::cout << "Please enter a sorting method:\n[m]merge sort " << std::endl;
		std::getline(std::cin, sortMethod);
		if (sortMethod != "m")
			std::cout << "You did not enter [m]" << std::endl;
	}

	// Since sortCriteria is a const parameter we need to make a copy
	std::vector<SortCriterion> sortCritCopy = sortCriteria;
	int resultsSize = results.size();
	mergeSort(sortCritCopy, results, resultsSize);

	return results.size();
}

/////////////////////
/* PRIVATE METHODS */
/////////////////////

bool Database::validDb() const
{
	return m_validDb;
}

// Both input from URL and File will pass through here
bool Database::tokenizeFirstLine(std::string firstLine)
{
	std::string delimiters = ",";
	Tokenizer t(firstLine, delimiters);
	std::string word;
	std::string indexedWord;
	std::vector<FieldDescriptor> schema;
	FieldDescriptor tempFd;

	while (t.getNextToken(word))
	{
		if (word[word.length() - 1] == '*')
		{
			// Check for empty space otherwise undefined behavior
			if (word.length() - 1 == 0)
				return false;

			word.resize(word.length() - 1);
			tempFd.name = word;
			tempFd.index = it_indexed;
			schema.push_back(tempFd);
		}

		else
		{
			tempFd.name = word;
			tempFd.index = it_none;
			schema.push_back(tempFd);
		}
	}

	return specifySchema(schema);

}

// Only input from URL will pass through here
bool Database::tokenizeFirstLineFromEntire(const std::string& entireText)
{
	std::istringstream s(m_loadPageData);
	std::string line;
	if (std::getline(s, line))
	{
		tokenizeFirstLine(line);
		while (std::getline(s, line))
		{
			// Equivalent to usage in loadFromFile()
			m_numberOfLines++;
			tokenizeLineIntoVector(line);
		}
		return true;
	}
	else
		return false;
}

// Both input from URL and File pass through here
void Database::tokenizeLineIntoVector(const std::string& singleLine)
{
	std::string delimiters = ",";
	Tokenizer t(singleLine, delimiters);
	std::string word;
	std::vector<std::string> row;

	while (t.getNextToken(word))
		row.push_back(word);

	addRow(row);

	// minus 1 because line counter is pre-incremented
	insertIntoFieldIndex(row, m_numberOfLines - 1);
}

void Database::insertIntoFieldIndex(const std::vector<std::string>& row, int rowNum)
{
	// Iterating through only a single row at a time
	for (unsigned int i = 0; i < m_schemaSize; i++)
		if (m_schema[i].index == it_indexed)
			m_fieldIndex[i]->insert(row[i], rowNum);
}

bool Database::getSearchCriteriaMatches(const std::vector<SearchCriterion>& searchCriteria,
	std::vector<int>& results)
{
	// Must be O(M log N), M matched iterms and N rows
	std::unordered_set<int> first;
	std::unordered_set<int> second;

	// Each time an additional search criterion after the first is added
	// check the new values against the previous and put the union of values
	// into a new temp unordered set. Once done iterating through this second
	// set, assign over the values from the second back to the first. Continue this
	// loop checking any additional search criterion against the previous amalgamation
	// Must be O(CM log N) C number of search criteria, M matched items and N rows
	for (unsigned int i = 0; i < searchCriteria.size(); i++)
	{
		// This gets the corresponding field index 
		int tempFieldIndexSub = m_searchSchemaMap[i];

		std::string minVal = searchCriteria[i].minValue;
		std::string maxVal = searchCriteria[i].maxValue;

		MultiMap::Iterator it; 

		// There are 3 possible cases: 
		// (A) both min and max are provided (iterate from min towards max)
		// (B) min is provided but max is NOT provided (same, iterate from min towards max which is an invalid state)
		// (C) min is NOT provided but max is provided (start iterating from max backwards towards min which is the invalid state) 

		if (minVal != "")  // Case (A) and (B)
			it = m_fieldIndex[tempFieldIndexSub]->findEqualOrSuccessor(minVal);

		else if (minVal == "" && maxVal != "")  // Case (C)
			it = m_fieldIndex[tempFieldIndexSub]->findEqualOrPredecessor(maxVal);

		if (!it.valid())  //  This likely implies a MultiMap that has very few or 0 actual items
			return false;

		else
		{
			// If either no min or max value provided, this loop will terminate in accordance after getting the
			// greatest possible min or max value
			while (it.valid()) 
			{
				// Case (A)
				if (minVal != "" &&  maxVal != "" && it.getKey() > maxVal)
					break;

				// Only on the first iteration
				if (i == 0)
					first.insert(it.getValue());

				else
				{
					// unordered set "count", 1 indicates match is also in the prior unordered set (first)
					if (first.count(it.getValue()) == 1)
						second.insert(it.getValue());
				}

				// Case (A) and (B)
				if (minVal != "")
					it.next();

				// Case (C)
				else
					it.prev();
			}

			// Assign over the union between the two back to first only after the first iteration
			if (i > 0)
			{
				first = second;
				second.clear();
			}
		}
	}

	// Copy the row numbers into the results vector and return
	// using c++11 range-based for loop
	for (unsigned int x : first)
		results.push_back(x);

	return true;
}

void Database::mergeSort(std::vector<SortCriterion>& sortCriteria,
	std::vector<int>& results, int size)
{
	if (size == 1)
		return;

	// Recursively call the first half of results
	mergeSort(sortCriteria, results, size / 2);

	// Create a vector for the second half of results values
	std::vector<int> secHalf;
	for (int i = size / 2; i < size; i++)
		secHalf.push_back(results[i]);

	// Recursively call the second half of results (secHalf vector)
	mergeSort(sortCriteria, secHalf, size - (size / 2));

	// Put the results of secHalf back into results
	for (int g = size / 2, j = 0; g < size; g++, j++)
		results[g] = secHalf[j];

	// Call merge to merge everything together in O(log N)
	merge(sortCriteria, results, size / 2, size - (size / 2));

}

void Database::merge(std::vector<SortCriterion>& sortCriteria, 
	std::vector<int>& results, int n1, int n2)
{
	int i, j, k;
	i = j = k = 0;

	std::vector<int> temp;

	// Initialize a vector that holds the sec half
	std::vector<int> sec;
	for (int a = n1; a < results.size(); a++)
		sec.push_back(results[a]);
	
	while (i < n1 || j < n2)
	{
		std::string tempVal;
		std::string secTempVal;

		if (i < n1)
		{
			// A temp string value corresponding to the first value of the sort criteria
			tempVal = m_rows[results[i]][m_sortSchemaMap[k]];
		}

		if (j < n2)
		{
			// Another temp string value but for the second half of results
			secTempVal = m_rows[sec[j]][m_sortSchemaMap[k]];
		}

		// No more values from the first half
		if (i == n1)
		{
			temp.push_back(sec[j++]);
		}

		// No more values from the second half
		else if (j == n2)
		{
			temp.push_back(results[i++]);
		}

		// Same as the values represnted by results[i] < sec[j]
		else if (tempVal < secTempVal)
		{
			temp.push_back(results[i++]);
		}

		// If the values are equal
		// TODO FIX BUG HERE: With more than 1 sort criteria, some items are out of order or duplicated
		else if (tempVal == secTempVal)
		{
			for (k = 0; k < m_sortSchemaMap.size(); k++)
			{
				if (sortCriteria[k].ordering == ot_ascending)
				{
					if (m_rows[results[i]][m_sortSchemaMap[k]] < m_rows[sec[j]][m_sortSchemaMap[k]])
					{
						temp.push_back(results[i++]);
						break;
					}
					
					else if (m_rows[results[i]][m_sortSchemaMap[k]] > m_rows[sec[j]][m_sortSchemaMap[k]])
					{
						temp.push_back(results[j++]);
						break;
					}

					// If all values by sort map are equal
					else if (k == m_sortSchemaMap.size() - 1 &&
						m_rows[results[i]][m_sortSchemaMap[k]] == m_rows[sec[j]][m_sortSchemaMap[k]])
					{
						temp.push_back(results[i++]);
						break;
					}
				}

				else if (sortCriteria[k].ordering == ot_descending)
				{
					if (m_rows[results[i]][m_sortSchemaMap[k]] < m_rows[sec[j]][m_sortSchemaMap[k]])
					{
						temp.push_back(sec[j++]);
						break;
					}

					else if (m_rows[results[i]][m_sortSchemaMap[k]] > m_rows[sec[j]][m_sortSchemaMap[k]])
					{
						temp.push_back(sec[i++]);
						break;
					}

					else if (k == m_sortSchemaMap.size() - 1 &&
						m_rows[results[i]][m_sortSchemaMap[k]] == m_rows[sec[j]][m_sortSchemaMap[k]])
					{
						temp.push_back(results[i++]);
						break;
					}
				}
			}
			k = 0;  // Reset k because not all values start off initially equal}
		}

		else
			temp.push_back(sec[j++]);
	}

	for (int z = 0; z < (n1 + n2); z++)
		results[z] = temp[z];
}

////////////////////
/* TEST FUNCTIONS */
////////////////////

bool Database::printBST() const
{
	// Check MultiMap validity
	if (!validDb())
		return false;
	
	for (unsigned int i = 0; i < m_schemaSize; i++)
		m_fieldIndex[i]->testPrintInit();

	return true;
}

bool Database::printSchema() const
{
	if (!validDb())
		return false;

	for (unsigned int i = 0; i < m_schemaSize; i++)
	{
		std::cerr << m_schema[i].name << " | " <<
			m_schema[i].index << std::endl;
	}

	return true;
}

bool Database::printRows() const
{
	if (!validDb())
		return false;

	for (unsigned int i = 0; i < m_rows.size(); i++)
	{
		for (unsigned int k = 0; k < m_rows[i].size(); k++)
			std::cerr << m_rows[i][k] << " ";
		std::cerr << std::endl;
	}
	return true;
}

bool Database::printMultiMaps() const
{
	if (!validDb())
		return false;
	
	for (unsigned int i = 0; i < m_schemaSize; i++)
		m_fieldIndex[i]->testPrintInit();

	return true;
}