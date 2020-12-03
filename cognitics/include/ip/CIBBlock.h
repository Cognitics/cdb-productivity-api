/****************************************************************************
Copyright 2014 Cognitics, Inc.
****************************************************************************/
#pragma once
#include <ip/RPFBlock.h>

#include <string>
#include <vector>
#include <sstream>
#include <ip/RPFBlock.h>

namespace ip
{

class CIBBlockContainer;
class CIBBlock : public RPFBlock
{

	static const double east_west_pixel_constants[NUM_ZONES];
	static const double north_south_pixel_constant;
	/**
	 * Returns an index into the east_west_pixel_constants array based on the char value specified.
	 **/
	static int GetZoneIndexFromChar(char zone);
	static char GetZoneCharFromIndex(int zone);
	/**
	 * Returns the latitude origin (sourthern border) for the specified zone and based on resolution.
     * This method will recursively determine the origin (the southernmost
     * latitude boundry) for a zone by going south to either zone 1 or zone 19
     * depending on the hemisphere. The latitude origin of a zone is defined
     * as the southernmost boundry of the northernmost row in a zone.
     * So the origin for zone 2 will be the southernmost boundry of the
     * row in zone 1 that overlaps with zone 2's nominal boundry (32).	 
	 **/
	static double GetZoneLatitudeOrigin(char zone, double resolution);

    /**
	 * GetZoneLatitudeNominal() returns the nominal latitude origin for this zone.
	 * We want the southernmost border for this zone. For zone 1, the southern most is
	 * 0, for zone 11 it is 32, etc.
 	 **/
    static double GetZoneLatitudeNominal(char zone);

	/**
     * Returns an exact northern border for a zone, taking overlap into consideration.
     **/
	static double GetZoneLatitudeExtent(char zone, double resolution);

	/**
	 * Returns the East-West Pixel Constant based on the specified zone and resolution.
	 **/
	static double GetEastWestPixelConstant(char zone,double resolution);

	static double GetPolarPixelConstant(double resolution);
	
	/**
	 * Returns the number of frames in each dimension in the polar area
	 * The total number of frames is (GetNumberPolarFrames(resolution)*2)^2
	 * See Figure 16 in MIL-C-89041
	 **/
	static int GetNumberPolarFrames(double resolution);
	/**
	 * Returns the North-South Pixel Constant based on the specified zone.
	 **/
	static double GetNorthSouthPixelConstant(char zone,double resolution);

public:
	CIBBlock(void);
	virtual ~CIBBlock(void);

	virtual std::string GetBlockType() 
	{
		return "CIB";
	}

	virtual std::string GetFriendlyResolutionString()
	{
		std::stringstream ss;
		ss << static_cast<int>(GetResolution());
		ss << "M";
		return ss.str();
	}

	virtual std::string GetFileExtention();
	//Modifying methods

	// Clip the block to the extents provided. This will modify the pixel
	// top/bottom/left/right values and the m_ClippedXXXXX values, but the
	// image buffer is untouched
	bool ClipBlock(double north, double east, double south, double west);


	// Comparison methods

	/**
	 * IsSameBlock compares only the zone, block ID and resolution, and returns true if they are the same.
	 * other members such as coveragex/y and lat/lon are ignored in this comparison.
	 **/
	bool IsSameBlock(const CIBBlock &in) const;

	/**
	 * Operator evaluates to true if the left-hand MDBBlock's zone, block ID, and resolution and less-than
	 * the right-hand MDBBlock's zone, block ID, and resolution.
	 */
	bool operator<(const CIBBlock &in) const;

    /**
	 * Operator performs equivalent operation as IsSameBlock().
	 * @see IsSameBlock()
	 */
	bool operator==(const CIBBlock &in) const;

	// The CIB Resolution
	double m_resolution;

	// The pixel buffer pointer
	unsigned char *m_buffer;



	// A latitude somewhere inside this block. For internal use only
	double m_lat;
	// A longitude somewhere inside this block. For internal use only
	double m_lon;

	
	virtual std::string ToString()
	{
		std::stringstream ss;
		ss << "Filename: " << GetFileName() << " Block: " << m_blocknum << " Zone: " << m_zone << " resolution: " << m_resolution;
		return ss.str();	
	}

	static CIBBlock GetBlock(double lat, double lon, double resolution, char forcedzone='X');
	static int GetZoneForPoint(double lat, double lon);
	static void ClipBlock(CIBBlock &block,double minx, double maxx, double miny, double maxy);
	static CIBBlockContainer GetBlocks(double north, double east, double south, double west, double resolution);

	static CIBBlock GetBlockBoundries(int blockno ,char zone,double resolution);
	static CIBBlock GetBlockBoundries(int row,int col,char zone,double resolution);

	/**
	 * Initialize the block number, zone, and resolution as well as extents from 
	 * a valid RPF filename (no path, just the filename)
	 **/
	static bool InitializeFromRPFFileName(std::string filename,CIBBlock &block);

	/**
	 * Returns a double precision value indicating how many degrees
	 * North/South per pixel for the given zone and resolution.
	 **/
	static double GetDegreesPerNorthSouthPixel(char zone, double resolution) {
		double ns_const = GetNorthSouthPixelConstant(zone,resolution);

		double pixels_per_degree = ns_const/90;
		return 1.000L/pixels_per_degree;

	}
	
	/**
	 * Return a double indicating how many degrees
	 * East/West per pixel for the given zone and resolution.
	 **/
	static double GetDegreesPerEastWestPixel(char zone, double resolution) {
		double ew_const = GetEastWestPixelConstant(zone,resolution);

		double pixels_per_degree = ew_const/360;
		return 1.000L/pixels_per_degree;
	}

	/**
	 * Returns a double precision value indicating how many degrees
	 * North/South per pixel for the given block
	 **/
	virtual double GetDegreesPerNorthSouthPixel() const
	{
		return CIBBlock::GetDegreesPerNorthSouthPixel(GetZone(),m_resolution);
	}
	
	/**
	 * Return a double indicating how many degrees
	 * East/West per pixel for the given block
	 **/
	virtual double GetDegreesPerEastWestPixel() const 
	{
		return CIBBlock::GetDegreesPerEastWestPixel(GetZone(),m_resolution);
	}

	virtual double GetResolution()
	{
		return m_resolution;
	}

	virtual CIBBlock * clone();

	int GetEWPixelConstant() const
	{
		return GetEastWestPixelConstant(m_zone, m_resolution);
	}

	int GetNSPixelConstant() const
	{
		return GetNorthSouthPixelConstant(m_zone, m_resolution);
	}

};


class CIBBlockContainer
{
	std::vector<CIBBlock> m_blocks;
	CIBBlock empty;
public:
	int GetBlockCount();
	CIBBlock &GetBlockNo(int idx);

	void AddBlock(CIBBlock block);

	void SortBlocks();
};



}