#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>  // for input and output files
#include <sstream>  // for string streams (load from URL to m_loadPageData)
#include <unordered_set>  // for search criteria
#include "MultiMap.h"
#include "http.h"
#include "Tokenizer.h"

class Database
{
public:
	enum IndexType { it_none, it_indexed };
	enum OrderingType { ot_ascending, ot_descending };

	struct FieldDescriptor
	{
		std::string name;
		IndexType index;
	};

	struct SearchCriterion
	{
		std::string fieldName;
		std::string minValue;
		std::string maxValue;
	};

	struct SortCriterion
	{
		std::string fieldName;
		OrderingType ordering;
	};

	static const int ERROR_RESULT = -1;

	Database();
	~Database();
	bool specifySchema(const std::vector<FieldDescriptor>& schema);
	bool addRow(const std::vector<std::string>& rowOfData);
	bool loadFromURL(std::string url);
	bool loadFromFile(std::string filename);
	int getNumRows() const;
	bool getRow(int rowNum, std::vector<std::string>& row) const;
	int search(const std::vector<SearchCriterion>& searchCriteria,
		const std::vector<SortCriterion>& sortCriteria,
		std::vector<int>& results);

	// Test printing
	bool printBST() const;
	bool printSchema() const;
	bool printRows() const;
	bool printMultiMaps() const;

private:
	// Prevents Database objects from being copied or assigned
	Database(const Database& other);
	Database& operator=(const Database& rhs);

	// Private methods
	bool validDb() const;
	bool tokenizeFirstLine(std::string firstLine); 
	bool tokenizeFirstLineFromEntire(const std::string& entireText);  // input from URL
	void tokenizeLineIntoVector(const std::string& singleLine);
	void insertIntoFieldIndex(const std::vector<std::string>& row, int rowNum);
	bool getSearchCriteriaMatches(const std::vector<SearchCriterion>& searchCriteria, 
		std::vector<int>& results);

	// Sorting methods
	void mergeSort(std::vector<SortCriterion>& sortCriteria,
		std::vector<int>& results, int size);
	void merge(std::vector<SortCriterion>& sortCriteria, 
		std::vector<int>& results, int n1, int n2);

	// Private data members
	std::vector<std::vector<std::string> > m_rows;
	std::vector<MultiMap*> m_fieldIndex;
	std::vector<FieldDescriptor> m_schema;
	std::vector<int> m_searchSchemaMap;
	std::vector<int> m_sortSchemaMap;
	std::string m_loadPageData;
	unsigned int m_schemaSize;
	unsigned int m_numberOfLines;
	bool m_validDb;

};

#endif  // DATABASE_H