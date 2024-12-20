
// #include <openssl/ec.h>
// #include <openssl/bn.h>
// #include <openssl/ecdsa.h>
// #include <openssl/pem.h>
// #include <openssl/err.h>

// // Functions to add CKey support for deterministic EC keys

// #include "Serializer.h"

// // <-- seed
// uint128 CKey::PassPhraseToKey(const std::string& passPhrase)
// {
// 	Serializer s;

// 	s.addRaw(passPhrase.c_str(), passPhrase.size());
// 	uint256	hash256	= s.getSHA512Half();
// 	uint128 ret(hash256);

// 	s.secureErase();

// 	return ret;
// }

// // --> seed
// // <-- private root generator + public root generator
// EC_KEY* CKey::GenerateRootDeterministicKey(const uint128& seed)
// {
// 	BN_CTX* ctx=BN_CTX_new();
// 	if(!ctx) return NULL;

// 	EC_KEY* pkey=EC_KEY_new_by_curve_name(NID_secp256k1);
// 	if(!pkey)
// 	{
// 		BN_CTX_free(ctx);
// 		return NULL;
// 	}
// 	EC_KEY_set_conv_form(pkey, POINT_CONVERSION_COMPRESSED);

// 	BIGNUM* order=BN_new();
// 	if(!order)
// 	{
// 		BN_CTX_free(ctx);
// 		EC_KEY_free(pkey);
// 		return NULL;
// 	}
// 	if(!EC_GROUP_get_order(EC_KEY_get0_group(pkey), order, ctx))
// 	{
// 		assert(false);
// 		BN_free(order);
// 		EC_KEY_free(pkey);
// 		BN_CTX_free(ctx);
// 		return NULL;
// 	}

// 	BIGNUM *privKey=NULL;
// 	int seq=0;
// 	do
// 	{ // private key must be non-zero and less than the curve's order
// 		Serializer s((128+32)/8);
// 		s.add128(seed);
// 		s.add32(seq++);
// 		uint256 root=s.getSHA512Half();
// 		s.secureErase();
// 		privKey=BN_bin2bn((const unsigned char *) &root, sizeof(root), privKey);
// 		if(privKey==NULL)
// 		{
// 			EC_KEY_free(pkey);
// 			BN_free(order);
// 			BN_CTX_free(ctx);
// 		}
// 		root.zero();
// 	} while(BN_is_zero(privKey) || (BN_cmp(privKey, order)>=0));

// 	BN_free(order);

// 	if(!EC_KEY_set_private_key(pkey, privKey))
// 	{ // set the random point as the private key
// 		assert(false);
// 		EC_KEY_free(pkey);
// 		BN_clear_free(privKey);
// 		BN_CTX_free(ctx);
// 		return NULL;
// 	}

// 	EC_POINT *pubKey=EC_POINT_new(EC_KEY_get0_group(pkey));
// 	if(!EC_POINT_mul(EC_KEY_get0_group(pkey), pubKey, privKey, NULL, NULL, ctx))
// 	{ // compute the corresponding public key point
// 		assert(false);
// 		BN_clear_free(privKey);
// 		EC_POINT_free(pubKey);
// 		EC_KEY_free(pkey);
// 		BN_CTX_free(ctx);
// 		return NULL;
// 	}
// 	BN_clear_free(privKey);
// 	if(!EC_KEY_set_public_key(pkey, pubKey))
// 	{
// 		assert(false);
// 		EC_POINT_free(pubKey);
// 		EC_KEY_free(pkey);
// 		BN_CTX_free(ctx);
// 		return NULL;
// 	}
// 	EC_POINT_free(pubKey);

// 	BN_CTX_free(ctx);

// 	assert(EC_KEY_check_key(pkey)==1);
// 	return pkey;
// }

// // Take newcoin address.
// // --> root public generator (consumes)
// // <-- root public generator in EC format
// EC_KEY* CKey::GenerateRootPubKey(BIGNUM* pubGenerator)
// {
// 	if (pubGenerator == NULL)
// 	{
// 		assert(false);
// 		return NULL;
// 	}

// 	EC_KEY* pkey = EC_KEY_new_by_curve_name(NID_secp256k1);
// 	if (!pkey)
// 	{
// 		BN_free(pubGenerator);
// 		return NULL;
// 	}
// 	EC_KEY_set_conv_form(pkey, POINT_CONVERSION_COMPRESSED);

// 	EC_POINT* pubPoint = EC_POINT_bn2point(EC_KEY_get0_group(pkey), pubGenerator, NULL, NULL);
// 	BN_free(pubGenerator);
// 	if(!pubPoint)
// 	{
// 		assert(false);
// 		EC_KEY_free(pkey);
// 		return NULL;
// 	}

// 	if(!EC_KEY_set_public_key(pkey, pubPoint))
// 	{
// 		assert(false);
// 		EC_POINT_free(pubPoint);
// 		EC_KEY_free(pkey);
// 		return NULL;
// 	}

// 	return pkey;
// }

// // --> public generator
// static BIGNUM* makeHash(const NewcoinAddress& pubGen, int seq, BIGNUM* order)
// {
// 	int subSeq=0;
// 	BIGNUM* ret=NULL;
// 	do
// 	{
// 		Serializer s((33*8+32+32)/8);
// 		s.addRaw(pubGen.getGenerator());
// 		s.add32(seq);
// 		s.add32(subSeq++);
// 		uint256 root=s.getSHA512Half();
// 		s.secureErase();
// 		ret=BN_bin2bn((const unsigned char *) &root, sizeof(root), ret);
// 		if(!ret) return NULL;
// 	} while (BN_is_zero(ret) || (BN_cmp(ret, order)>=0));

