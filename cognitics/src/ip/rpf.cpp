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

#include <ip/rpf.h>
#include <ccl/ccl.h>
#include <fstream>

namespace ip {

	static int GetField(char *field, int field_len)
	{
		if (field_len>999)
		{
			return -1;
		}
		char buf[1000];
		memcpy(buf, field, field_len);
		buf[field_len] = 0;// null terminate
		char *bufp = buf;
		// Now get rid of leading 0s so it's not turned into octal.
		for (int i = 0; i<field_len; i++)
		{
			if (buf[i] == '0')
			{
				bufp++;
				break;
			}
		}
		if (*bufp == 0)
		{
			return 0;
		}
		return atoi(bufp);
	}


	std::vector<std::string> getFileListFromTOC(const std::string &tocfile)
	{
		std::vector<std::string> ret;
		ccl::FileInfo tocfileinfo(tocfile);
		std::string base_dir = tocfileinfo.getDirName();

		size_t tocLen = 0;
		FILE *in = fopen(tocfile.c_str(), "rb");
		if (in)
		{
			int buffer_len;
			u_char *buffer;

			fseek(in, 0, SEEK_END);
			tocLen = ftell(in);
			// Back to the beginning.
			fseek(in, 0, SEEK_SET);
			//sanity check the size, using 100mb as an arbitraty upper max
			//any bigger must be something else!
			if (tocLen > (100 * 1024 * 1024))
			{
				fclose(in);
				return ret;
			}
			buffer_len = tocLen;
			buffer = new u_char[buffer_len];
			// Read the whole file into memory.
			int bytes_read = fread(buffer, 1, buffer_len, in);
			fclose(in);

			int currentPos = 0;
			currentPos += 9;	// FHDR
			currentPos += 2;	// CLEVEL
			currentPos += 4;	// STYPE
			currentPos += 10;	// OSTAID
			currentPos += 14;	// FDT
			currentPos += 80;	// FTITLE
			currentPos += 1;	// FSCLAS
			currentPos += 40;	// FSCODE
			currentPos += 40;	// FSCTLH
			currentPos += 40;	// FSREL
			currentPos += 20;	// FSCAUT
			currentPos += 20;	// FSCTLN

			u_char* fsdwng = &buffer[currentPos];
			currentPos += 6;    // FSDWNG
			// if FSDWNG is 999998, the 40-char FSDEVT field follows
			if(memcmp((char*)fsdwng, "999998", 6) == 0)
				currentPos += 40;  // FSDEVT

			currentPos += 5;	// FSCOP
			currentPos += 5;	// FSCPYS
			currentPos += 1;	// ENCRYP
			currentPos += 27;	// ONAME
			currentPos += 18;	// OPHONE
			currentPos += 12;	// FL
			currentPos += 6;	// HL

			int numi = GetField((char*)&buffer[currentPos], 3);
			currentPos += 3;
			for (int i = 0; i<numi; i++)
			{
				currentPos += 16;
				// If NUMI > 0 then skip plus 6 (the size of LISH001)
				// If NUMI > 0 then skip 10 more (the size of LI001)
			}
			u_char * p = &buffer[currentPos];
			int nums = GetField((char *)p, 3);
			currentPos += 3;
			for (int i = 0; i<nums; i++)
			{
				// LSSH001 = 4 bytes
				// LS001 = 6 bytes
				currentPos += 10;
			}

			p = &buffer[currentPos];
			int numl = GetField((char *)p, 3);// Also known as numx
			currentPos += 3;
			for (int i = 0; i<numl; i++)
			{
				// LLSH001 = 4 bytes
				// LL001 = 3 bytes
				currentPos += 7;
			}

			p = &buffer[currentPos];
			int numt = GetField((char *)p, 3);
			currentPos += 3;
			for (int i = 0; i<numt; i++)
			{
				// LTSH001 = 4 bytes
				// LT001 = 5 bytes
				currentPos += 9;
			}
			p = &buffer[currentPos];
			int numdes = GetField((char *)p, 3);
			currentPos += 3;
			for (int i = 0; i<numdes; i++)
			{
				p = &buffer[currentPos];
				int ldsh001 = GetField((char *)p, 4);//209
				currentPos += 4;
				p = &buffer[currentPos];
				int ld001 = GetField((char *)p, 9);//428
				currentPos += 9;
			}
			p = &buffer[currentPos];
			//numres = 3 bytes
			int numres = GetField((char *)p, 3);
			currentPos += 3;
			for (int i = 0; i<numres; i++)
			{
				p = &buffer[currentPos];
				////LRSH001 = 4 bytes
				int lrsh001 = GetField((char *)p, 4);
				currentPos += 4;
				p = &buffer[currentPos];
				//LR001 = 7 bytes
				int lr001 = GetField((char *)p, 7);
				currentPos += 7;
			}

			//UDHDL = 5 bytes
			p = &buffer[currentPos];
			int udhdl = GetField((char *)p, 5);
			currentPos += 5;
			if(udhdl > 0)
			{
				//UDHOFL = 3 bytes
				p = &buffer[currentPos];
				int udofl = GetField((char *)p, 3);
				currentPos += 3;
			}

			//UDHD = * bytes
			p = &buffer[currentPos];
			//Here is the RPFHDR
			rpf_tre *rpftre = (rpf_tre *)p;
			if(udhdl > 0)
				currentPos += (udhdl - 3);// We subtract 3 because we already accounted for UDHOFL

			//XHDL = 5 bytes
			p = &buffer[currentPos];
			int xhdl = GetField((char *)p, 5);
			currentPos += 5;
			//XHD = **bytes
			p = &buffer[currentPos];
			currentPos += xhdl;

			// Next is the DE section, starts at nitf_header_len
			p = &buffer[currentPos];
			nitf_de_header *de_header = (nitf_de_header *)p;
			int de_header_len = GetField(de_header->DESSHL, 4);
			currentPos += de_header_len + sizeof(nitf_de_header);

			p = &buffer[currentPos];
			char rpf_tre_name[6];
			memcpy(rpf_tre_name, p, 6);
			currentPos += 6;
			p = &buffer[currentPos];
			char rpf_tre_slen[5];
			memcpy(rpf_tre_slen, p, 5);
			currentPos += 5;
			p = &buffer[currentPos];

			rpftre->rpfhdr.SwapBytes();

			currentPos = rpftre->rpfhdr.location_section_offset;
			p = &buffer[currentPos];
			rpf_toc_location_section *toc_location = (rpf_toc_location_section *)p;
			toc_location->SwapBytes();
			currentPos += sizeof(rpf_toc_location_section);
			nitf_location_table_entry **locations = new nitf_location_table_entry *[toc_location->num_component_loc_recs];

			rpf_toc_boundary_rect_section_hdr *boundary_rect_hdr = NULL;
			rpf_frame_file_idx_section_sub_hdr *frame_file_idx_hdr = NULL;
			rpf_frame_file_idx_record **frame_file_idx_rec = NULL;
			rpf_toc_boundary_rect_record **boundary_rect_records = NULL;
			for (int i = 0; i<toc_location->num_component_loc_recs; i++)
			{
				p = &buffer[currentPos];
				locations[i] = (nitf_location_table_entry *)p;
				currentPos += toc_location->component_loc_rec_len;
			}
			int frame_file_index_section = 0;
			int path_section_offset = 0;
			for (int i = 0; i<toc_location->num_component_loc_recs; i++)
			{
				locations[i]->SwapBytes();
				currentPos = locations[i]->offset;
				p = &buffer[currentPos];

				switch (locations[i]->id)
				{
				case LID_BoundaryRectangleTable:
				{
					// We need the header first!
					if (boundary_rect_hdr)
					{
						for (int i = 0; i<boundary_rect_hdr->num_boundary_records; i++)
						{
							p = &buffer[currentPos];
							boundary_rect_records[i] = (rpf_toc_boundary_rect_record *)p;
							boundary_rect_records[i]->SwapBytes();
							currentPos += boundary_rect_hdr->boundary_record_len;
						}
					}
				}
					break;
				case LID_BoundaryRectangleSectionSubheader:
				{
					boundary_rect_hdr = (rpf_toc_boundary_rect_section_hdr *)p;
					boundary_rect_hdr->SwapBytes();
					boundary_rect_records = new rpf_toc_boundary_rect_record*[boundary_rect_hdr->num_boundary_records];
					currentPos += sizeof(rpf_toc_boundary_rect_section_hdr);

				}
					break;
				case LID_FrameFileIndexSectionSubHeader:
				{
					path_section_offset = currentPos + sizeof(rpf_frame_file_idx_section_sub_hdr);
					frame_file_idx_hdr = (rpf_frame_file_idx_section_sub_hdr *)p;
					frame_file_idx_hdr->SwapBytes();
				}
					break;

				case LID_FrameFileIndexSubsection:
				{
					frame_file_index_section = currentPos;
					int numrecs = frame_file_idx_hdr->num_frame_file_index_recs;
					frame_file_idx_rec = new rpf_frame_file_idx_record *[numrecs];
					for (int i = 0; i<numrecs; i++)
					{
						p = &buffer[currentPos];
						frame_file_idx_rec[i] = (rpf_frame_file_idx_record *)p;
						frame_file_idx_rec[i]->SwapBytes();
						currentPos += frame_file_idx_hdr->frame_file_idx_rec_len;
						rpf_pathname_record *pathrec = (rpf_pathname_record *)&buffer[frame_file_index_section + frame_file_idx_rec[i]->pathname_record_offset];
						//Don't swap the actual record because multiple files refer to it
						u_short pathlen = ByteSwap(pathrec->pathlen);
						char *pathsz = new char[pathlen + 1];
						memcpy(pathsz, pathrec->pathname, pathlen);
						pathsz[pathlen] = 0;
						std::string filepath = ccl::joinPaths(base_dir, pathsz);
						char filename[13];
						filename[12] = 0;
						memcpy(filename, frame_file_idx_rec[i]->frame_file_name, 12);
						filepath = ccl::joinPaths(filepath, filename);
						ccl::FileInfo fi(filepath);
						std::string ext = fi.getSuffix();
						std::string basefile = fi.getBaseName();
						std::transform(basefile.begin(), basefile.end(), basefile.begin(), ::toupper);

						std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
						if (ext != "ovr")
						{
							ret.push_back(filepath);
						}
						delete[] pathsz;
					}
				}
					break;
				}
			}
			p = &buffer[currentPos];
			
			/*
			rpf_pathname_record *pathrec1 = (rpf_pathname_record *)p;
			pathrec1->SwapBytes();

			char *pathsz = new char[pathrec1->pathlen + 1];
			memset(pathsz, 0, pathrec1->pathlen + 1);
			memcpy(pathsz, pathrec1->pathname, pathrec1->pathlen);
			std::string thepath(pathsz);
			//ret.push_back(thepath);
			delete[] pathsz;
			
			
			currentPos += (sizeof(u_short) + pathrec1->pathlen);
			p = &buffer[currentPos];
			*/

			delete buffer;
		}

		return ret;
	}
	
}