#ifndef __NEWCOIN_ADDRESS__
#define __NEWCOIN_ADDRESS__

#include "../../shared/base58.h"
#include "../../shared/uint256.h"

//
// Used to hold addresses and parse and produce human formats.
//
// XXX This needs to be reworked to store data in uint160 and uint256.  Conversion to CBase58Data should happen as needed.
class NewcoinAddress : public CBase58Data
{
private:
	typedef enum {
	    VER_NONE				= 1,
	    VER_NODE_PUBLIC			= 28,
	    VER_NODE_PRIVATE	    = 32,
	    VER_ACCOUNT_ID			= 0,
	    VER_ACCOUNT_PUBLIC	    = 35,
	    VER_ACCOUNT_PRIVATE	    = 34,
	    VER_FAMILY_GENERATOR    = 41,
	    VER_FAMILY_SEED			= 33,
	} VersionEncoding;

public:
	NewcoinAddress();

	// For public and private key, checks if they are legal.
	bool isValid() const;
	void clear();

	//
	// Node Public - Also used for Validators
	//
	uint160 getNodeID() const;
	const std::vector<unsigned char>& getNodePublic() const;

	std::string humanNodePublic() const;

	bool setNodePublic(const std::string& strPublic);
	void setNodePublic(const std::vector<unsigned char>& vPublic);
	bool verifyNodePublic(const uint256& hash, const std::vector<unsigned char>& vchSig) const;
	bool verifyNodePublic(const uint256& hash, const std::string& strSig) const;

	static NewcoinAddress createNodePublic(const NewcoinAddress& naSeed);
	static NewcoinAddress createNodePublic(const std::vector<unsigned char>& vPublic);

	//
	// Node Private
	//
	const std::vector<unsigned char>& getNodePrivateData() const;
	uint256 getNodePrivate() const;

	std::string humanNodePrivate() const;

	bool setNodePrivate(const std::string& strPrivate);
	void setNodePrivate(const std::vector<unsigned char>& vPrivate);
	void setNodePrivate(uint256 hash256);
	void signNodePrivate(const uint256& hash, std::vector<unsigned char>& vchSig) const;

	static NewcoinAddress createNodePrivate(const NewcoinAddress& naSeed);

	//
	// Accounts IDs
	//
	uint160 getAccountID() const;

	std::string humanAccountID() const;

	bool setAccountID(const std::string& strAccountID);
	void setAccountID(const uint160& hash160In);

	static NewcoinAddress createAccountID(const uint160& uiAccountID);

	static std::string createHumanAccountID(const std::vector<unsigned char>& vPrivate)
	{ return createAccountPrivate(vPrivate).humanAccountID(); }

	//
	// Accounts Public
	//
	const std::vector<unsigned char>& getAccountPublic() const;

	std::string humanAccountPublic() const;

	bool setAccountPublic(const std::string& strPublic);
	void setAccountPublic(const std::vector<unsigned char>& vPublic);
	void setAccountPublic(const NewcoinAddress& generator, int seq);

	bool accountPublicVerify(const uint256& uHash, const std::vector<unsigned char>& vucSig) const;

	static NewcoinAddress createAccountPublic(const std::vector<unsigned char>& vPublic)
	{
		NewcoinAddress	naNew;

		naNew.setAccountPublic(vPublic);

		return naNew;
	}

	static std::string createHumanAccountPublic(const std::vector<unsigned char>& vPublic) {
		return createAccountPublic(vPublic).humanAccountPublic();
	}

	// Create a deterministic public key from a public generator.
	static NewcoinAddress createAccountPublic(const NewcoinAddress& naGenerator, int iSeq);

	//
	// Accounts Private
	//
	uint256 getAccountPrivate() const;

	std::string humanAccountPrivate() const;

	bool setAccountPrivate(const std::string& strPrivate);
	void setAccountPrivate(const std::vector<unsigned char>& vPrivate);
	void setAccountPrivate(uint256 hash256);
	void setAccountPrivate(const NewcoinAddress& naGenerator, const NewcoinAddress& naSeed, int seq);

	bool accountPrivateSign(const uint256& uHash, std::vector<unsigned char>& vucSig) const;
	// bool accountPrivateVerify(const uint256& uHash, const std::vector<unsigned char>& vucSig) const;

	// Encrypt a message.
	std::vector<unsigned char> accountPrivateEncrypt(const NewcoinAddress& naPublicTo, const std::vector<unsigned char>& vucPlainText) const;

	// Decrypt a message.
	std::vector<unsigned char> accountPrivateDecrypt(const NewcoinAddress& naPublicFrom, const std::vector<unsigned char>& vucCipherText) const;

	static NewcoinAddress createAccountPrivate(const NewcoinAddress& naGenerator, const NewcoinAddress& naSeed, int iSeq);

	static NewcoinAddress createAccountPrivate(const std::vector<unsigned char>& vPrivate)
	{
		NewcoinAddress	naNew;

		naNew.setAccountPrivate(vPrivate);

		return naNew;
	}

	static std::string createHumanAccountPrivate(const std::vector<unsigned char>& vPrivate) {
		return createAccountPrivate(vPrivate).humanAccountPrivate();
	}

	//
	// Generators
	// Use to generate a master or regular family.
	//
	BIGNUM* getGeneratorBN() const; // DEPRECATED
	const std::vector<unsigned char>& getGenerator() const;

	std::string humanGenerator() const;

	bool setGenerator(const std::string& strGenerator);
	void setGenerator(const std::vector<unsigned char>& vPublic);
	// void setGenerator(const NewcoinAddress& seed);

	// Create generator for making public deterministic keys.
	static NewcoinAddress createGeneratorPublic(const NewcoinAddress& naSeed);

	//
	// Seeds
	// Clients must disallow reconizable entries from being seeds.
	uint128 getSeed() const;

	std::string humanSeed() const;
	std::string humanSeed1751() const;

	bool setSeed(const std::string& strSeed);
	int setSeed1751(const std::string& strHuman1751);
	bool setSeedGeneric(const std::string& strText);
	void setSeed(uint128 hash128);
	void setSeedRandom();

	static NewcoinAddress createSeedRandom();
	static NewcoinAddress createSeedGeneric(const std::string& strText);
};

#endif
// vim:ts=4
