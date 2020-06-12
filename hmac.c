/**
 * Copyright (c) 2013-2014 Tomas Dzetkulic
 * Copyright (c) 2013-2014 Pavol Rusnak
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <string.h>
#include <malloc.h>
#include "hmac.h"
#include "options.h"
#include "memzero.h"

void hmac_sha256_Init(HMAC_SHA256_CTX *hctx, const uint8_t *key, const uint32_t keylen)
{
	static CONFIDENTIAL uint8_t i_key_pad[SHA256_BLOCK_LENGTH];
	memset(i_key_pad, 0, SHA256_BLOCK_LENGTH);
	if (keylen > SHA256_BLOCK_LENGTH) {
		sha256_Raw(key, keylen, i_key_pad);
	} else {
		memcpy(i_key_pad, key, keylen);
	}
	for (int i = 0; i < SHA256_BLOCK_LENGTH; i++) {
		hctx->o_key_pad[i] = i_key_pad[i] ^ 0x5c;
		i_key_pad[i] ^= 0x36;
	}
	sha256_Init(&(hctx->ctx));
	sha256_Update(&(hctx->ctx), i_key_pad, SHA256_BLOCK_LENGTH);
	memzero(i_key_pad, sizeof(i_key_pad));
}

void hmac_sha256_Update(HMAC_SHA256_CTX *hctx, const uint8_t *msg, const uint32_t msglen)
{
	sha256_Update(&(hctx->ctx), msg, msglen);
}

void hmac_sha256_Final(HMAC_SHA256_CTX *hctx, uint8_t *hmac)
{
	sha256_Final(&(hctx->ctx), hmac);
	sha256_Init(&(hctx->ctx));
	sha256_Update(&(hctx->ctx), hctx->o_key_pad, SHA256_BLOCK_LENGTH);
	sha256_Update(&(hctx->ctx), hmac, SHA256_DIGEST_LENGTH);
	sha256_Final(&(hctx->ctx), hmac);
	memzero(hctx, sizeof(HMAC_SHA256_CTX));
}

void hmac_sha256(const uint8_t *key, const uint32_t keylen, const uint8_t *msg, const uint32_t msglen, uint8_t *hmac)
{
	static CONFIDENTIAL HMAC_SHA256_CTX hctx;
	hmac_sha256_Init(&hctx, key, keylen);
	hmac_sha256_Update(&hctx, msg, msglen);
	hmac_sha256_Final(&hctx, hmac);
}

void hmac_sha256_prepare(const uint8_t *key, const uint32_t keylen, uint32_t *opad_digest, uint32_t *ipad_digest)
{
	static CONFIDENTIAL uint32_t key_pad[SHA256_BLOCK_LENGTH/sizeof(uint32_t)];

	memzero(key_pad, sizeof(key_pad));
	if (keylen > SHA256_BLOCK_LENGTH) {
		static CONFIDENTIAL SHA256_CTX context;
		sha256_Init(&context);
		sha256_Update(&context, key, keylen);
		sha256_Final(&context, (uint8_t*)key_pad);
	} else {
		memcpy(key_pad, key, keylen);
	}

	/* compute o_key_pad and its digest */
	for (int i = 0; i < SHA256_BLOCK_LENGTH/(int)sizeof(uint32_t); i++) {
		uint32_t data;
#if BYTE_ORDER == LITTLE_ENDIAN
		REVERSE32(key_pad[i], data);
#else
		data = key_pad[i];
#endif
		key_pad[i] = data ^ 0x5c5c5c5c;
	}
	sha256_Transform(sha256_initial_hash_value, key_pad, opad_digest);

	/* convert o_key_pad to i_key_pad and compute its digest */
	for (int i = 0; i < SHA256_BLOCK_LENGTH/(int)sizeof(uint32_t); i++) {
		key_pad[i] = key_pad[i] ^ 0x5c5c5c5c ^ 0x36363636;
	}
	sha256_Transform(sha256_initial_hash_value, key_pad, ipad_digest);
	memzero(key_pad, sizeof(key_pad));
}

void hmac_sha512_Init(HMAC_SHA512_CTX *hctx, const uint8_t *key, const uint32_t keylen)
{
	static CONFIDENTIAL uint8_t i_key_pad[SHA512_BLOCK_LENGTH];
	memset(i_key_pad, 0, SHA512_BLOCK_LENGTH);
	if (keylen > SHA512_BLOCK_LENGTH) {
		sha512_Raw(key, keylen, i_key_pad);
	} else {
		memcpy(i_key_pad, key, keylen);
	}
	for (int i = 0; i < SHA512_BLOCK_LENGTH; i++) {
		hctx->o_key_pad[i] = i_key_pad[i] ^ 0x5c;
		i_key_pad[i] ^= 0x36;
	}
	sha512_Init(&(hctx->ctx));
	sha512_Update(&(hctx->ctx), i_key_pad, SHA512_BLOCK_LENGTH);
	memzero(i_key_pad, sizeof(i_key_pad));
}

void hmac_sha512_Update(HMAC_SHA512_CTX *hctx, const uint8_t *msg, const uint32_t msglen)
{
	sha512_Update(&(hctx->ctx), msg, msglen);
}

