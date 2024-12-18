#ifndef DATABASECON_H
#define DATABASECON_H

#include "IDatabaseCon.h"
#include "../../database/SqliteDatabase.h"
#include <boost/thread/recursive_mutex.hpp>
#include "../../shared/ScopedLock.h"

class DatabaseCon : public IDatabaseCon {
protected:
    Database* mDatabase;
    boost::recursive_mutex mLock;

public:
    DatabaseCon(const std::string& name, const char* initString[], int countInit);
    ~DatabaseCon();

    Database* getDB() override;
    ScopedLock getDBLock() override;
};

#endif // DATABASECON_H
