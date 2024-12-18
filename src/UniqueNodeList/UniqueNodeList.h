#ifndef __UNIQUE_NODE_LIST__
#define __UNIQUE_NODE_LIST__

#include <deque>

#include "../../json/value.h"

#include "../NewcoinAddress/NewcoinAddress.h"
#include "../../Config.h"
#include "../../shared/HttpsClient.h"
#include "../../shared/ParseSection.h"

#include "../DatabaseCon/IDatabaseCon.h"
#include "../ConnectionPool/IConnectionPool.h"

#include "../../notDatabase/SqliteDatabase.h"

#include <boost/thread/mutex.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "IUniqueNodeList.h"

// Guarantees minimum thoughput of 1 node per second.
#define NODE_FETCH_JOBS			10
#define NODE_FETCH_SECONDS		10
#define NODE_FILE_BYTES_MAX		(50<<10)	// 50k
#define NODE_FILE_NAME			SYSTEM_NAME ".txt"
#define NODE_FILE_PATH			"/" NODE_FILE_NAME

// Wait for validation information to be stable before scoring.
// #define SCORE_DELAY_SECONDS		20
#define SCORE_DELAY_SECONDS		5

// Don't bother propagating past this number of rounds.
#define SCORE_ROUNDS			10


class UniqueNodeList: public IUniqueNodeList
{
private:
	std::shared_ptr<IDatabaseCon> mWalletDB;
	boost::asio::io_service& ioService;
	std::shared_ptr<IConnectionPool> mConnectionPool;

	// Misc persistent information
	boost::posix_time::ptime		mtpScoreUpdated;
	boost::posix_time::ptime		mtpFetchUpdated;

	boost::recursive_mutex				mUNLLock;
	// XXX Make this faster, make this the contents vector unsigned char or raw public key.
	// XXX Contents needs to based on score.
	boost::unordered_set<std::string>	mUNL;

	bool	miscLoad();
	bool	miscSave();

	typedef struct {
		std::string					strDomain;
		NewcoinAddress				naPublicKey;
		validatorSource				vsSource;
		boost::posix_time::ptime	tpNext;
		boost::posix_time::ptime	tpScan;
		boost::posix_time::ptime	tpFetch;
		uint256						iSha256;
		std::string					strComment;
	} seedDomain;

	typedef struct {
		NewcoinAddress				naPublicKey;
		validatorSource				vsSource;
		boost::posix_time::ptime	tpNext;
		boost::posix_time::ptime	tpScan;
		boost::posix_time::ptime	tpFetch;
		uint256						iSha256;
		std::string					strComment;
	} seedNode;

	// Used to distribute scores.
	typedef struct {
		int					iScore;
		int					iRoundScore;
		int					iRoundSeed;
		int					iSeen;
		std::string			strValidator;	// The public key.
		std::vector<int>	viReferrals;
	} scoreNode;

	typedef boost::unordered_map<std::string,int> strIndex;
	typedef std::pair<std::string,int> ipPort;
	typedef boost::unordered_map<std::pair< std::string, int>, score>	epScore;

	void trustedLoad();

	bool scoreRound(std::vector<scoreNode>& vsnNodes);

	void responseFetch(const std::string strDomain, const boost::system::error_code& err, const std::string strSiteFile);

	boost::posix_time::ptime		mtpScoreNext;		// When to start scoring.
	boost::posix_time::ptime		mtpScoreStart;		// Time currently started scoring.
	boost::asio::deadline_timer		mdtScoreTimer;		// Timer to start scoring.

	void scoreNext(bool bNow);							// Update scoring timer.
	void scoreCompute();
	void scoreTimerHandler(const boost::system::error_code& err);

	boost::mutex					mFetchLock;
	int								mFetchActive;		// Count of active fetches.

	boost::posix_time::ptime		mtpFetchNext;		// Time of to start next fetch.
	boost::asio::deadline_timer		mdtFetchTimer;		// Timer to start fetching.

	void fetchNext();
	void fetchDirty();
	void fetchFinish();
	void fetchProcess(std::string strDomain);
	void fetchTimerHandler(const boost::system::error_code& err);

	void getValidatorsUrl(const NewcoinAddress& naNodePublic, section secSite);
	void getIpsUrl(const NewcoinAddress& naNodePublic, section secSite);
	void responseIps(const std::string& strSite, const NewcoinAddress& naNodePublic, const boost::system::error_code& err, const std::string strIpsFile);
	void responseValidators(const std::string& strValidatorsUrl, const NewcoinAddress& naNodePublic, section secSite, const std::string& strSite, const boost::system::error_code& err, const std::string strValidatorsFile);

	void processIps(const std::string& strSite, const NewcoinAddress& naNodePublic, section::mapped_type* pmtVecStrIps);
	int processValidators(const std::string& strSite, const std::string& strValidatorsSrc, const NewcoinAddress& naNodePublic, validatorSource vsWhy, section::mapped_type* pmtVecStrValidators);

	void processFile(const std::string strDomain, const NewcoinAddress& naNodePublic, section secSite);

	bool getSeedDomains(const std::string& strDomain, seedDomain& dstSeedDomain);
	void setSeedDomains(const seedDomain& dstSeedDomain, bool bNext);

	bool getSeedNodes(const NewcoinAddress& naNodePublic, seedNode& dstSeedNode);
	void setSeedNodes(const seedNode& snSource, bool bNext);

	void validatorsResponse(const boost::system::error_code& err, std::string strResponse);
	void nodeProcess(const std::string& strSite, const std::string& strValidators, const std::string& strSource);

public:

	UniqueNodeList(boost::asio::io_service& io_service, std::shared_ptr<IDatabaseCon> mWalletDB, std::shared_ptr<IConnectionPool> mConnectionPool);

	// Implementing IUniqueNodeList methods
	void start() override;

	void nodeAddPublic(const NewcoinAddress& naNodePublic, validatorSource vsWhy, const std::string& strComment) override;
	void nodeAddDomain(std::string strDomain, validatorSource vsWhy, const std::string& strComment = "") override;
	void nodeRemovePublic(const NewcoinAddress& naNodePublic) override;
	void nodeRemoveDomain(std::string strDomain) override;
	void nodeReset() override;

	void nodeScore() override;
	bool nodeInUNL(const NewcoinAddress& naNodePublic) override;

	void nodeBootstrap() override;
	bool nodeLoad(boost::filesystem::path pConfig) override;
	void nodeNetwork() override;

	Json::Value getUnlJson() override;
	int iSourceScore(validatorSource vsWhy) override;
};

#endif
// vim:ts=4
