/****************************************************************************
Copyright 2014 Cognitics, Inc.
****************************************************************************/

#include <ip/RPFBlock.h>

#include "assert.h"
#include <float.h>

#include <algorithm>
namespace ip
{

bool RPFBlock::SetZone(char zone)
{
	for(int i=0;i<NUM_ZONES;i++)
	{
		if(zone==south_north_zone_order[i])
		{
			m_zone = zone;
			return true;
		}
	}
	return false;
}

std::string RPFBlock::GetFileExtention()
{
	return "";
}

bool RPFBlock::IsBlockInsideRect(double top, double right, double bottom, double left)
{
	if( (m_RPFNorth < top) &&
		(m_RPFSouth > bottom) &&
		(m_RPFWest > left) &&
		(m_RPFEast < right)
	  )
		return true;

	return false;
}

unsigned int RPFBlock::StringToRPFRadix34UInt(std::string str)
{
	unsigned int val = 0;
	const int radix = 34;
	if(str.size()==5)
		str = "0" + str;
	// The RPF Radix34 encoding leaves out the characters I and O, which is why we
	// don't just use itoa(value,36);
	const char r34chars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
		,'G','H','J','K','L','M','N','P','Q','R','S','T','U','V','W','X','Y','Z'};
	int digits = (int)str.size();
	for(int i=0;i<digits;i++)
	{
		bool found = false;
		char dig = str[i];
		// This isn't as fast as a big lookup table, but it will work
		for(int j=0;j<sizeof(r34chars);j++)
		{
			if(dig==r34chars[j])
			{
				val *= radix;
				val += j;
				found = true;
				break;// Break out of the search!
			}
		}
		if(!found)
		{
			// If we get here that means it wasn't a valid radix34 char
			return 0;
		}
	}
	return val;
}


/**
 * I'm so thrilled I get to write itoa()!
 * Converts an unsigned int to the radix34 string used by RPF.
 **/
std::string RPFBlock::uToRPFRadix34String(unsigned int value, int len)
{
	std::string revstring = "";//This will be the reverse of the string we'll return
	const int radix = 34;
	// The RPF Radix34 encoding leaves out the characters I and O, which is why we
	// don't just use itoa(value,36);
	const char r34chars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
		,'G','H','J','K','L','M','N','P','Q','R','S','T','U','V','W','X','Y','Z'};
	//assert((value % radix)<sizeof(r34chars));
	do
	{		
		revstring += r34chars[(value % radix)];
		value /= radix;
	}while(value!=0);
	
	int padding_needed = len - (int)revstring.size();
	for(int a=0;a<padding_needed;a++)
	{
		revstring += '0';
	}
	
	// Now reverse the string
	std::string retstr = "";
	
	int retlen = (int)revstring.size();
	for(int i = retlen-1;i>=0;i--)
	{
		retstr += revstring[i];
	}
	return retstr;
}

std::string RPFBlock::GetFileName()
{
	std::string ret;
	ret = uToRPFRadix34String(m_blocknum);
	ret += '1'; // This is the version number.
	ret += 'X'; // This is the producer ID. We don't have an official producer ID so we'll use the last one available
	ret += GetFileExtention();
	transform(ret.begin(), ret.end(), ret.begin(), tolower);
	return ret;
}


char RPFBlock::GetZone() const
{
	return m_zone;
}


}