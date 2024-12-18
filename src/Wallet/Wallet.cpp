#define WIN32_LEAN_AND_MEAN 

#include "Wallet.h"
//#include "Ledger.h"
#include "../NewcoinAddress/NewcoinAddress.h"
#include "../DatabaseCon/IDatabaseCon.h"
#include "../Application.h"
#include "../../shared/ScopedLock.h"
#include "../../shared/utils.h"

#include <boost/format.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>
#include <stdexcept>

Wallet::Wallet(std::shared_ptr<IDatabaseCon> mWalletDB, std::shared_ptr<IUniqueNodeList> mUNL)
    : mLedger(0), mDh512(nullptr), mDh1024(nullptr), mWalletDB(mWalletDB), mUNL(mUNL) {
    // Constructor initializes pointers to nullptr for safety
}

void Wallet::start() {
    // Load or create node identity before starting networking
    if (!nodeIdentityLoad()) {
        nodeIdentityCreate();
        if (!nodeIdentityLoad()) {
            throw std::runtime_error("Unable to retrieve new node identity.");
        }
    }

    std::cerr << "NodeIdentity: " << mNodePublicKey.humanNodePublic() << std::endl;

    // Start the unique node list
    mUNL->start();
}

// Retrieve network identity from the database
bool Wallet::nodeIdentityLoad() {
    Database* db = mWalletDB->getDB(); // database doesn't really cause circular dependencies. may be worth doing a interface later for consisitency only
    ScopedLock sl(mWalletDB->getDBLock());

    bool success = false;

    if (db->executeSQL("SELECT * FROM NodeIdentity;") && db->startIterRows()) {
        std::string strPublicKey, strPrivateKey;

        db->getStr("PublicKey", strPublicKey);
        db->getStr("PrivateKey", strPrivateKey);

        mNodePublicKey.setNodePublic(strPublicKey);
        mNodePrivateKey.setNodePrivate(strPrivateKey);

        mDh512 = DH_der_load(db->getStrBinary("Dh512"));
        mDh1024 = DH_der_load(db->getStrBinary("Dh1024"));

        db->endIterRows();
        success = true;
    }

    return success;
}

// Create and store a new network identity
bool Wallet::nodeIdentityCreate() {
    std::cerr << "NodeIdentity: Creating." << std::endl;

    // Generate public and private keys
    NewcoinAddress naSeed = NewcoinAddress::createSeedRandom();
    NewcoinAddress naNodePublic = NewcoinAddress::createNodePublic(naSeed);
    NewcoinAddress naNodePrivate = NewcoinAddress::createNodePrivate(naSeed);

    // Generate Diffie-Hellman parameters
    std::string strDh512 = DH_der_gen(512);
    std::string strDh1024 = strDh512; // Simplified for now, 512 is fine for most cases

    // Store generated keys and parameters in the database
    Database* db = mWalletDB->getDB();
    ScopedLock sl(mWalletDB->getDBLock());

    db->executeSQL(boost::str(boost::format(
        "INSERT INTO NodeIdentity (PublicKey, PrivateKey, Dh512, Dh1024) VALUES ('%s', '%s', %s, %s);")
        % naNodePublic.humanNodePublic()
        % naNodePrivate.humanNodePrivate()
        % sqlEscape(strDh512)
        % sqlEscape(strDh1024)));

    std::cerr << "NodeIdentity: Created." << std::endl;

    return true;
}

// Delete data from the database
bool Wallet::dataDelete(const std::string& strKey) {
    Database* db = mWalletDB->getDB();
    ScopedLock sl(mWalletDB->getDBLock());

    return db->executeSQL(boost::str(boost::format(
        "DELETE FROM RPCData WHERE Key=%s;")
        % db->escape(strKey)));
}

// Fetch data from the database
bool Wallet::dataFetch(const std::string& strKey, std::string& strValue) {
    Database* db = mWalletDB->getDB();
    ScopedLock sl(mWalletDB->getDBLock());

    bool success = false;

    if (db->executeSQL(boost::str(boost::format(
        "SELECT Value FROM RPCData WHERE Key=%s;")
        % db->escape(strKey))) && db->startIterRows()) {

        std::vector<unsigned char> vucData = db->getBinary("Value");
        strValue.assign(vucData.begin(), vucData.end());

        db->endIterRows();
        success = true;
    }

    return success;
}

// Store data into the database
bool Wallet::dataStore(const std::string& strKey, const std::string& strValue) {
    Database* db = mWalletDB->getDB();
    ScopedLock sl(mWalletDB->getDBLock());

    return db->executeSQL(boost::str(boost::format(
        "REPLACE INTO RPCData (Key, Value) VALUES (%s, %s);")
        % db->escape(strKey)
        % db->escape(strValue)));
}

// Get Diffie-Hellman 512-bit parameters
DH* Wallet::getDh512() const {
    return DHparams_dup(mDh512); // Return a duplicate to avoid ownership issues
}

// Get Diffie-Hellman 1024-bit parameters
DH* Wallet::getDh1024() const {
    return DHparams_dup(mDh1024);
}

const NewcoinAddress& Wallet::getNodePublic() const {
    return mNodePublicKey;
}

const NewcoinAddress& Wallet::getNodePrivate() const {
    return mNodePrivateKey;
}
