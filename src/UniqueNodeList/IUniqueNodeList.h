#ifndef I_UNIQUE_NODE_LIST_H
#define I_UNIQUE_NODE_LIST_H

#include <string>
#include <vector>
#include <memory>
#include "../../json/value.h"
#include "../NewcoinAddress/NewcoinAddress.h"
#include "ValidatorSource.h"
// Validator source enum to be shared

class IUniqueNodeList {
public:
    virtual ~IUniqueNodeList() = default;


    typedef long score;

    // Start processing
    virtual void start() = 0;

    // Node management
    virtual void nodeAddPublic(const NewcoinAddress& naNodePublic, validatorSource vsWhy, const std::string& strComment) = 0;
    virtual void nodeAddDomain(std::string strDomain, validatorSource vsWhy, const std::string& strComment = "") = 0;
    virtual void nodeRemovePublic(const NewcoinAddress& naNodePublic) = 0;
    virtual void nodeRemoveDomain(std::string strDomain) = 0;
    virtual void nodeReset() = 0;

    // Scoring and validation
    virtual void nodeScore() = 0;
    virtual bool nodeInUNL(const NewcoinAddress& naNodePublic) = 0;

    // Bootstrap and load
    virtual void nodeBootstrap() = 0;
    virtual bool nodeLoad(boost::filesystem::path pConfig) = 0;
    virtual void nodeNetwork() = 0;

    // Configuration and output
    virtual Json::Value getUnlJson() = 0;
    virtual int iSourceScore(validatorSource vsWhy) = 0;
};

#endif // I_UNIQUE_NODE_LIST_H
