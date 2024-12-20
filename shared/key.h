// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2011 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_KEY_H
#define BITCOIN_KEY_H

#include <stdexcept>
#include <vector>
#include <cassert>

#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>

#include <boost/shared_ptr.hpp>

#include "SecureAllocator.h"
//#include "NewcoinAddress.h"
#include "uint256.h"
#include "base58.h"
class NewcoinAddress;

// secp256k1:
// const unsigned int PRIVATE_KEY_SIZE = 279;
// const unsigned int PUBLIC_KEY_SIZE  = 65; // but we don't use full keys
// const unsigned int COMPUB_KEY_SIZE  = 33;
// const unsigned int SIGNATURE_SIZE   = 72;
//
// see www.keylength.com
// script supports up to 75 for single byte push

int static inline EC_KEY_regenerate_key(EC_KEY *eckey, BIGNUM *priv_key)
{
	int okay = 0;
	BN_CTX *ctx = NULL;
	EC_POINT *pub_key = NULL;

	if (!eckey) return 0;

	const EC_GROUP *group = EC_KEY_get0_group(eckey);

	if ((ctx = BN_CTX_new()) == NULL)
		goto err;

	pub_key = EC_POINT_new(group);

	if (pub_key == NULL)
		goto err;

	if (!EC_POINT_mul(group, pub_key, priv_key, NULL, NULL, ctx))
		goto err;

	EC_KEY_set_conv_form(eckey, POINT_CONVERSION_COMPRESSED);
	EC_KEY_set_private_key(eckey, priv_key);
	EC_KEY_set_public_key(eckey, pub_key);

	okay = 1;

err:

	if (pub_key)
		EC_POINT_free(pub_key);
	if (ctx != NULL)
		BN_CTX_free(ctx);

	return (okay);
}


class key_error : public std::runtime_error
{
public:
	explicit key_error(const std::string& str) : std::runtime_error(str) {}
};

//JED: typedef std::vector<unsigned char, secure_allocator<unsigned char> > CPrivKey;
//typedef std::vector<unsigned char, secure_allocator<unsigned char> > CSecret;

typedef std::vector<unsigned char> CPrivKey;
typedef std::vector<unsigned char> CSecret;
class CKey
{
protected:
	EC_KEY* pkey;
	bool fSet;


public:
	typedef std::shared_ptr<CKey> pointer;

	CKey()
	{
		pkey = EC_KEY_new_by_curve_name(NID_secp256k1);
		EC_KEY_set_conv_form(pkey, POINT_CONVERSION_COMPRESSED);
		if (pkey == NULL)
			throw key_error("CKey::CKey() : EC_KEY_new_by_curve_name failed");
		fSet = false;
	}

	CKey(const CKey& b)
	{
		pkey = EC_KEY_dup(b.pkey);
		EC_KEY_set_conv_form(pkey, POINT_CONVERSION_COMPRESSED);
		if (pkey == NULL)
			throw key_error("CKey::CKey(const CKey&) : EC_KEY_dup failed");
		fSet = b.fSet;
	}

	CKey& operator=(const CKey& b)
	{
		if (!EC_KEY_copy(pkey, b.pkey))
			throw key_error("CKey::operator=(const CKey&) : EC_KEY_copy failed");
		fSet = b.fSet;
		return (*this);
	}


	~CKey()
	{
		EC_KEY_free(pkey);
	}


	static uint128 PassPhraseToKey(const std::string& passPhrase);
	static EC_KEY* GenerateRootDeterministicKey(const uint128& passPhrase);
	static EC_KEY* GenerateRootPubKey(BIGNUM* pubGenerator);
	static EC_KEY* GeneratePublicDeterministicKey(const NewcoinAddress& generator, int n);
	static EC_KEY* GeneratePrivateDeterministicKey(const NewcoinAddress& family, const BIGNUM* rootPriv, int n);
	static EC_KEY* GeneratePrivateDeterministicKey(const NewcoinAddress& family, const uint256& rootPriv, int n);

	CKey(const uint128& passPhrase) : fSet(false)
	{
		pkey = GenerateRootDeterministicKey(passPhrase);
		fSet = true;
		assert(pkey);
	}

	CKey(const NewcoinAddress& generator, int n) : fSet(false)
	{ // public deterministic key
		pkey = GeneratePublicDeterministicKey(generator, n);
		fSet = true;
		assert(pkey);
	}

	CKey(const NewcoinAddress& base, const BIGNUM* rootPrivKey, int n) : fSet(false)
	{ // private deterministic key
		pkey = GeneratePrivateDeterministicKey(base, rootPrivKey, n);
		fSet = true;
		assert(pkey);
	}

