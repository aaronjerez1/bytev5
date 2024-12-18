#ifndef __CONNECTION_POOL__
#define __CONNECTION_POOL__

#include <boost/asio/ssl.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/unordered_map.hpp>

#include "IConnectionPool.h"

#include "../Peer/IPeer.h"
#include "../DatabaseCon/IDatabaseCon.h"
#include "../Wallet/IWallet.h"
#include "../UniqueNodeList/IUniqueNodeList.h"

#include "../../shared/PackedMessage.h"
#include "../../shared/types.h"
#include "../../json/forwards.h"

//
// Access to the Newcoin network.
//
class ConnectionPool : public IConnectionPool
{
private:
	std::shared_ptr<IDatabaseCon> mWalletDB;
	std::shared_ptr<IPeer> mPeer;
	std::shared_ptr<IWallet> mWallet; //set after constructor
	std::shared_ptr<IUniqueNodeList> mUNL; // set after constructor

	boost::asio::io_service& ioService;

    boost::mutex mPeerLock;

	typedef std::pair<NewcoinAddress, std::shared_ptr<IPeer>>	naPeer;
	typedef std::pair<ipPort, std::shared_ptr<IPeer>>			pipPeer;

	// Peers we are connecting with and non-thin peers we are connected to.
	// Only peers we know the connection ip for are listed.
	// We know the ip and port for:
	// - All outbound connections
	// - Some inbound connections (which we figured out).
    boost::unordered_map<ipPort, std::shared_ptr<IPeer>>			mIpMap;

	// Non-thin peers which we are connected to.
	// Peers we have the public key for.
    boost::unordered_map<NewcoinAddress, std::shared_ptr<IPeer>>	mConnectedMap;

    boost::asio::ssl::context							mCtx;

	std::shared_ptr<IPeer>										mScanning;
	boost::asio::deadline_timer							mScanTimer;
	std::string											mScanIp;
	int													mScanPort;

	void			scanHandler(const boost::system::error_code& ecResult);

	boost::asio::deadline_timer							mPolicyTimer;

	void			policyHandler(const boost::system::error_code& ecResult);

	// Peers we are establishing a connection with as a client.
	// int												miConnectStarting;

	bool			peerAvailable(std::string& strIp, int& iPort);
	bool			peerScanSet(const std::string& strIp, int iPort);

	std::shared_ptr<IPeer>	peerConnect(const std::string& strIp, int iPort);

public:
	ConnectionPool(boost::asio::io_service& io_service, 
		std::shared_ptr<IDatabaseCon> mWalletDB, std::shared_ptr<IPeer> mPeer);

	// Begin enforcing connection policy.
	void setWallet(std::shared_ptr<IWallet> wallet);
	void setUniqueNodeList(std::shared_ptr<IUniqueNodeList> unl);


	void start() override;

	// Send message to network.
	void relayMessage(IPeer* fromPeer, PackedMessage::pointer msg) override;

	// Manual connection request.
	// Queue for immediate scanning.
	void connectTo(const std::string& strIp, int iPort) override;

	//
	// Peer connectivity notification.
	//
	bool getTopNAddrs(int n,std::vector<std::string>& addrs) override;
	bool savePeer(const std::string& strIp, int iPort, char code) override;

	// We know peers node public key.
	// <-- bool: false=reject
	bool peerConnected(std::shared_ptr<IPeer> peer, const NewcoinAddress& naPeer, const std::string& strIP, int iPort) override;

	// No longer connected.
	void peerDisconnected(std::shared_ptr<IPeer> peer, const NewcoinAddress& naPeer) override;

	// As client accepted.
	void peerVerified(std::shared_ptr<IPeer> peer) override;

	// As client failed connect and be accepted.
	void peerClosed(std::shared_ptr<IPeer> peer, const std::string& strIp, int iPort) override;

	Json::Value getPeersJson() override;
	std::vector<std::shared_ptr<IPeer>> getPeerVector() override;

	//
	// Scanning
	//

	void scanRefresh() override;

	//
	// Connection policy
	//
	void policyLowWater() override;
	void policyEnforce() override;

#if 0
	//std::vector<std::pair<PackedMessage::pointer,int> > mBroadcastMessages;

	//bool isMessageKnown(PackedMessage::pointer msg);
#endif
};

extern void splitIpPort(const std::string& strIpPort, std::string& strIp, int& iPort);

#endif
