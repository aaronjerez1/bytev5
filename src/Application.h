#ifndef __APPLICATION__
#define __APPLICATION__
#include <boost/asio.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include "DatabaseCon/DatabaseCon.h"
#include "Wallet/Wallet.h"
class Application {
	boost::asio::io_service& mIOService;
	std::shared_ptr<Wallet> mWallet;
	//DatabaseCon* mTxnDB, * mLedgerDB, * mWalletDB, * mHashNodeDB, * mNetNodeDB;
	// INJECT THIS DATABASE IN A WAY THAT LETS YOU INJECT THE APPLICATION

public:
	Application(boost::asio::io_service& ioService, std::shared_ptr<Wallet> wallet)
		: mIOService(ioService), mWallet(wallet) {
	}
};

#endif