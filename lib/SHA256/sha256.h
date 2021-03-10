#pragma once

#include <stdint.h>
#include <stddef.h>

namespace sha
{
	typedef struct
	{
		uint8_t data[64];
		uint32_t datalen;
		unsigned long long bitlen;
		uint32_t state[8];
	} SHA256_CTX;

	extern void sha256_init(SHA256_CTX *ctx);
	extern void sha256_update(SHA256_CTX *ctx, const uint8_t data[], size_t len);
	extern void sha256_final(SHA256_CTX *ctx, uint8_t hash[]);
	extern String sha256_compute(String data);
} // namespace sha
