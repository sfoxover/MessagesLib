#pragma once
/*
	Static helper functions
*/
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>

class EndianHelper
{
public:
	// Test for OS endianess
	static bool IsBigEndianOS();

	// Convert to big endian if needed
	static void MakeBigEndian(int64_t& value);

	// Convert to OS endian if needed
	static void ToggleEndianInt64(int64_t& value);

	// Get in64 number in big endian bytes
	static std::vector<unsigned char> GetBigEndianInt64Bytes(const int64_t value);
};