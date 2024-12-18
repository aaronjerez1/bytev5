#ifndef SQLITE_DATABASE_H
#define SQLITE_DATABASE_H
#include "database.h"

struct sqlite3;
struct sqlite3_stmt;

class SqliteDatabase : public Database
{
	sqlite3* mConnection;
	sqlite3_stmt* mCurrentStmt;
	bool mMoreRows;
public:
	SqliteDatabase(const char* host);

	void connect();
	void disconnect();

	// returns true if the query went ok
	bool executeSQL(const char* sql, bool fail_okay);

	// tells you how many rows were changed by an update or insert
	int getNumRowsAffected();
	int getLastInsertID();

	// returns false if there are no results
	bool startIterRows();
	void endIterRows();

	// call this after you executeSQL
	// will return false if there are no more rows
	bool getNextRow();

	bool getNull(int colIndex);
	char* getStr(int colIndex,std::string& retStr);
	int32 getInt(int colIndex);
	float getFloat(int colIndex);
	bool getBool(int colIndex);
	// returns amount stored in buf
	int getBinary(int colIndex,unsigned char* buf,int maxSize);
	std::vector<unsigned char> getBinary(int colIndex);
	uint64 getBigInt(int colIndex);

	void escape(const unsigned char* start,int size,std::string& retStr);
};
#endif // SQLITE_DATABASE_H

// vim:ts=4
