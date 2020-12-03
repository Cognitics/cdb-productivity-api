/****************************************************************************
Copyright 2016 Cognitics, Inc.
Contract No:  
Contractor Name: Cognitics, Inc.
Contractor Address: 

Expiration of SBIR Data Rights Period: 

SBIR DATA RIGHTS - The Government's rights to use, modify, reproduce,
release, perform, display, or disclose technical data or computer software
marked with this legend are restricted during the period shown as provided
in paragraph (b)(4) of the Rights in Noncommercial Technical Data and
Computer Software -- Small Business Innovative Research (SBIR) Program
clause contained in the above identified contract. No restrictions apply
after the expiration date shown above. Any reproduction of technical data,
computer software, or portions thereof marked with this legend must also
reproduce the markings.
****************************************************************************/
#pragma once

#include <string>
#include <ip/ip.h>
#include <ccl/binary.h>
namespace ip {

	std::vector<std::string> getFileListFromTOC(const std::string &tocfile);

	/* -------------------------------------------------------------------- */
	/*  Borrowed from GDAL  */
	/* -------------------------------------------------------------------- */

	typedef enum {
		LID_HeaderComponent = 128,
		LID_LocationComponent = 129,
		LID_CoverageSectionSubheader = 130,
		LID_CompressionSectionSubsection = 131,
		LID_CompressionLookupSubsection = 132,
		LID_CompressionParameterSubsection = 133,
		LID_ColorGrayscaleSectionSubheader = 134,
		LID_ColormapSubsection = 135,
		LID_ImageDescriptionSubheader = 136,
		LID_ImageDisplayParametersSubheader = 137,
		LID_MaskSubsection = 138,
		LID_ColorConverterSubsection = 139,
		LID_SpatialDataSubsection = 140,
		LID_AttributeSectionSubheader = 141,
		LID_AttributeSubsection = 142,
		LID_ExplicitArealCoverageTable = 143,
		LID_RelatedImagesSectionSubheader = 144,
		LID_RelatedImagesSubsection = 145,
		LID_ReplaceUpdateSectionSubheader = 146,
		LID_ReplaceUpdateTable = 147,
		LID_BoundaryRectangleSectionSubheader = 148,
		LID_BoundaryRectangleTable = 149,
		LID_FrameFileIndexSectionSubHeader = 150,
		LID_FrameFileIndexSubsection = 151,
		LID_ColorTableIndexSectionSubheader = 152,
		LID_ColorTableIndexRecord = 153
	} NITFLocId;

	inline u_int ByteSwap(u_int in)
	{
		u_int out;
		char *indata = (char *)&in;
		char *outdata = (char *)&out;
		outdata[0] = indata[3];
		outdata[3] = indata[0];

		outdata[1] = indata[2];
		outdata[2] = indata[1];
		return out;
	}

	inline u_short ByteSwap(u_short in)
	{
		u_short out;
		char *indata = (char *)&in;
		char *outdata = (char *)&out;
		outdata[0] = indata[1];
		outdata[1] = indata[0];
		return out;
	}

	inline double ByteSwap(double in)
	{
		double out;
		char *data_in, *data_out;

		data_in = (char*)&in;
		data_out = (char*)&out;

		data_out[0] = data_in[7];
		data_out[7] = data_in[0];

		data_out[1] = data_in[6];
		data_out[6] = data_in[1];

		data_out[2] = data_in[5];
		data_out[5] = data_in[2];

		data_out[4] = data_in[3];
		data_out[3] = data_in[4];
		return out;
	}

	inline void DoubleSwap(double* number)
	{
		int *data;
		int dummy;

		data = (int*)number;
		dummy = data[0];
		data[0] = data[1];
		data[1] = dummy;
	}

#pragma pack(push,1)

