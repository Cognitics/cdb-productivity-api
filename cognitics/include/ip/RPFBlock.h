/****************************************************************************
Copyright 2014 Cognitics, Inc.
****************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace ip
{

	/**
	 * This is just a handy array for getting zone numbers from south to north.
	 **/
	const int NUM_ZONES = 18;
	/**
	 * These values come from Appendix A of the CIB standard, they define the north border for a zone
											   J   H   G   F   E   D   C   B  A 1  2  3  4  5  6  7  8  9 **/
	const double zone_latitudes[NUM_ZONES] = {-80,-76,-72,-68,-64,-56,-48,-32,0,32,48,56,64,68,72,76,80,90};
	const char south_north_zone_order[NUM_ZONES] = { 'J','H','G','F','E','D','C','B','A','1','2','3','4','5','6','7','8','9' };
	const int CIB_PIXELS_PER_BLOCK = 1536;

class RPFBlock
{
protected:
	// The character representation CIB Zone this block is in ('1','2','3','4','5','6','7','8','A','B','C','D','E','F','G','H' are valid values)
	char m_zone;

public:

	virtual ~RPFBlock()
	{

	}
	virtual std::string GetBlockType() 
	{
		return "RPF";
	}

	virtual std::string GetFriendlyResolutionString()
	{
		return "";
	}
	/**
	 * Returns true if this CIB block is totally inside the rectangle
	 * specified in the input parameters.
	 **/
	bool IsBlockInsideRect(double top, double right, double bottom, double left);

	virtual std::string GetFileName();
	virtual std::string GetFileExtention();

	/**
	 * Gets the character representation of the zone.
	 * '1','2','3','4','5','6','7','8','A','B','C','D','E','F','G','H' are valid values
	 **/
	char GetZone() const;
	/**
	 * Sets the character representation of the zone.
	 * '1','2','3','4','5','6','7','8','A','B','C','D','E','F','G','H' are valid values
	 **/
	bool SetZone(char zone);

	// The North extent of the RPF tile
	double m_RPFNorth;
	// The South extent of the RPF tile
	double m_RPFSouth;
	// The East extent of the RPF tile
	double m_RPFEast;
	// The West extent of the RPF tile
	double m_RPFWest;


	// The RPF Block number
	int m_blocknum;

	// The RPF row for this block
	int m_blockrow;
	// The RPF column for this block
	int m_blockcol;

	// The RPF version number, used in the filename generation. Defaults to '1'
	char m_version;

	// The width in pixels of the image buffer
	int m_width;
	// The Height in pixels of the image buffer
	int m_height;
	// The Depth in bytes of each pixels (1 for 8 bit greyscale, 3 for 24bit RGB)
	int m_depth;

	// The North extent of the block after having been clipped
	double m_ClippedNorth;
	// The East extent of the block after having been clipped
	double m_ClippedEast;
	// The South extent of the block after having been clipped
	double m_ClippedSouth;
	// The West extent of the block after having been clipped
	double m_ClippedWest;

	// The top pixel offset for the clipped image (top==0 in an unclipped image)
	int m_ClippedTopPixel;
	// The bottom pixel offset for the clipped image (bottom==height-1 in an unclipped image)
	int m_ClippedBottomPixel;
	// The left pixel offset for the clipped image (left==0 in an unclipped image)
	int m_ClippedLeftPixel;
	// The right pixel offset for the clipped image (right==width-1 in an unclipped image)
	int m_ClippedRightPixel;

	/**
	 * Convert the string from a radix34 representation into an
	 * unsigned integer.
	 **/
	static unsigned int StringToRPFRadix34UInt(std::string str);
	/**
	 * Convert the unsigned integrer into a RPF Radix-34 value
	 * following the conventions in MIL-STD-2411
	 **/
	static std::string uToRPFRadix34String(unsigned int value, int len=6);

	virtual std::string ToString()
	{
		std::stringstream ss;
		//ss << "Filename: " << GetFileName() << " Block: " << m_blocknum << " Zone: " << m_zone << " resolution: " << m_resolution;
		return ss.str();	
	}

	/**
	 * Returns a double precision value indicating how many degrees
	 * North/South per pixel for the given block
	 **/
	virtual double GetDegreesPerNorthSouthPixel() const
	{
		return 0;
	}
	
	/**
	 * Return a double indicating how many degrees
	 * East/West per pixel for the given block
	 **/
	virtual double GetDegreesPerEastWestPixel() const
	{
		return 0;
	}

	virtual double GetResolution()
	{
		return 0;
	}

	virtual double GetResolutionEastWest()
	{
		return GetResolution();
	}

	virtual double GetResolutionNorthSouth()
	{
		return GetResolution();
	}

	virtual RPFBlock * clone()
	{
		return NULL;
	}
};


}