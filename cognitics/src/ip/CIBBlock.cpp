/****************************************************************************
Copyright 2014 Cognitics, Inc.
****************************************************************************/

#include <ip/CIBBlock.h>
#include <ip/RPFBlock.h>

#include <float.h>
#include <math.h>

#include <algorithm>

namespace ip
{


/**
 * These values come from Appendix A of the CIB standard
 * This array is mapped against actual zone numbers, so it has some empty values, and 
 * duplicates.
 **/
const double CIBBlock::east_west_pixel_constants[NUM_ZONES] = 
	{10008576,8243200,11008000,13721600,16332800,19916800,24576000,30259200,36966400,
	 36966400,30259200,24576000,19916800,16332800,13721600,11008000,8243200,10008576};


/**
 * These values come from Appendix A of the CIB standard
 **/
const double CIBBlock::north_south_pixel_constant = 10009600;

CIBBlock::CIBBlock(void)
{
	m_blocknum = 0;
	m_resolution = 0;
	m_RPFNorth = 0;
	m_RPFSouth = 0;
	m_RPFEast = 0;
	m_RPFWest = 0;
	m_ClippedNorth = 0;
	m_ClippedEast = 0;
	m_ClippedSouth = 0;
	m_ClippedWest = 0;
	m_width = 0;
	m_height = 0;
	m_depth = 0;
	m_buffer = NULL;
	m_ClippedTopPixel = 0;
	m_ClippedBottomPixel = 0;
	m_ClippedLeftPixel = 0;
	m_ClippedRightPixel = 0;
	m_version = '3';
	SetZone('1');

}

CIBBlock::~CIBBlock(void)
{
}


/**
 *
 * Values set for CIB from MIL-STD-2411-1 page 9
 **/

std::string CIBBlock::GetFileExtention()
{
	std::string ret;
	if(m_resolution==1.0f)
		ret = ".I4";
	else if(m_resolution==5.0f)
		ret = ".I2";
	else if(m_resolution==10.0f)
		ret = ".I1";
	else if(m_resolution==0.5f)
		ret = ".I5";
	else if(m_resolution==0.25f)
		ret = ".I6";
	else
		ret = ".xx";

	ret += GetZone();
	return ret;
}

bool CIBBlock::IsSameBlock(const CIBBlock &in) const
{
	if((in.GetZone()==this->GetZone())&&
		(in.m_blocknum==this->m_blocknum)&&
		(in.m_resolution==this->m_resolution)) {
			return true;
	}
	return false;
}

bool CIBBlock::operator==(const CIBBlock &in) const 
{
	return IsSameBlock(in);
}

bool CIBBlock::operator<(const CIBBlock &in) const
{
	if((m_resolution<in.m_resolution) ||
        (GetZone() < in.GetZone()) ||
        (m_blocknum < in.m_blocknum))
        return true;
	return false;
}

bool CIBBlock::ClipBlock(double north, double east, double south, double west)
{

	return false;
}


/**
 * Returns the East-West Pixel Constant based on the specified zone.
 **/
double CIBBlock::GetEastWestPixelConstant(char zonechar,double resolution)
{
	if(zonechar=='J' || zonechar=='9')
	{
		return GetPolarPixelConstant(resolution);
	}
	int zone = GetZoneIndexFromChar(zonechar);
	return east_west_pixel_constants[zone] / (double)resolution;
}

/**
 * Returns the North-South Pixel Constant based on the specified zone.
 **/
double CIBBlock::GetNorthSouthPixelConstant(char zonechar,double resolution)
{
	if(zonechar=='J' || zonechar=='9')
	{
		return GetPolarPixelConstant(resolution);
	}
	return north_south_pixel_constant / resolution;
}



int CIBBlockContainer::GetBlockCount() 
{ 
	return (int)m_blocks.size(); 
}

CIBBlock &CIBBlockContainer::GetBlockNo(int idx)
{
	if(idx<GetBlockCount())
	{
		return m_blocks.at(idx);
	}
	else
	{
		return empty;
	}

}

void CIBBlockContainer::AddBlock(CIBBlock block)
{
	m_blocks.push_back(block);
}

// returns true if block1 is greater (more to the north or to the right) than block2
bool CIBBlockCompare(CIBBlock block1, CIBBlock block2)
{
	if(block1.GetZone() > block2.GetZone())
		return false;
	if(block1.GetZone() < block2.GetZone())
		return true;
	//if the zones are the same, compare rows...
	if(block1.m_blocknum > block2.m_blocknum)
		return false;
	if(block1.m_blocknum < block2.m_blocknum)
		return true;

	return false;
}

void CIBBlockContainer::SortBlocks()
{
	sort(m_blocks.begin(),m_blocks.end(),CIBBlockCompare);//Sort the blocks!
}



bool CIBBlock::InitializeFromRPFFileName(std::string filename,CIBBlock &block)
{
	// Make sure it's upper case so the conversion to int works.
	transform(filename.begin(), filename.end(), filename.begin(), toupper);
	// A RPF filename should always be 12 bytes long
	// (6 radix34 + 1 version + 1 producer ID + 1 '.' + 2 resolution + 1 zone)
	if(filename.size()!=12)
		return false;

	std::string radix34str = filename.substr(0,6);
	unsigned int blockno = CIBBlock::StringToRPFRadix34UInt(radix34str);
	char zonech = filename[11];
	std::string series = filename.substr(9,2);
	double res = 0;
	if(series=="I4")
	{
		res = 1.0f;
	}
	else if(series=="I2")
	{
		res = 5.0f;
	}
	else if(series=="I1")
	{
		res = 10.0f;
	}
	else
	{
		return false;
	}
	block = GetBlockBoundries(blockno,zonech,res);
	return true;
}
CIBBlockContainer CIBBlock::GetBlocks(double north, double east, double south, double west, double resolution)
{
	CIBBlockContainer blocks;
	// For all boundry boxes we need the pixel positions. 
	// Otherwise we just provide the entire block.

	CIBBlock originBlock = CIBBlock::GetBlock(south,west,resolution);
	CIBBlock extentBlock = CIBBlock::GetBlock(north,east,resolution);

	for(int i=0;i<NUM_ZONES;i++) {
		CIBBlock southern_block = originBlock;
		CIBBlock northern_block = extentBlock;
		CIBBlock western_block = originBlock;
		CIBBlock eastern_block = extentBlock;
		
		char zonechar = GetZoneCharFromIndex(i);
		if(zonechar=='X')
			continue;

		double northern_zone_border = GetZoneLatitudeExtent(zonechar,resolution);
		double southern_zone_border = GetZoneLatitudeOrigin(zonechar,resolution);
		double southern_border = south;
		double southern_clip_border = south;
		double northern_border = north;

        CIBBlock zoneOriginBlock = CIBBlock::GetBlock(south,west,resolution,zonechar);
	    CIBBlock zoneExtentBlock = CIBBlock::GetBlock(north,east,resolution,zonechar);

		int start_row = zoneOriginBlock.m_blockrow;
		int end_row = zoneExtentBlock.m_blockrow;

		if((southern_zone_border > northern_border) || 
			(northern_zone_border < southern_border)) {
			// we need no blocks from this zone
			continue;
		}

		// check to see if this block spans multiple zones.
		if(northern_block.m_RPFNorth > northern_zone_border) {
			// compute a new northern_block, which is the northern most 
			// block for this zone
			// get the southern border of the zone above us, to use for the new northern 
			// border block.
			// We want to get the -nominal- southern border for the zone -above us- to determine
			// the last block in this zone
			if(zonechar!='9')// If we're at the top zone don't do this step
			{
				char zonechar2 = GetZoneCharFromIndex(i+1);
				double nominal_border = GetZoneLatitudeNominal(zonechar2);
				
				northern_block = GetBlock(nominal_border,west,resolution,zonechar);
				northern_border = northern_block.m_RPFNorth;
				end_row = northern_block.m_blockrow;
			}
		}
		if(southern_block.m_RPFSouth <= southern_zone_border) {
			// compute a new southern border
			southern_border = southern_zone_border;
			start_row = 0;
		}
		// compute a new western and eastern block since the colno will be different for each zone
		// We add some to put us inside the new zone, rather than at the border, which would put us in the next block south
		western_block = CIBBlock::GetBlock(southern_border + DBL_EPSILON,west,resolution,zonechar);
		eastern_block = CIBBlock::GetBlock(southern_border + DBL_EPSILON,east,resolution,zonechar);//We use the southern border to make sure we stay in the same zone.

		// add all blocks in this zone...
		// row loop  (from southern_row to northern row)
		int total_rows = (end_row - start_row)+1;
		// col loop (from origin col to extent col)
		int total_cols = (eastern_block.m_blockcol - western_block.m_blockcol) + 1;
				
		for(int row=start_row;row<=end_row;row++) {
			for(int col=western_block.m_blockcol;col<=eastern_block.m_blockcol;col++) {
				CIBBlock block;
				block = GetBlockBoundries(row,col,zonechar,resolution);
				double minx = 0;
				double maxx = 0;
				double miny = 0;
				double maxy = 0;
				if(row==start_row) {
					miny = southern_clip_border;
				}
				if(row==end_row) {
					maxy = northern_border;
				}
				if(col==western_block.m_blockcol) {
					minx = western_block.m_lon;				
				}
				if(col==eastern_block.m_blockcol) {
					maxx = eastern_block.m_lon;
				}
				CIBBlock::ClipBlock(block,minx,maxx,miny,maxy);
				blocks.AddBlock(block);
			}
		}
	}
	blocks.SortBlocks();
	return blocks;
}

int CIBBlock::GetZoneIndexFromChar(char zone)
{
	if(zone=='X')
	{
		return 0;//Invalid zone
	}
	
	for(int i=0;i<NUM_ZONES;i++)
	{
		if(zone==south_north_zone_order[i])
		{
			return i;
		}
	}
	return 0;
}

char CIBBlock::GetZoneCharFromIndex(int zone)
{
	if((zone<0)||(zone >= NUM_ZONES))
	{
		return 'X';//Invalid zone
	}
	return south_north_zone_order[zone];
}


double CIBBlock::GetPolarPixelConstant(double resolution)
{
	// Special polar pixel constants calculated according
	// to MIL-C-89041 A.5.2.1
	double ps = (north_south_pixel_constant/resolution)*20.0f/90.0f;
	int ips = (int)ps;
	int remainder = ips % 512;
	// round off 
	ips -= remainder;
	if(remainder>256)
		ips += 512;
	ps = (double)ips*90.0f/20.0f;
	return ps;
}


#define round(a) (floor(a+0.5))
/**
 * ClipBlock will clip the extents in case we don't want the entire block for an operation.
 * It will also compute pixel extents
 **/
void CIBBlock::ClipBlock(CIBBlock &block,double minx, double maxx, double miny, double maxy)
{
	block.m_ClippedWest = block.m_RPFWest;
	block.m_ClippedEast = block.m_RPFEast;
	block.m_ClippedNorth = block.m_RPFNorth;
	block.m_ClippedSouth = block.m_RPFSouth;
	
	char zone = block.GetZone();
	double ew_const = GetEastWestPixelConstant(zone,block.m_resolution);
	double ns_const = GetNorthSouthPixelConstant(zone,block.m_resolution);
	// Get the latitude/longitude origins
	double orig = GetZoneLatitudeOrigin(block.GetZone(),block.m_resolution);

	double border_bottom = ((90/ns_const) * CIB_PIXELS_PER_BLOCK * (block.m_blockrow )) + orig;
	double border_top = ((90/ns_const) * CIB_PIXELS_PER_BLOCK * (block.m_blockrow + 1)) + orig;
	double border_left = ((360/ew_const) * CIB_PIXELS_PER_BLOCK * block.m_blockcol) + -180;
	double border_right = ((360/ew_const) * CIB_PIXELS_PER_BLOCK * (block.m_blockcol+1)) + -180;
	// The pixels originate from the top left, whereas everything else has an origin of the
	// bottom left, so we have some confusing stuff below.
	if(miny) {
		block.m_ClippedBottomPixel = (int)floor(((border_top - miny)/90) * ns_const);
		
		if(block.m_ClippedBottomPixel>(CIB_PIXELS_PER_BLOCK-1))
			block.m_ClippedBottomPixel = (CIB_PIXELS_PER_BLOCK-1);
		
		// Now convert the clipped geo extent to the actual pixel edge.
		block.m_ClippedSouth = border_top - ((block.m_ClippedBottomPixel+1) * block.GetDegreesPerNorthSouthPixel());
	}
	else 
	{
		block.m_ClippedBottomPixel = CIB_PIXELS_PER_BLOCK-1;
		block.m_ClippedSouth = border_bottom;
	}
	if(maxy)
	{
		block.m_ClippedTopPixel = (int)floor(((border_top - maxy)/90) * ns_const);
		block.m_ClippedNorth = border_top - ((block.m_ClippedTopPixel) * block.GetDegreesPerNorthSouthPixel());
	}
	else
	{
		block.m_ClippedTopPixel = 0;
		block.m_ClippedNorth = border_top;
	}

	
	if(minx) 
	{
		block.m_ClippedLeftPixel = (int)floor(((minx - border_left)/360) * ew_const);
		block.m_ClippedWest = ((block.m_ClippedLeftPixel) * block.GetDegreesPerEastWestPixel()) + border_left;
	}
	else
	{
		block.m_ClippedLeftPixel = 0;
		block.m_ClippedWest = border_left;

	}
	if(maxx)
	{
		block.m_ClippedRightPixel = (int)floor(((maxx - border_left)/360) * ew_const);
		if(block.m_ClippedRightPixel>(CIB_PIXELS_PER_BLOCK-1))
			block.m_ClippedRightPixel = (CIB_PIXELS_PER_BLOCK-1);
		block.m_ClippedEast = ((block.m_ClippedRightPixel+1) * block.GetDegreesPerEastWestPixel()) + border_left;
	}
	else
	{
		block.m_ClippedRightPixel = CIB_PIXELS_PER_BLOCK-1;
		block.m_ClippedEast = border_right;
	}

}

CIBBlock CIBBlock::GetBlockBoundries(int blockno ,char zone,double resolution)
{
	int row;
	int col;
	double ew_const = GetEastWestPixelConstant(zone,resolution);
	double ns_const = GetNorthSouthPixelConstant(zone,resolution);
	int num_cols = (int)ceil(ew_const / CIB_PIXELS_PER_BLOCK);

	row = blockno / num_cols;
	col = blockno - (row * num_cols);

	CIBBlock ret = GetBlockBoundries(row, col, zone, resolution);
	std::string fname = ret.GetFileName();
	return ret; 

}

CIBBlock CIBBlock::GetBlockBoundries(int row,int col,char zone,double resolution)
{
	CIBBlock block;
	block.m_blockcol = col;
	block.m_blockrow = row;
	block.m_resolution = resolution;
	block.SetZone(zone);
    block.m_ClippedLeftPixel = 0;
    block.m_ClippedRightPixel = CIB_PIXELS_PER_BLOCK-1;
    block.m_ClippedBottomPixel = CIB_PIXELS_PER_BLOCK-1;
    block.m_ClippedTopPixel = 0;
	double ew_const = GetEastWestPixelConstant(zone,resolution);
	double ns_const = GetNorthSouthPixelConstant(zone,resolution);
	int num_cols = (int)ceil(ew_const/CIB_PIXELS_PER_BLOCK);
	block.m_blocknum = (num_cols*row) + col;

	// Get the latitude/longitude origins
	// Origins are upper north, which is pixel 0
    double orig = GetZoneLatitudeOrigin(zone,block.m_resolution);
	block.m_RPFSouth = ((90/ns_const) * CIB_PIXELS_PER_BLOCK * (row)) + orig;
	block.m_RPFNorth = ((90/ns_const) * CIB_PIXELS_PER_BLOCK * (row + 1)) + orig;
	
	block.m_RPFWest = ((360/ew_const) * CIB_PIXELS_PER_BLOCK * col) + -180;
	block.m_RPFEast = ((360/ew_const) * CIB_PIXELS_PER_BLOCK * (col+1)) + -180;
	block.m_width = 1536;
	block.m_height = 1536;
	return block;
}


/**
 * Returns the southern border for a zone
 **/
double CIBBlock::GetZoneLatitudeNominal(char zonechar)
{
	int zone = GetZoneIndexFromChar(zonechar);
	if(zone==0)
		return -90.0f;
		
	double val = zone_latitudes[zone-1];
	return val;
}

/**
 * Returns an exact northern border for a zone, taking overlap into consideration.
 **/
double CIBBlock::GetZoneLatitudeExtent(char zonechar, double resolution)
{
	int zone = GetZoneIndexFromChar(zonechar);
	double val = 0;
	if(zonechar=='9')
		return 90.0f;
		

		
	// Check the northern hemisphere	
	double ns_const = GetNorthSouthPixelConstant(zonechar,resolution);
	// compute the number of rows for this zone
	double pixels_per_degree = ns_const/90;
	
	// Handle the south pole
	if(zonechar=='J')
	{
		int numFrames = GetNumberPolarFrames(resolution);
		double exact_extent = -90 + ((double)numFrames * (double)CIB_PIXELS_PER_BLOCK)/pixels_per_degree;
		return exact_extent;	
	}
	// get the nominal border by looking at the origin of the zone
	// above this one.
	double nominal_boundry = 0;
	nominal_boundry = GetZoneLatitudeNominal(GetZoneCharFromIndex(zone+1));
	
	int num_rows=0;
	// round up to the next integer value
	if(zone > 8) {
		num_rows = (int)ceil((nominal_boundry * pixels_per_degree)/CIB_PIXELS_PER_BLOCK);
	}
	else {
		num_rows = (int)floor((abs(nominal_boundry) * pixels_per_degree)/CIB_PIXELS_PER_BLOCK);
	}
	// now we will find the actual extent by reversing the calculation we did above
	// because we did an integer rounding, we will get the overlapping portion.
	double exact_extent = ((double)num_rows * (double)CIB_PIXELS_PER_BLOCK)/pixels_per_degree;
	
	if(zone < 9)
		return exact_extent * -1;
	else
		return exact_extent;
}

/**
 * Returns the southern border for a zone
 **/
double CIBBlock::GetZoneLatitudeOrigin(char zonechar, double resolution)
{
	int zone = GetZoneIndexFromChar(zonechar);
    // This method will recursively determine the origin (the southernmost
    // latitude boundry) for a zone by going south to either zone 1 or zone 19
    // depending on the hemisphere. The latitude origin of a zone is defined
    // as the southernmost boundry of the northernmost row in a zone.
    // So the origin for zone 2 will be the southernmost boundry of the
    // row in zone 1 that overlaps with zone 2's nominal boundry (32).

    // If zone 'J' or index 0, return -90, the southern most
    if(zonechar=='J') 
    {
        return -90.0f;
    }  
    //Zone '1' is on the equator
    if(zonechar=='1')
    {
		return 0;
    }
    // Zone 9, the north pole has its own rules for overlap
    if(zonechar=='9')
	{
		double pixels_per_degree = GetPolarPixelConstant(resolution)/90;
		int numFrames = GetNumberPolarFrames(resolution);
		double exact_extent = 90.0f - ((double)(numFrames) * (double)CIB_PIXELS_PER_BLOCK)/pixels_per_degree;
		return exact_extent;	
	}

    // otherwise, get the southernmost boundry for the northernmost row in the
    // zone south of this one
    int southern_zone = zone - 1;

	double nominal_zone_origin = GetZoneLatitudeNominal(zonechar);
    double southern_zone_origin = GetZoneLatitudeOrigin(GetZoneCharFromIndex(southern_zone),resolution);
    double southern_zone_span = nominal_zone_origin - southern_zone_origin;

    double ns_const = GetNorthSouthPixelConstant(GetZoneCharFromIndex(southern_zone),resolution);
	// compute the number of rows for this zone
	double pixels_per_degree = ns_const/90;



    int num_rows=0;
    // find out how many rows in the zone to the south
	// round up to the next integer value

	if(zone>8) {
		num_rows = (int)ceil((southern_zone_span * pixels_per_degree)/CIB_PIXELS_PER_BLOCK);
	}
	else {
		num_rows = (int)floor((abs(southern_zone_span) * pixels_per_degree)/CIB_PIXELS_PER_BLOCK);
	}
	
	double exact_extent = southern_zone_origin + ((double)(num_rows-1) * (double)CIB_PIXELS_PER_BLOCK)/pixels_per_degree;
    return exact_extent;
	
}

int CIBBlock::GetNumberPolarFrames(double resolution)
{
	//Compute according to MIL-C-89041
	// Section A.5.2.2
	double pixelConstant = GetPolarPixelConstant(resolution);
	double numFrames = (((pixelConstant * (20.0f/90.0f))/256)+4)/6;
	int inumFrames = (int)ceil(numFrames);
	if((inumFrames%2)==0)
	{
		inumFrames++;// Round up to the next odd number
	}
	return inumFrames;
}

/**
 * This static method returns a CIBBlock structure filled in, based 
 * on the input values of latitude, longitude, and resolution.
 **/
CIBBlock CIBBlock::GetBlock(double lat, double lon, double resolution, char forcedzone)
{
    CIBBlock theblock;
    if((lat < -90)||(lat > 90)) {
        return theblock;
    }
    if((lon <-180)||(lon > 180)) {
        return theblock;
    }

	//TODO:	Test with polar regions!
	
	theblock.m_lat = lat;
	theblock.m_lon = lon;
    theblock.SetZone((forcedzone!='X')?forcedzone:GetZoneForPoint(lat,lon));	

	theblock.m_resolution = resolution;

	// First we need to figure out how many blocks it takes to circle the earth at this latitude.
	double ew_const = GetEastWestPixelConstant(theblock.GetZone(),resolution);
	double ns_const = GetNorthSouthPixelConstant(theblock.GetZone(),resolution);
	int num_cols = (int)ceil(ew_const / CIB_PIXELS_PER_BLOCK);
	
	int row = (int)((lat-GetZoneLatitudeOrigin(theblock.GetZone(),resolution))/(90) * (ns_const/CIB_PIXELS_PER_BLOCK));	
	int col = (int)((lon-(-180))/(360)*(ew_const/CIB_PIXELS_PER_BLOCK));

	theblock.m_blocknum = (num_cols*row) + col;
	theblock.m_blockcol = col;
	theblock.m_blockrow = row;

	// Get the latitude/longitude origins
	double orig = GetZoneLatitudeOrigin(theblock.GetZone(),resolution);
	theblock.m_RPFNorth = ((90/ns_const) * CIB_PIXELS_PER_BLOCK * (row + 1)) + orig;
	theblock.m_RPFSouth = ((90/ns_const) * CIB_PIXELS_PER_BLOCK * (row )) + orig;
	
	theblock.m_RPFWest = ((360/ew_const) * CIB_PIXELS_PER_BLOCK * col) + -180;
	theblock.m_RPFEast = ((360/ew_const) * CIB_PIXELS_PER_BLOCK * (col+1)) + -180;

	return theblock;
}

/**
 * This static method returns the zone based on lon/lat input.
 **/
int CIBBlock::GetZoneForPoint(double lat, double lon)
{
    if(lat<=-90) {
        return 'J';
    }
    else if(lat>90) {
        return '9';
    }

	// start at the south and return the first zone with a north border above this point.
	for(int i=0;i<NUM_ZONES;i++) 
	{			
		if(lat < zone_latitudes[i])
		{
			char zc = GetZoneCharFromIndex(i);
			return zc;
		}
	}

	return 'X';
}

CIBBlock * CIBBlock::clone()
{
	CIBBlock *theClone = new CIBBlock();
	*theClone = *this;
	return theClone;
}


}