// 	return ret;
// }

// // --> public generator
// EC_KEY* CKey::GeneratePublicDeterministicKey(const NewcoinAddress& pubGen, int seq)
// { // publicKey(n) = rootPublicKey EC_POINT_+ Hash(pubHash|seq)*point
// 	EC_KEY*			rootKey		= CKey::GenerateRootPubKey(pubGen.getGeneratorBN());
// 	const EC_POINT*	rootPubKey	= EC_KEY_get0_public_key(rootKey);
// 	BN_CTX*			ctx			= BN_CTX_new();
// 	EC_KEY*			pkey		= EC_KEY_new_by_curve_name(NID_secp256k1);
// 	EC_POINT*		newPoint	= 0;
// 	BIGNUM*			order		= 0;
// 	BIGNUM*			hash		= 0;
// 	bool			success		= true;

// 	if (!ctx || !pkey)	success	= false;

// 	if (success)
// 		EC_KEY_set_conv_form(pkey, POINT_CONVERSION_COMPRESSED);

// 	if (success) {
// 		newPoint	= EC_POINT_new(EC_KEY_get0_group(pkey));
// 		if(!newPoint)	success	= false;
// 	}

// 	if (success) {
// 		order		= BN_new();

// 		if(!order || !EC_GROUP_get_order(EC_KEY_get0_group(pkey), order, ctx))
// 			success	= false;
// 	}

// 	// Calculate the private additional key.
// 	if (success) {
// 		hash		= makeHash(pubGen, seq, order);
// 		if(!hash)	success	= false;
// 	}

// 	if (success) {
// 		// Calculate the corresponding public key.
// 		EC_POINT_mul(EC_KEY_get0_group(pkey), newPoint, hash, NULL, NULL, ctx);

// 		// Add the master public key and set.
// 		EC_POINT_add(EC_KEY_get0_group(pkey), newPoint, newPoint, rootPubKey, ctx);
// 		EC_KEY_set_public_key(pkey, newPoint);
// 	}

// 	if (order)				BN_free(order);
// 	if (hash)				BN_free(hash);
// 	if (newPoint)			EC_POINT_free(newPoint);
// 	if (ctx)				BN_CTX_free(ctx);
// 	if (rootKey)			EC_KEY_free(rootKey);
// 	if (pkey && !success)	EC_KEY_free(pkey);

// 	return success ? pkey : NULL;
// }

// EC_KEY* CKey::GeneratePrivateDeterministicKey(const NewcoinAddress& pubGen, const uint256& u, int seq)
// {
// 	CBigNum bn(u);
// 	return GeneratePrivateDeterministicKey(pubGen, static_cast<BIGNUM*>(&bn), seq);
// }

// // --> root private key
// EC_KEY* CKey::GeneratePrivateDeterministicKey(const NewcoinAddress& pubGen, const BIGNUM* rootPrivKey, int seq)
// { // privateKey(n) = (rootPrivateKey + Hash(pubHash|seq)) % order
// 	BN_CTX* ctx=BN_CTX_new();
// 	if(ctx==NULL) return NULL;

// 	EC_KEY* pkey=EC_KEY_new_by_curve_name(NID_secp256k1);
// 	if(pkey==NULL)
// 	{
// 		BN_CTX_free(ctx);
// 		return NULL;
// 	}
// 	EC_KEY_set_conv_form(pkey, POINT_CONVERSION_COMPRESSED);

// 	BIGNUM* order=BN_new();
// 	if(order==NULL)
// 	{
// 		BN_CTX_free(ctx);
// 		EC_KEY_free(pkey);
// 		return NULL;
// 	}

// 	if(!EC_GROUP_get_order(EC_KEY_get0_group(pkey), order, ctx))
// 	{
// 		BN_free(order);
// 		BN_CTX_free(ctx);
// 		EC_KEY_free(pkey);
// 		return NULL;
// 	}

// 	// calculate the private additional key
// 	BIGNUM* privKey=makeHash(pubGen, seq, order);
// 	if(privKey==NULL)
// 	{
// 		BN_free(order);
// 		BN_CTX_free(ctx);
// 		EC_KEY_free(pkey);
// 		return NULL;
// 	}

// 	// calculate the final private key
// 	BN_mod_add(privKey, privKey, rootPrivKey, order, ctx);
// 	BN_free(order);
// 	EC_KEY_set_private_key(pkey, privKey);

// 	// compute the corresponding public key
// 	EC_POINT* pubKey=EC_POINT_new(EC_KEY_get0_group(pkey));
// 	if(!pubKey)
// 	{
// 		BN_clear_free(privKey);
// 		BN_CTX_free(ctx);
// 		EC_KEY_free(pkey);
// 		return NULL;
// 	}
// 	if(EC_POINT_mul(EC_KEY_get0_group(pkey), pubKey, privKey, NULL, NULL, ctx)==0)
// 	{
// 		BN_clear_free(privKey);
// 		BN_CTX_free(ctx);
// 		EC_KEY_free(pkey);
// 		return NULL;
// 	}
// 	BN_clear_free(privKey);
// 	EC_KEY_set_public_key(pkey, pubKey);

// 	EC_POINT_free(pubKey);
// 	BN_CTX_free(ctx);

// 	return pkey;
// }

// // vim:ts=4
