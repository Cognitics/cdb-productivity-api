#pragma once
#include "b64\base64.h"
#include <vector>

std::string base64Encode(const std::string &in)
{
	std::string out;
	int i = 0;
	int j = -6;
	for (unsigned char c : in)
	{
		i = (i << 8) + c;
		j += 8;
		while (j >= 0)
		{
			out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(i >> j) & 0x3F]);
			j -= 6;
		}
	}
	if (j > -6)
	{
		out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((i << 8) >> (j + 8)) & 0x3F]);
	}
	while (out.size() % 4)
	{
		out.push_back('=');
	}
	return out;
}

std::string base64Decode(const std::string &in) 
{
	std::string out;

	std::vector<int> T(256, -1);
	for (int i = 0; i < 64; i++)
	{
		T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
	}

	int i = 0;
	int j = -8;
	for (unsigned char c : in) 
	{
		if (T[c] == -1)
		{
			break;
		}
		i = (i << 6) + T[c];
		j += 6;
		if (j >= 0) 
		{
			out.push_back(char((i >> j) & 0xFF));
			j -= 8;
		}
	}
	return out;
}