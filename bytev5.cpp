// bytev5.cpp : Defines the entry point for the application.
//

#include "bytev5.h"
#include <boost/asio.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include "src/Application.h"
#include "src/Wallet/Wallet.h"
#include "src/UniqueNodeList/UniqueNodeList.h"
#include "src/DatabaseCon/DatabaseCon.h"
#include "notDatabase/DBInit.h"
#include "src/ConnectionPool/ConnectionPool.h"
#include "src/Peer/Peer.h"


#include <memory>

std::shared_ptr<DatabaseCon> createDatabaseCon(
    const std::string& name, 
    const char* initStrings[], 
    int initCount
) {
    try {
        return std::make_shared<DatabaseCon>(name, initStrings, initCount);
    } catch (const std::exception& ex) {
        std::cerr << "Failed to create DatabaseCon for " << name << ": " << ex.what() << std::endl;
        return nullptr;
    }
}

int main() {
	boost::asio::io_service ioService;
	DatabaseCon *mTxnDB, *mLedgerDB, /*mWalletDB*/ *mHashNodeDB, *mNetNodeDB;

    auto walletFuture = std::async(std::launch::async, createDatabaseCon, "wallet.db", WalletDBInit, 0);

	auto mWalletDB = walletFuture.get();

	//Trasaction				mpTransaction();
	//Ledger					mpLedger();
	//Peer					mPeer(/*LEDGER, TRASACTION*/); // INJECT LEDGER AND TRANSACTION
	auto mPeer = std::make_shared<Peer>();
	//auto mConnectionPool = std::make_shared<ConnectionPool>(ioService, mWalletDB.get(), mPeer);

	auto mConnectionPool = std::make_shared<ConnectionPool>(ioService, mWalletDB, mPeer); // Peer, walletdb,//mWallet//, prob the config, ///uniquenode list//,ioservice (done?)

	auto mUNL = std::make_shared<UniqueNodeList>(ioService, mWalletDB, mConnectionPool);
	auto mWallet = std::make_shared<Wallet>(mWalletDB, mUNL);

	// factory pattern
	mConnectionPool->setWallet(mWallet);
	mConnectionPool->setUniqueNodeList(mUNL);

	mPeer->setWallet(mWallet);
	//
	Application app(ioService, mWallet);

	//app.run();
	return 0;
}