#ifndef I_DATABASECON_H
#define I_DATABASECON_H

#include <string>
#include <memory>
#include "../../shared/ScopedLock.h"

// Forward declaration
class Database;

class IDatabaseCon {
public:
    virtual ~IDatabaseCon() = default;

    // Retrieve the database instance
    virtual Database* getDB() = 0;

    // Acquire a scoped lock on the database
    virtual ScopedLock getDBLock() = 0;
};

#endif // I_DATABASECON_H