void hmac_sha512_Final(HMAC_SHA512_CTX *hctx, uint8_t *hmac)
{
	sha512_Final(&(hctx->ctx), hmac);
	sha512_Init(&(hctx->ctx));
	sha512_Update(&(hctx->ctx), hctx->o_key_pad, SHA512_BLOCK_LENGTH);
	sha512_Update(&(hctx->ctx), hmac, SHA512_DIGEST_LENGTH);
	sha512_Final(&(hctx->ctx), hmac);
	memzero(hctx, sizeof(HMAC_SHA512_CTX));
}

void hmac_sha512(const uint8_t *key, const uint32_t keylen, const uint8_t *msg, const uint32_t msglen, uint8_t *hmac)
{
	HMAC_SHA512_CTX hctx;
	hmac_sha512_Init(&hctx, key, keylen);
	hmac_sha512_Update(&hctx, msg, msglen);
	hmac_sha512_Final(&hctx, hmac);
}

void hmac_sha512_prepare(const uint8_t *key, const uint32_t keylen, uint64_t *opad_digest, uint64_t *ipad_digest)
{
	static CONFIDENTIAL uint64_t key_pad[SHA512_BLOCK_LENGTH/sizeof(uint64_t)];

	memzero(key_pad, sizeof(key_pad));
	if (keylen > SHA512_BLOCK_LENGTH) {
		static CONFIDENTIAL SHA512_CTX context;
		sha512_Init(&context);
		sha512_Update(&context, key, keylen);
		sha512_Final(&context, (uint8_t*)key_pad);
	} else {
		memcpy(key_pad, key, keylen);
	}

	/* compute o_key_pad and its digest */
	for (int i = 0; i < SHA512_BLOCK_LENGTH/(int)sizeof(uint64_t); i++) {
		uint64_t data;
#if BYTE_ORDER == LITTLE_ENDIAN
		REVERSE64(key_pad[i], data);
#else
		data = key_pad[i];
#endif
		key_pad[i] = data ^ 0x5c5c5c5c5c5c5c5c;
	}
	sha512_Transform(sha512_initial_hash_value, key_pad, opad_digest);

	/* convert o_key_pad to i_key_pad and compute its digest */
	for (int i = 0; i < SHA512_BLOCK_LENGTH/(int)sizeof(uint64_t); i++) {
		key_pad[i] = key_pad[i] ^ 0x5c5c5c5c5c5c5c5c ^ 0x3636363636363636;
	}
	sha512_Transform(sha512_initial_hash_value, key_pad, ipad_digest);
	memzero(key_pad, sizeof(key_pad));
}



void pad_init(unsigned char *inner_pad, unsigned char *outer_pad,
	const unsigned char * const key, const int key_len)
{
	int i = 0;

	for (i = 0; i < MAX_DIGEST_BLOCK_LEN && i < key_len; i++) {
		inner_pad[i] = key[i] ^ 0x36;
		outer_pad[i] = key[i] ^ 0x5c;
	}

	if (i < MAX_DIGEST_BLOCK_LEN)
	{
		while (i < MAX_DIGEST_BLOCK_LEN)
		{
			inner_pad[i] = 0x36;
			outer_pad[i] = 0x5c;
			i++;
		}
	}
	return;
}

#if 0
int
hmac_sha3_256(const char *hmac_key, const int hmac_key_len, const char *msg, const unsigned int msg_len,
	unsigned char *hmac)
{
	unsigned char inner_hash[SHA3_256_DIGEST_LENGTH] = { 0 };
	unsigned char block_inner_pad[MAX_DIGEST_BLOCK_LEN] = { 0 };
	unsigned char block_outer_pad[MAX_DIGEST_BLOCK_LEN] = { 0 };
	unsigned char final_key[MAX_DIGEST_BLOCK_LEN] = { 0 };
	unsigned char padded_hash[SHA3_256_BLOCK_LENGTH + SHA3_256_DIGEST_LENGTH + 1] = { 0 };
	//unsigned char *padded_msg = calloc(1, msg_len + MAX_DIGEST_BLOCK_LEN + 1);
	unsigned char *padded_msg = malloc(msg_len + MAX_DIGEST_BLOCK_LEN + 1);

	int final_len = hmac_key_len;

	if (padded_msg == NULL)
		return -1;

	if (SHA3_256_BLOCK_LENGTH < hmac_key_len)
	{
		/* Calculate the digest of the key
		*/
		sha3_256((unsigned char *)hmac_key, final_len, final_key);
		final_len = SHA3_256_DIGEST_LENGTH;
	}
	else
	{
		memcpy(final_key, hmac_key, hmac_key_len);
	}
	pad_init(block_inner_pad, block_outer_pad, final_key, final_len);

	//The first step is to hash the inner_pad + message
	memcpy(padded_msg, block_inner_pad, SHA3_256_BLOCK_LENGTH);
	memcpy(padded_msg + SHA3_256_BLOCK_LENGTH, msg, msg_len);

	//Calculate the inner hash
	sha3_256(padded_msg, msg_len + SHA3_256_BLOCK_LENGTH, inner_hash);

	//Then hash the outer pad + inner hash
	memcpy(padded_hash, block_outer_pad, SHA3_256_BLOCK_LENGTH);
	memcpy(padded_hash + SHA3_256_BLOCK_LENGTH, inner_hash, SHA3_256_DIGEST_LENGTH);

	//the outer hash is the final hmac
	sha3_256(padded_hash, SHA3_256_BLOCK_LENGTH + SHA3_256_DIGEST_LENGTH, hmac);

	free(padded_msg);
	return 0;
}
#endif

