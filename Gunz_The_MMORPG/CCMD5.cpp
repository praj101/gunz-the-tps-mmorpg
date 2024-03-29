// CCMD5.cpp
#include "stdafx.h"
#include <string.h>
#include "CCMD5.h"

#ifndef GET_UINT32_LE
#define GET_UINT32_LE(n, b, i)						\
{													\
	(n) = ( (unsigned long) (b)[(i)]       )		\
	| ( (unsigned long) (b)[(i) + 1] <<  8 )		\
	| ( (unsigned long) (b)[(i) + 2] << 16 )		\
	| ( (unsigned long) (b)[(i) + 3] << 24 );		\
}
#endif

#ifndef PUT_UINT32_LE
#define PUT_UINT32_LE(n, b, i)						\
{													\
	(b)[(i)    ] = (unsigned char) ( (n)       );	\
	(b)[(i) + 1] = (unsigned char) ( (n) >>  8 );	\
	(b)[(i) + 2] = (unsigned char) ( (n) >> 16 );	\
	(b)[(i) + 3] = (unsigned char) ( (n) >> 24 );	\
}
#endif

static const unsigned char md5_padding[64] =
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static void md5_process(md5_context* ctx, unsigned char data[64])
{
	unsigned long X[16], A, B, C, D;

	GET_UINT32_LE( X[0],  data,  0 );
	GET_UINT32_LE( X[1],  data,  4 );
	GET_UINT32_LE( X[2],  data,  8 );
	GET_UINT32_LE( X[3],  data, 12 );
	GET_UINT32_LE( X[4],  data, 16 );
	GET_UINT32_LE( X[5],  data, 20 );
	GET_UINT32_LE( X[6],  data, 24 );
	GET_UINT32_LE( X[7],  data, 28 );
	GET_UINT32_LE( X[8],  data, 32 );
	GET_UINT32_LE( X[9],  data, 36 );
	GET_UINT32_LE( X[10], data, 40 );
	GET_UINT32_LE( X[11], data, 44 );
	GET_UINT32_LE( X[12], data, 48 );
	GET_UINT32_LE( X[13], data, 52 );
	GET_UINT32_LE( X[14], data, 56 );
	GET_UINT32_LE( X[15], data, 60 );

#define S(x, n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define _P_(a, b, c, d, k, s, t)			\
{										\
	a += F(b, c, d) + X[k] + t;			\
	a = S(a, s) + b;					\
}

    A = ctx->state[0];
	B = ctx->state[1];
	C = ctx->state[2];
	D = ctx->state[3];

#define F(x, y, z) (z ^ (x & (y ^ z)))
	_P_( A, B, C, D,  0,  7, 0xD76AA478 );
	_P_( D, A, B, C,  1, 12, 0xE8C7B756 );
	_P_( C, D, A, B,  2, 17, 0x242070DB );
	_P_( B, C, D, A,  3, 22, 0xC1BDCEEE );
	_P_( A, B, C, D,  4,  7, 0xF57C0FAF );
	_P_( D, A, B, C,  5, 12, 0x4787C62A );
	_P_( C, D, A, B,  6, 17, 0xA8304613 );
	_P_( B, C, D, A,  7, 22, 0xFD469501 );
	_P_( A, B, C, D,  8,  7, 0x698098D8 );
	_P_( D, A, B, C,  9, 12, 0x8B44F7AF );
	_P_( C, D, A, B, 10, 17, 0xFFFF5BB1 );
	_P_( B, C, D, A, 11, 22, 0x895CD7BE );
	_P_( A, B, C, D, 12,  7, 0x6B901122 );
	_P_( D, A, B, C, 13, 12, 0xFD987193 );
	_P_( C, D, A, B, 14, 17, 0xA679438E );
	_P_( B, C, D, A, 15, 22, 0x49B40821 );
#undef F

#define F(x, y, z) (y ^ (z & (x ^ y)))
	_P_( A, B, C, D,  1,  5, 0xF61E2562 );
	_P_( D, A, B, C,  6,  9, 0xC040B340 );
	_P_( C, D, A, B, 11, 14, 0x265E5A51 );
	_P_( B, C, D, A,  0, 20, 0xE9B6C7AA );
	_P_( A, B, C, D,  5,  5, 0xD62F105D );
	_P_( D, A, B, C, 10,  9, 0x02441453 );
	_P_( C, D, A, B, 15, 14, 0xD8A1E681 );
	_P_( B, C, D, A,  4, 20, 0xE7D3FBC8 );
	_P_( A, B, C, D,  9,  5, 0x21E1CDE6 );
	_P_( D, A, B, C, 14,  9, 0xC33707D6 );
	_P_( C, D, A, B,  3, 14, 0xF4D50D87 );
	_P_( B, C, D, A,  8, 20, 0x455A14ED );
	_P_( A, B, C, D, 13,  5, 0xA9E3E905 );
	_P_( D, A, B, C,  2,  9, 0xFCEFA3F8 );
	_P_( C, D, A, B,  7, 14, 0x676F02D9 );
	_P_( B, C, D, A, 12, 20, 0x8D2A4C8A );
#undef F

#define F(x, y, z) (x ^ y ^ z)
	_P_( A, B, C, D,  5,  4, 0xFFFA3942 );
	_P_( D, A, B, C,  8, 11, 0x8771F681 );
	_P_( C, D, A, B, 11, 16, 0x6D9D6122 );
	_P_( B, C, D, A, 14, 23, 0xFDE5380C );
	_P_( A, B, C, D,  1,  4, 0xA4BEEA44 );
	_P_( D, A, B, C,  4, 11, 0x4BDECFA9 );
	_P_( C, D, A, B,  7, 16, 0xF6BB4B60 );
	_P_( B, C, D, A, 10, 23, 0xBEBFBC70 );
	_P_( A, B, C, D, 13,  4, 0x289B7EC6 );
	_P_( D, A, B, C,  0, 11, 0xEAA127FA );
	_P_( C, D, A, B,  3, 16, 0xD4EF3085 );
	_P_( B, C, D, A,  6, 23, 0x04881D05 );
	_P_( A, B, C, D,  9,  4, 0xD9D4D039 );
	_P_( D, A, B, C, 12, 11, 0xE6DB99E5 );
	_P_( C, D, A, B, 15, 16, 0x1FA27CF8 );
	_P_( B, C, D, A,  2, 23, 0xC4AC5665 );
#undef F

#define F(x, y, z) (y ^ (x | ~z))
	_P_( A, B, C, D,  0,  6, 0xF4292244 );
	_P_( D, A, B, C,  7, 10, 0x432AFF97 );
	_P_( C, D, A, B, 14, 15, 0xAB9423A7 );
	_P_( B, C, D, A,  5, 21, 0xFC93A039 );
	_P_( A, B, C, D, 12,  6, 0x655B59C3 );
	_P_( D, A, B, C,  3, 10, 0x8F0CCC92 );
	_P_( C, D, A, B, 10, 15, 0xFFEFF47D );
	_P_( B, C, D, A,  1, 21, 0x85845DD1 );
	_P_( A, B, C, D,  8,  6, 0x6FA87E4F );
	_P_( D, A, B, C, 15, 10, 0xFE2CE6E0 );
	_P_( C, D, A, B,  6, 15, 0xA3014314 );
	_P_( B, C, D, A, 13, 21, 0x4E0811A1 );
	_P_( A, B, C, D,  4,  6, 0xF7537E82 );
	_P_( D, A, B, C, 11, 10, 0xBD3AF235 );
	_P_( C, D, A, B,  2, 15, 0x2AD7D2BB );
	_P_( B, C, D, A,  9, 21, 0xEB86D391 );
#undef F

	ctx->state[0] += A;
	ctx->state[1] += B;
	ctx->state[2] += C;
	ctx->state[3] += D;
}

