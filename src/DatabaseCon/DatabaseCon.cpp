#include "DatabaseCon.h"
#include "../../Config.h"
#include <boost/filesystem.hpp>

// Constructor
DatabaseCon::DatabaseCon(const std::string& strName, const char* initStrings[], int initCount)
{
    boost::filesystem::path pPath = theConfig.DATA_DIR / strName;

    // Initialize and connect to the SQLite database
    mDatabase = new SqliteDatabase(pPath.string().c_str());
    mDatabase->connect();

    // Execute initialization SQL strings
    for (int i = 0; i < initCount; ++i) {
        mDatabase->executeSQL(initStrings[i], true);
    }
}

// Destructor
DatabaseCon::~DatabaseCon()
{
    if (mDatabase) {
        mDatabase->disconnect();
        delete mDatabase;
        mDatabase = nullptr;
    }
}

// getDB() - Returns the database instance
Database* DatabaseCon::getDB()
{
    return mDatabase;
}

// getDBLock() - Returns a scoped lock for thread safety
ScopedLock DatabaseCon::getDBLock()
{
    return ScopedLock(mLock);
}
