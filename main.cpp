#include "MultiMap.h"
#include "Database.h"
#include <iostream>
#include <string>
#include <cassert>

// Database tests
bool setSchema(Database& db);
bool addARow(Database& db);
bool addFromInternet(Database& db, std::string url);
bool addFromFile(Database& db, std::string fileName);
void doAQuery(Database &db);

// MultiMap tests (BROKEN)
void initMultiMapTest();
void findEqualTests(MultiMap test);
void nextIteratorTest(MultiMap test);

/* KNOWN BUGS */
// - Having more than 1 sort criterion will have some results out of order (See AYERS OFELIA)

int main()
{
	/* TEST LOAD FROM RUNTIME ENVIRONMENT */
	Database A;
	//assert(setSchema(A));
	//assert(addARow(A));

	/* TEST LOAD FROM URL */
	Database B;
	std::string uclaUrl[] = {
		"http://www.cs.ucla.edu/classes/winter14/cs32/Projects/4/Data/census.csv",
		"http://www.cs.ucla.edu/classes/winter14/cs32/Projects/4/Data/fn_ln_age_kids_married_ssn_25k.csv",
		"http://www.cs.ucla.edu/classes/winter14/cs32/Projects/4/Data/fn_ln_age_kids_married_ssn_100k.csv"
	};

	//assert(addFromInternet(B, uclaUrl[0]));
	//assert(addFromInternet(B, uclaUrl[1]));
	//assert(addFromInternet(B, uclaUrl[2]));
	//assert(B.printSchema());
	//assert(B.printRows());
	//assert(B.printMultiMaps());

	/* TEST LOAD FROM FILE */
	Database C;
	std::string fileNames[] = {
		"C:/Users/nibeecolisp/Documents/Visual Studio 2013/Projects/CS32 2014 P4/CS32 2014 P4/data/data.txt",
		"C:/Users/nibeecolisp/Documents/Visual Studio 2013/Projects/CS32 2014 P4/CS32 2014 P4/data/data2.txt",
		"C:/Users/nibeecolisp/Documents/Visual Studio 2013/Projects/CS32 2014 P4/CS32 2014 P4/data/data3.txt"
	};

	assert(addFromFile(C, fileNames[0]));
	//assert(addFromFile(C, fileNames[1]));
	//assert(addFromFile(C, fileNames[2]));
	//assert(C.printSchema());
	//assert(C.printRows());
	//assert(C.printMultiMaps());

	/* TEST PRINT BINARY SEARCH TREE*/
	//assert(A.printBST());

	/* TEST DO A DATABASE QUERY */
	doAQuery(C);
	
	std::cerr << "\nPassed all Database tests\n" << std::endl;

	// The following will not work unless a proper destructor, copy constructer 
	// and assignment operater are written (hence BROKEN)
	
	//MultiMap test1 = initMultiMapTest();
	//test.testPrintInit();
	//findEqualTests(test1);
	//nextIteratorTest(test1);
}

bool setSchema(Database& db)
{
	Database::FieldDescriptor fd1, fd2, fd3;

	fd1.name = "username";
	fd1.index = Database::it_indexed;

	fd2.name = "phonenum";
	fd2.index = Database::it_indexed;

	fd3.name = "age";
	fd3.index = Database::it_none;

	std::vector<Database::FieldDescriptor> schema;
	schema.push_back(fd1);
	schema.push_back(fd2);
	schema.push_back(fd3);

	return db.specifySchema(schema);
}

bool addARow(Database& db)
{
	std::vector<std::string> row;
	row.push_back("Daniel");
	row.push_back("310-439-2932");
	row.push_back("0035");

	return db.addRow(row);
}

bool addFromInternet(Database& db, std::string url)
{
	bool loadOk = db.loadFromURL(url);

	if (loadOk)
	{
		std::cerr << url << " loaded into schema successfully" << std::endl;
		return true;
	}

	else
	{
		std::cerr << "Error loading schema from " << url << std::endl;
		return false;
	}

}

bool addFromFile(Database& db, std::string fileName)
{
	bool fileOk = db.loadFromFile(fileName);

	if (fileOk)
	{
		std::cerr << fileName << " loaded into schema successfully" << std::endl;
		return true;
	}

	else
	{
		std::cerr << "Error loading schema from " << fileName << std::endl;
		return false;
	}
}