CCMD5::CCMD5()
{

}

CCMD5::~CCMD5()
{

}

void CCMD5::md5_starts(md5_context *ctx)
{
	ctx->total[0] = 0;
	ctx->total[1] = 0;

	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xEFCDAB89;
	ctx->state[2] = 0x98BADCFE;
	ctx->state[3] = 0x10325476;
}

void CCMD5::md5_update(md5_context *ctx, unsigned char *input, int ilen)
{
	int fill;
	unsigned long left;

	if (ilen <= 0)
	{
		return;
	}

	left = ctx->total[0] & 0x3F;
	fill = 64 - left;

	ctx->total[0] += ilen;
	ctx->total[0] &= 0xFFFFFFFF;

	if (ctx->total[0] < (unsigned long)ilen)
	{
		ctx->total[1]++;
	}

	if (left && ilen >= fill)
	{
		memcpy((void *)(ctx->buffer + left), (void *)input, fill);
		md5_process(ctx, ctx->buffer);
		input += fill;
		ilen  -= fill;
		left = 0;
	}

	while (ilen >= 64)
	{
		md5_process(ctx, input);
		input += 64;
		ilen  -= 64;
	}

	if (ilen > 0)
	{
		memcpy((void *)(ctx->buffer + left), (void *)input, ilen);
	}
}

void CCMD5::md5_finish(md5_context *ctx, unsigned char output[16])
{
	unsigned long last, padn;
	unsigned long high, low;
	unsigned char msglen[8];

	high = (ctx->total[0] >> 29) | (ctx->total[1] <<  3);
	low  = (ctx->total[0] <<  3);

	PUT_UINT32_LE(low,  msglen, 0);
	PUT_UINT32_LE(high, msglen, 4);

	last = ctx->total[0] & 0x3F;
	padn = (last < 56) ? (56 - last) : (120 - last);

	md5_update(ctx, (unsigned char *)md5_padding, padn);
	md5_update(ctx, msglen, 8);

	PUT_UINT32_LE(ctx->state[0], output,  0);
	PUT_UINT32_LE(ctx->state[1], output,  4);
	PUT_UINT32_LE(ctx->state[2], output,  8);
	PUT_UINT32_LE(ctx->state[3], output, 12);
}

int CCMD5::md5_file(char *filePath, unsigned char output[16])
{
	FILE *f;
	size_t n;
	md5_context ctx;
	unsigned char buf[1024];

	if ((f = fopen(filePath, "rb")) == NULL)
	{
		return  1;
	}

	md5_starts(&ctx);

	while ((n = fread(buf, 1, 1024, f)) > 0)
	{
		md5_update(&ctx, buf, (int)n);
	}

	md5_finish(&ctx, output);
	fclose(f);
	return 0;
}

void CCMD5::md5_string(unsigned char *input, int ilen, unsigned char output[16])
{
	md5_context ctx;

	md5_starts(&ctx);
	md5_update(&ctx, input, ilen);
	md5_finish(&ctx, output);
}