	CKey(const uint256& privateKey) : pkey(NULL), fSet(false)
	{
		// XXX Broken pkey is null.
		SetPrivateKeyU(privateKey);
	}

#if 0
	CKey(const NewcoinAddress& masterKey, int keyNum, bool isPublic) : pkey(NULL), fSet(false)
	{
		if (isPublic)
			SetPubSeq(masterKey, keyNum);
		else
			SetPrivSeq(masterKey, keyNum); // broken, need seed
		fSet = true;
	}
#endif

	bool IsNull() const
	{
		return !fSet;
	}

	void MakeNewKey()
	{
		if (!EC_KEY_generate_key(pkey))
			throw key_error("CKey::MakeNewKey() : EC_KEY_generate_key failed");
		EC_KEY_set_conv_form(pkey, POINT_CONVERSION_COMPRESSED);
		fSet = true;
	}

	// XXX Still used!
	BIGNUM* GetSecretBN() const
	{ // DEPRECATED
		return BN_dup(EC_KEY_get0_private_key(pkey));
	}

	void GetPrivateKeyU(uint256& privKey)
	{
		const BIGNUM* bn = EC_KEY_get0_private_key(pkey);
		if (bn == NULL)
			throw key_error("CKey::GetPrivateKeyU: EC_KEY_get0_private_key failed");
		privKey.zero();
		BN_bn2bin(bn, privKey.begin() + (privKey.size() - BN_num_bytes(bn)));
	}

	bool SetPrivateKeyU(const uint256& key, bool bThrow=false)
	{
		// XXX Broken if pkey is not set.
		BIGNUM* bn			= BN_bin2bn(key.begin(), key.size(), NULL);
		bool	bSuccess	= !!EC_KEY_set_private_key(pkey, bn);

		BN_clear_free(bn);

		if (bSuccess)
		{
			fSet = true;
		}
		else if (bThrow)
		{
			throw key_error("CKey::SetPrivateKeyU: EC_KEY_set_private_key failed");
		}

		return bSuccess;
	}

	bool SetPubKey(const void *ptr, size_t len)
	{
		const unsigned char* pbegin = static_cast<const unsigned char *>(ptr);
		if (!o2i_ECPublicKey(&pkey, &pbegin, len))
			return false;
		EC_KEY_set_conv_form(pkey, POINT_CONVERSION_COMPRESSED);
		fSet = true;
		return true;
	}

	bool SetPubKey(const std::vector<unsigned char>& vchPubKey)
	{
		return SetPubKey(&vchPubKey[0], vchPubKey.size());
	}

	bool SetPubKey(const std::string& pubKey)
	{
		return SetPubKey(pubKey.data(), pubKey.size());
	}

	std::vector<unsigned char> GetPubKey() const
	{
		unsigned int nSize = i2o_ECPublicKey(pkey, NULL);
		assert(nSize<=33);
		if (!nSize)
			throw key_error("CKey::GetPubKey() : i2o_ECPublicKey failed");
		std::vector<unsigned char> vchPubKey(33, 0);
		unsigned char* pbegin = &vchPubKey[0];
		if (i2o_ECPublicKey(pkey, &pbegin) != nSize)
			throw key_error("CKey::GetPubKey() : i2o_ECPublicKey returned unexpected size");
		assert(vchPubKey.size()<=33);
		return vchPubKey;
	}

	bool Sign(const uint256& hash, std::vector<unsigned char>& vchSig)
	{
		unsigned char pchSig[10000];
		unsigned int nSize = 0;

		vchSig.clear();

		if (!ECDSA_sign(0, (unsigned char*)hash.begin(), hash.size(), pchSig, &nSize, pkey))
			return false;

		vchSig.resize(nSize);
		memcpy(&vchSig[0], pchSig, nSize);

		return true;
	}

	bool Verify(const uint256& hash, const void *sig, size_t sigLen) const
	{
		// -1 = error, 0 = bad sig, 1 = good
		if (ECDSA_verify(0, hash.begin(), hash.size(), (const unsigned char *) sig, sigLen, pkey) != 1)
			return false;
		return true;
	}

	bool Verify(const uint256& hash, const std::vector<unsigned char>& vchSig) const
	{
		return Verify(hash, &vchSig[0], vchSig.size());
	}

	bool Verify(const uint256& hash, const std::string& sig) const
	{
		return Verify(hash, sig.data(), sig.size());
	}

	// ECIES functions. These throw on failure

	// returns a 32-byte secret unique to these two keys. At least one private key must be known.
	void getECIESSecret(CKey& otherKey, uint256& enc_key, uint256& hmac_key);

	// encrypt/decrypt functions with integrity checking.
	// Note that the other side must somehow know what keys to use
	std::vector<unsigned char> encryptECIES(CKey& otherKey, const std::vector<unsigned char>& plaintext);
	std::vector<unsigned char> decryptECIES(CKey& otherKey, const std::vector<unsigned char>& ciphertext);
};

#endif
// vim:ts=4
