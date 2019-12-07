#include "EndianHelper.h"
#ifdef _WIN32
#include "windows.h"
#else
#include <unistd.h>
#endif
#include <filesystem>
#include <codecvt>
#include <iostream>
#include <cassert>
#include <cstring>

// Test for OS endianess
bool EndianHelper::IsBigEndianOS()
{
	union {
		uint32_t i;
		char c[4];
	} bint = { 0x01020304 };

	return bint.c[0] == 1;
}

// Convert to big endian if needed
void EndianHelper::MakeBigEndian(int64_t& value)
{
	if (!IsBigEndianOS())
	{
		ToggleEndianInt64(value);
	}
}

// Toggle endian for int64
void EndianHelper::ToggleEndianInt64(int64_t& value)
{
#ifdef _WIN32
	value = _byteswap_uint64(value);
#else
	value = __builtin_bswap64(value);
#endif
}

// Get in64 number in big endian bytes
std::vector<unsigned char> EndianHelper::GetBigEndianInt64Bytes(const int64_t value)
{
	int64_t copyValue = value;
	MakeBigEndian(copyValue);
	std::vector<unsigned char> bytes;
	bytes.resize(sizeof(int64_t));
	std::memcpy(&bytes[0], &copyValue, bytes.size());
	return bytes;
}