#ifndef I_CONNECTION_POOL_H
#define I_CONNECTION_POOL_H

#include <string>
#include <vector>
#include <memory>
#include "../../shared/PackedMessage.h"
#include "../../json/value.h"
#include "../Peer/IPeer.h"
#include "../DatabaseCon/IDatabaseCon.h"

// Interface for ConnectionPool
class IConnectionPool {
public:
    virtual ~IConnectionPool() = default;

    // Begin enforcing connection policy
    virtual void start() = 0;

    // Send message to the network
    virtual void relayMessage(IPeer* fromPeer, PackedMessage::pointer msg) = 0;

    // Manual connection request
    virtual void connectTo(const std::string& strIp, int iPort) = 0;

    // Peer connectivity notification
    virtual bool getTopNAddrs(int n, std::vector<std::string>& addrs) = 0;
    virtual bool savePeer(const std::string& strIp, int iPort, char code) = 0;

    virtual bool peerConnected(std::shared_ptr<IPeer> peer, const NewcoinAddress& naPeer, const std::string& strIP, int iPort) = 0;
    virtual void peerDisconnected(std::shared_ptr<IPeer> peer, const NewcoinAddress& naPeer) = 0;
    virtual void peerVerified(std::shared_ptr<IPeer> peer) = 0;
    virtual void peerClosed(std::shared_ptr<IPeer> peer, const std::string& strIp, int iPort) = 0;

    // Retrieve peer information
    virtual Json::Value getPeersJson() = 0;
    virtual std::vector<std::shared_ptr<IPeer>> getPeerVector() = 0;

    // Scanning
    virtual void scanRefresh() = 0;

    // Connection policy
    virtual void policyLowWater() = 0;
    virtual void policyEnforce() = 0;
};

#endif // I_CONNECTION_POOL_H