int
hmac_sha3_256(const char *msg, const unsigned int msg_len,
	const char *hmac_key, const int hmac_key_len, unsigned char *hmac)
{
	unsigned char inner_hash[SHA3_256_DIGEST_LENGTH] = { 0 };
	unsigned char block_inner_pad[MAX_DIGEST_BLOCK_LEN] = { 0 };
	unsigned char block_outer_pad[MAX_DIGEST_BLOCK_LEN] = { 0 };
	unsigned char final_key[MAX_DIGEST_BLOCK_LEN] = { 0 };
	unsigned char padded_hash[SHA3_256_BLOCK_LENGTH + SHA3_256_DIGEST_LENGTH + 1] = { 0 };
	unsigned char *padded_msg = calloc(1, msg_len + MAX_DIGEST_BLOCK_LEN + 1);
	int final_len = hmac_key_len;

	if (padded_msg == NULL)
		return -1;

	if (SHA3_256_BLOCK_LENGTH < hmac_key_len)
	{
		/* Calculate the digest of the key
		*/
		sha3_256((unsigned char *)hmac_key, final_len, final_key);
		final_len = SHA3_256_DIGEST_LENGTH;
	}
	else
	{
		memcpy(final_key, hmac_key, hmac_key_len);
	}
	pad_init(block_inner_pad, block_outer_pad, final_key, final_len);

	//The first step is to hash the inner_pad + message
	memcpy(padded_msg, block_inner_pad, SHA3_256_BLOCK_LENGTH);
	memcpy(padded_msg + SHA3_256_BLOCK_LENGTH, msg, msg_len);

	//Calculate the inner hash
	sha3_256(padded_msg, msg_len + SHA3_256_BLOCK_LENGTH, inner_hash);

	//Then hash the outer pad + inner hash
	memcpy(padded_hash, block_outer_pad, SHA3_256_BLOCK_LENGTH);
	memcpy(padded_hash + SHA3_256_BLOCK_LENGTH, inner_hash, SHA3_256_DIGEST_LENGTH);

	//the outer hash is the final hmac
	sha3_256(padded_hash, SHA3_256_BLOCK_LENGTH + SHA3_256_DIGEST_LENGTH, hmac);

	free(padded_msg);
	return 0;
}


int
hmac_sha3_512(const char *msg, const unsigned int msg_len,
	 const char *hmac_key, const int hmac_key_len, unsigned char *hmac)
{
	unsigned char inner_hash[SHA3_512_DIGEST_LENGTH] = { 0 };
	unsigned char block_inner_pad[MAX_DIGEST_BLOCK_LEN] = { 0 };
	unsigned char block_outer_pad[MAX_DIGEST_BLOCK_LEN] = { 0 };
	unsigned char final_key[MAX_DIGEST_BLOCK_LEN] = { 0 };
	unsigned char padded_hash[SHA3_512_BLOCK_LENGTH + SHA3_512_DIGEST_LENGTH + 1] = { 0 };
	unsigned char *padded_msg = calloc(1, msg_len + MAX_DIGEST_BLOCK_LEN + 1);
	int final_len = hmac_key_len;

	if (padded_msg == NULL)
		return -1;

	if (SHA3_512_BLOCK_LENGTH < hmac_key_len)
	{
		/* Calculate the digest of the key
		*/
		sha3_512((unsigned char *)hmac_key, final_len, final_key);
		final_len = SHA3_512_DIGEST_LENGTH;
	}
	else
	{
		memcpy(final_key, hmac_key, hmac_key_len);
	}
	pad_init(block_inner_pad, block_outer_pad, final_key, final_len);

	//The first step is to hash the inner_pad + message
	memcpy(padded_msg, block_inner_pad, SHA3_512_BLOCK_LENGTH);
	memcpy(padded_msg + SHA3_512_BLOCK_LENGTH, msg, msg_len);

	//Calculate the inner hash
	sha3_512(padded_msg, msg_len + SHA3_512_BLOCK_LENGTH, inner_hash);

	//Then hash the outer pad + inner hash
	memcpy(padded_hash, block_outer_pad, SHA3_512_BLOCK_LENGTH);
	memcpy(padded_hash + SHA3_512_BLOCK_LENGTH, inner_hash, SHA3_512_DIGEST_LENGTH);

	//the outer hash is the final hmac
	sha3_512(padded_hash, SHA3_512_BLOCK_LENGTH + SHA3_512_DIGEST_LENGTH, hmac);

	free(padded_msg);
	return 0;
}
