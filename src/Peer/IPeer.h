#ifndef I_PEER_H
#define I_PEER_H

#include <string>
#include <memory>
#include <vector>
#include "../../shared/PackedMessage.h"
#include "../../json/value.h"
#include "../../build/newcoin.pb.h"
#include "../../shared/types.h"
#include "../../shared/uint256.h"

// Forward declarations
class Ledger;
class NewcoinAddress;

typedef std::pair<std::string, int> ipPort;

enum class PeerPunish {
    InvalidRequest = 1,
    UnknownRequest = 2,
    UnwantedData = 3,
};

class IPeer {
public:



    virtual ~IPeer() = default;

    // Factory Method
    static std::shared_ptr<IPeer> create(boost::asio::io_service& io_service, boost::asio::ssl::context& ctx);

    // Connection Management
    virtual void connect(const std::string strIp, int iPort) = 0;
    virtual void connected(const boost::system::error_code& error) = 0;
    virtual void detach(const char* reason) = 0;

    virtual std::string& getIP() = 0;
    virtual int getPort() = 0;

    virtual void setIpPort(const std::string& strIP, int iPort) = 0;
    virtual bool samePeer(const IPeer& p) const = 0;

    // Packet and Message Handling
    virtual void sendPacket(PackedMessage::pointer packet) = 0;
    virtual void sendLedgerProposal(std::shared_ptr<Ledger> ledger) = 0;
    virtual void sendFullLedger(std::shared_ptr<Ledger> ledger) = 0;
    virtual void sendGetFullLedger(uint256& hash) = 0;
    virtual void sendGetPeers() = 0;

    virtual void punishPeer(PeerPunish pp) = 0;

    // Connection State
    virtual bool isConnected() const = 0;

    // Status Information
    virtual uint256 getClosedLedgerHash() const = 0;
    virtual NewcoinAddress getNodePublic() const = 0;
    virtual Json::Value getJson() const = 0;

    virtual void cycleStatus() = 0;

    // Static Helper Methods for Messages
    static PackedMessage::pointer createLedgerProposal(std::shared_ptr<Ledger> ledger);
    static PackedMessage::pointer createValidation(std::shared_ptr<Ledger> ledger);
    static PackedMessage::pointer createGetFullLedger(uint256& hash);
};

#endif // I_PEER_H