	/**
	* From MIL-STD-2500a, Page 83
	* TABLE XVII. Data extension segment subheader format.
	**/
	typedef struct
	{
		char part_type[2]; //=='DE'
		char DESTAG[25];
		char DESVER[2];
		char DESSG[167];
		char DESOFLW[6];
		char DESITEM[3];
		char DESSHL[4];

	}nitf_de_header;

#define NITF_210 1
	/**
	* TABLE I. NITF file header.
	* From MIL-STD-2500A page 19
	**/
	typedef struct _nitf_header
	{
		char FHDR[4];// File Profile Name
		char FVER[5];// File Version
		char CLEVEL[2];// Complexity Level
		char STYPE[4];// Standard Type
		char OSTAID[10];// Originating Station ID
		char FDT[14];// File Date and Time
		char FTITLE[80];// File Title
		char FSCLAS;//File Security Class

		char FSCODE[40];//File codewords
		char FSCTLH[40];//File Control and Handling (FSCTLH)
		char FSREL[40];//File Releasing Instructions (FSREL) 20 s BCS paces (code 0x20)	
		char FSCAUT[20];//Classification Authority
		char FSCTLN[20];//File Security Control Number
		char FSDWNG[6];//File Security Downgrade

		char FSDEVT[40];//File Downgrading Event

		char FSCOP[5];//File Copy Number (FSCOP) 00000 5 digits - all zeros indicate there is no tracking of NITF file copies
		char FSCPYS[5];//File Number of Copies (FSCPYS) 00000 5 digits – all zeros indicate there is no tracking of NITF file copies
		char ENCRYP;//Encryption (ENCRYP) 0 1 digit - required default

		// This is a NITF 2.00 file without a background color, a later addendum made the ONAME field 24 chars and added
		// the background color.
		char ONAME[27];//Originator's Name (ONAME) 24 characters


		char OPHONE[18];//Originator's Phone Number (OPHONE) 18 characters
		char FL[12];//File Length (FL) 12 digits
		char HL[6];//NITF File Header Length (HL) 6 digits
		char NUMI[3];//Number of Image Segments (NUMI)  3 digits	
	}nitf_header;

	/**
	* [header section]
	* From MIL-STF-2411, Figure 3, page 53
	* Location LID_HeaderComponent points to this data.
	**/
	typedef struct _rpf_header
	{
		u_char endianness;
		u_short header_section_len;
		char fname[12];
		u_char new_indicator;
		char governing_spec_num[15];
		char governing_spec_date[8];
		char security_classification;
		char country_code[2];
		char security_release_marking[2];
		u_int location_section_offset;

		void SwapBytes()
		{
			header_section_len = ByteSwap(header_section_len);
			location_section_offset = ByteSwap(location_section_offset);
		}

	}rpf_header;

	/**
	* The TRE name and length is defined in MIL-STD-2500a,
	* "TABLE XV. Registered tagged record extension format."
	**/
	typedef struct _rpf_tre
	{
		char name[6];
		char szlen[5];
		rpf_header rpfhdr;
	}rpf_tre;



	/**
	* [location section]
	* From MIL-STF-2411, Figure 2, page 39
	**/
	typedef struct _rpf_toc_location_section
	{
		u_short section_length;
		u_int location_table_offset;
		u_short num_component_loc_recs;
		u_short component_loc_rec_len;
		u_int component_aggregate_len;
		void SwapBytes()
		{
			section_length = ByteSwap(section_length);
			location_table_offset = ByteSwap(location_table_offset);
			num_component_loc_recs = ByteSwap(num_component_loc_recs);
			component_loc_rec_len = ByteSwap(component_loc_rec_len);
			component_aggregate_len = ByteSwap(component_aggregate_len);

		}
	} rpf_toc_location_section;

	/**
	* [boundary rectangle section subheader]
	* From MIL-STF-2411, Figure 2, page 39
	**/
	typedef struct _rpf_toc_boundary_rect_section_hdr
	{
		u_int boundary_rect_offset;
		u_short num_boundary_records;
		u_short boundary_record_len;

		void SwapBytes()
		{
			boundary_rect_offset = ByteSwap(boundary_rect_offset);
			num_boundary_records = ByteSwap(num_boundary_records);
			boundary_record_len = ByteSwap(boundary_record_len);

		}
	}rpf_toc_boundary_rect_section_hdr;

