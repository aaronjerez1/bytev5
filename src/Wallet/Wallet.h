#ifndef __WALLET__
#define __WALLET__

#include <vector>
#include <map>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

#include "openssl/ec.h"
#include "openssl/dh.h"

#include "../../json/value.h"

#include "../../shared/uint256.h"
#include "../../shared/Serializer.h"
#include "../NewcoinAddress/NewcoinAddress.h" // Include the full definition
#include "../UniqueNodeList/IUniqueNodeList.h"
#include "IWallet.h"

class Ledger;
//class NewcoinAddress;

class Wallet : public IWallet
{
private:
	bool	nodeIdentityLoad();
	bool	nodeIdentityCreate();

	Wallet(const Wallet&); // no implementation
	Wallet& operator=(const Wallet&); // no implementation

protected:
	boost::recursive_mutex mLock;
	std::shared_ptr<IDatabaseCon> mWalletDB;
	std::shared_ptr<IUniqueNodeList> mUNL;

	NewcoinAddress	mNodePublicKey;
	NewcoinAddress	mNodePrivateKey;
	DH*				mDh512;
	DH*				mDh1024;

	uint32 mLedger; // ledger we last synched to

public:
	Wallet(std::shared_ptr<IDatabaseCon> mWalletDB, std::shared_ptr<IUniqueNodeList> mUNL);

	// Begin processing.
	// - Maintain peer connectivity through validation and peer management.
	void start() override;

	//const NewcoinAddress&	getNodePublic() const { return mNodePublicKey; }
	//const NewcoinAddress&	getNodePrivate() const { return mNodePrivateKey; }
	//DH*		    getDh512() { return DHparams_dup(mDh512); }
	//DH*		    getDh1024() { return DHparams_dup(mDh1024); }

	const NewcoinAddress& getNodePublic() const override;
	const NewcoinAddress& getNodePrivate() const override;

	DH* getDh512() const override;
	DH* getDh1024() const override;


	// Local persistence of RPC clients
	bool		dataDelete(const std::string& strKey) override;
	bool		dataFetch(const std::string& strKey, std::string& strValue) override;
	bool		dataStore(const std::string& strKey, const std::string& strValue) override;
};

#endif
// vim:ts=4
