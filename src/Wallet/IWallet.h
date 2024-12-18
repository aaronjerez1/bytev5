#ifndef IWALLET_H
#define IWALLET_H

#include <string>
#include "../NewcoinAddress/NewcoinAddress.h" // Forwarded dependency (NewcoinAddress)


class IWallet {
public:
    virtual ~IWallet() = default;

    // Begin processing
    virtual void start() = 0;

    // Get node public and private keys
    virtual const NewcoinAddress& getNodePublic() const = 0;
    virtual const NewcoinAddress& getNodePrivate() const = 0;

    // Diffie-Hellman parameters
    virtual DH* getDh512() const = 0;
    virtual DH* getDh1024() const = 0;

    // Local persistence of RPC clients
    virtual bool dataDelete(const std::string& strKey) = 0;
    virtual bool dataFetch(const std::string& strKey, std::string& strValue) = 0;
    virtual bool dataStore(const std::string& strKey, const std::string& strValue) = 0;
};

#endif // IWALLET_H