	/**
	* [boundary rectangle record]
	* From MIL-STD-2411, Figure 2, page 39
	**/
	typedef struct _rpf_toc_boundary_rect_record
	{
		char product_data_type[5];
		char compress_ratio[5];
		char scale_resolution[12];
		char zone;
		char producer[5];
		double ULLat;
		double ULLon;
		double LLLat;
		double LLLon;
		double URLat;
		double URLon;
		double LRLat;
		double LRLon;
		double vert_resolution;
		double horiz_resolution;
		double vert_interval;
		double horiz_interval;

		u_int num_frames_north_south;
		u_int num_frames_east_west;

		void SwapBytes()
		{
			ULLat = ByteSwap(ULLat);
			ULLon = ByteSwap(ULLon);
			LLLat = ByteSwap(LLLat);
			LLLon = ByteSwap(LLLon);
			URLat = ByteSwap(URLat);
			URLon = ByteSwap(URLon);
			LRLat = ByteSwap(LRLat);
			LRLon = ByteSwap(LRLon);
			vert_resolution = ByteSwap(vert_resolution);
			horiz_resolution = ByteSwap(horiz_resolution);
			vert_interval = ByteSwap(vert_interval);
			horiz_interval = ByteSwap(horiz_interval);
			num_frames_north_south = ByteSwap(num_frames_north_south);
			num_frames_east_west = ByteSwap(num_frames_east_west);
		}
	}rpf_toc_boundary_rect_record;

	/**
	* [frame file index section]
	* From MIL-STF-2411, Figure 2, page 39
	**/
	typedef struct _rpf_frame_file_idx_section_sub_hdr
	{
		char highest_security_class;
		u_int frame_file_index_table_loc;
		u_int num_frame_file_index_recs;
		u_short num_pathname_recs;
		u_short frame_file_idx_rec_len;
		void SwapBytes()
		{
			frame_file_index_table_loc = ByteSwap(frame_file_index_table_loc);
			num_pathname_recs = ByteSwap(num_pathname_recs);
			num_frame_file_index_recs = ByteSwap(num_frame_file_index_recs);
			frame_file_idx_rec_len = ByteSwap(frame_file_idx_rec_len);
		}

	}rpf_frame_file_idx_section_sub_hdr;


	/**
	* [frame file index record]
	* From MIL-STD-2411, Figure 2, page 41
	**/
	typedef struct _rpf_frame_file_idx_record
	{
		u_short boundary_rect_recno;
		u_short frame_location_row_num;
		u_short frame_location_col_num;
		u_int pathname_record_offset;
		char frame_file_name[12];
		char geo_location[6];//AKA Zone
		char frame_file_security_class;
		char frame_file_security_country_code[2];
		char frame_file_security_release_mark[2];
		void SwapBytes()
		{
			boundary_rect_recno = ByteSwap(boundary_rect_recno);
			frame_location_row_num = ByteSwap(frame_location_row_num);
			frame_location_col_num = ByteSwap(frame_location_col_num);
			pathname_record_offset = ByteSwap(pathname_record_offset);
		}

	}rpf_frame_file_idx_record;

	/**
	* [component location record]
	* From MIL-SDF-2411, Figure 3, page 53
	**/
	typedef struct _nitf_location_table_entry
	{
		u_short id;
		u_int len;
		u_int offset;
		void SwapBytes()
		{
			id = ByteSwap(id);
			len = ByteSwap(len);
			offset = ByteSwap(offset);
		}

		void SetDefaults(u_short in_id, u_int in_len, u_int in_offset)
		{
			id = in_id;
			len = in_len;
			offset = in_offset;
		}
	}nitf_location_table_entry;

	/**
	* [pathname record]
	* From MIL-STF-2411, Figure 2, page 41
	**/
	typedef struct _rpf_pathname_record
	{
		u_short pathlen;
		char pathname[1];// Defined as one byte here, depends on pathlen
		void SwapBytes()
		{
			pathlen = ByteSwap(pathlen);
		}
	}rpf_pathname_record;


}