void doAQuery(Database& db)
{
	// Query based on Data.txt or Data3.txt which is equivalent to the 2nd URL
	std::vector<Database::SearchCriterion> searchCrit;

	Database::SearchCriterion s1;
	s1.fieldName = "LastName";
	s1.minValue = "A";
	s1.maxValue = "N";

	Database::SearchCriterion s2;
	s2.fieldName = "Age";
	s2.minValue = "";  // No minimum specified
	s2.maxValue = "099";

	Database::SearchCriterion s3;
	s3.fieldName = "FirstName";
	s3.minValue = "O";
	s3.maxValue = "";  // No maximum specified

	searchCrit.push_back(s1);
	searchCrit.push_back(s2);
	searchCrit.push_back(s3);

	std::vector<Database::SortCriterion> sortCrit;

	Database::SortCriterion c1;
	c1.fieldName = "LastName";
	c1.ordering = Database::ot_ascending;

	Database::SortCriterion c2;
	c2.fieldName = "Age";
	c2.ordering = Database::ot_ascending;

	Database::SortCriterion c3;
	c3.fieldName = "FirstName";
	c3.ordering = Database::ot_descending;

	sortCrit.push_back(c1);
	sortCrit.push_back(c2);
	sortCrit.push_back(c3);

	std::vector<int> results;
	int numFound = db.search(searchCrit, sortCrit, results);
	if (numFound == Database::ERROR_RESULT)
		std::cerr << "Error querying the database. Check for mispelled field names" << std::endl;
	else
	{
		std::cerr << numFound << " rows matched the criteria; here they are:" << std::endl;
		for (size_t i = 0; i < results.size(); i++)
		{
			// Print the row number out where we had a match
			std::cerr << "Row #" << results[i] << ": ";

			// Get and print the field values out from that row
			std::vector<std::string> rowData;
			if (db.getRow(results[i], rowData))
			{
				for (size_t j = 0; j < rowData.size(); j++)
					std::cerr << rowData[j] << " ";
				std::cerr << std::endl;
			}
			else
				std::cerr << "Error retriving row's data" << std::endl;
		}
	}
}

void initMultiMapTest()
{
	MultiMap test;
	test.insert("D", 99);
	test.insert("R", 1);
	test.insert("B", 1);
	test.insert("P", 1);
	test.insert("Ab", 1);
	test.insert("C", 0);
	test.insert("D", 10);
	test.insert("D", 2);
	test.insert("O", 54);
	test.insert("N", 23);
	test.insert("Pa", 98);
	test.insert("Oa", 22);

	//return test;
}

void findEqualTests(MultiMap test)
{
	MultiMap::Iterator a = test.findEqual("D");
	//a.testIteratorPrint();
	assert(a.valid());
	MultiMap::Iterator b;
	//b.testIteratorPrint();
	assert(!b.valid());
	MultiMap::Iterator c = test.findEqualOrSuccessor("D");
	//c.testIteratorPrint();
	assert(c.valid());
	MultiMap::Iterator d = test.findEqualOrSuccessor("E");
	//d.testIteratorPrint();
	assert(d.valid());
	MultiMap::Iterator e = test.findEqualOrSuccessor("Cb");
	//e.testIteratorPrint();
	assert(e.valid());
	MultiMap::Iterator f = test.findEqualOrPredecessor("E");
	//f.testIteratorPrint();
	assert(f.valid());
	MultiMap::Iterator g = test.findEqualOrPredecessor("Cb");
	//g.testIteratorPrint();
	assert(g.valid());
	MultiMap::Iterator h = test.findEqualOrPredecessor("Ab");
	//h.testIteratorPrint();
	assert(h.valid());
	MultiMap::Iterator i = test.findEqualOrPredecessor("A");
	assert(!i.valid());

	std::cerr << "Passed all find equal tests" << std::endl;
}

void nextIteratorTest(MultiMap test)
{
	MultiMap::Iterator h = test.findEqualOrPredecessor("Ab");

	while (h.valid() != false)
	{
		std::cout << h.getKey() << " : " << h.getValue() << std::endl;
		h.next();
	}

	std::cout << h.getKey() << " : " << h.getValue() << std::endl;
}

// Main() tests, Removed and placed here for readability
/*
MultiMap test2;
test2.insert("D", 99);
test2.insert("R", 1);
test2.insert("B", 1);
test2.insert("P", 1);
test2.insert("Ab", 1);
test2.insert("C", 0);
test2.insert("D", 10);
test2.insert("D", 2);
test2.insert("O", 54);
test2.insert("N", 23);
test2.insert("Pa", 98);
test2.insert("Oa", 22);

MultiMap::Iterator a = test2.findEqual("Ab");
assert(a.valid());
MultiMap::Iterator c = test2.findEqualOrSuccessor("B");
assert(c.valid());
MultiMap::Iterator h = test2.findEqualOrPredecessor("T");
assert(h.valid());
MultiMap::Iterator i = test2.findEqualOrPredecessor("D");
assert(h.valid());

while (a.valid() != false)
{
std::cout << a.getKey() << " : " << a.getValue() << std::endl;
a.prev();
}
std::cout << std::endl;
while (c.valid() != false)
{
std::cout << c.getKey() << " : " << c.getValue() << std::endl;
c.prev();
}
std::cout << std::endl;
while (h.valid() != false)
{
std::cout << h.getKey() << " : " << h.getValue() << std::endl;
h.prev();
}
std::cout << std::endl;

std::cout << i.getKey() << " : " << i.getValue() << std::endl;
i.prev();
std::cout << i.getKey() << " : " << i.getValue() << std::endl;
i.prev();
std::cout << i.getKey() << " : " << i.getValue() << std::endl;
i.next();
std::cout << i.getKey() << " : " << i.getValue() << std::endl;
i.next();
std::cout << i.getKey() << " : " << i.getValue() << std::endl;
i.next();
std::cout << i.getKey() << " : " << i.getValue() << std::endl;
i.next();
std::cout << i.getKey() << " : " << i.getValue() << std::endl;
i.next();
*/
