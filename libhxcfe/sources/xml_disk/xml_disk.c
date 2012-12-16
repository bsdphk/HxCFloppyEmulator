/*
//
// Copyright (C) 2006 - 2012 Jean-Fran�ois DEL NERO
//
// This file is part of the HxCFloppyEmulator library
//
// HxCFloppyEmulator may be used and distributed without restriction provided
// that this copyright statement is not removed from the file and that any
// derivative work contains the original copyright notice and the associated
// disclaimer.
//
// HxCFloppyEmulator is free software; you can redistribute it
// and/or modify  it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// HxCFloppyEmulator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with HxCFloppyEmulator; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
*/
///////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//
//-----------H----H--X----X-----CCCCC----22222----0000-----0000------11----------//
//----------H----H----X-X-----C--------------2---0----0---0----0--1--1-----------//
//---------HHHHHH-----X------C----------22222---0----0---0----0-----1------------//
//--------H----H----X--X----C----------2-------0----0---0----0-----1-------------//
//-------H----H---X-----X---CCCCC-----222222----0000-----0000----1111------------//
//-------------------------------------------------------------------------------//
//----------------------------------------------------- http://hxc2001.free.fr --//
///////////////////////////////////////////////////////////////////////////////////
// File : xml_disk.c
// Contains: raw disk loader/creator
//
// Written by:	DEL NERO Jean Francois
//
// Change History (most recent first):
///////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <expat.h>

#include "libhxcfe.h"
#include "floppy_loader.h"
#include "floppy_utils.h"

#include "xml_disk.h"
#include "./DiskLayouts/LayoutsIndex.h"
#include "./packer/pack.h"

typedef struct parse_stack_
{
	int state;
	int cur_track;
}parse_stack;

typedef struct track_state_
{
	int base_adress;
	int track_size;
	int set;
}track_state;

typedef struct app_data
{
	HXCFLOPPYEMULATOR* floppycontext;

	int current_state;
	void * p;

	int stack_ptr;

	int layout_id;
	parse_stack statestack[32];

	FBuilder * fb;
	FLOPPY* floppy;

	track_state ts[256*2];

	int xmlcheck;

	char * name[512];
	char * description[512];

	unsigned char * image_data;
	int buffer_size;
	int track_size;

	int skew_per_track;
	int skew_per_side;

	int interface_mode;
	int double_step;

} AppData;

int generateDisk(AppData *ad,unsigned char * diskdata,int buffersize)
{
	int i,j,ret;
	int numberofsector,numberoftrack,numberofside,sectorsize;
	int type;
	int statetracknb;
	unsigned int rpm;
	int offset;

	offset = 0;
	sectorsize = hxcfe_getCurrentSectorSize(ad->fb);
	numberofsector = hxcfe_getCurrentNumberOfSector(ad->fb);
	numberoftrack = hxcfe_getCurrentNumberOfTrack(ad->fb);
	numberofside = hxcfe_getCurrentNumberOfSide(ad->fb);
	rpm = hxcfe_getCurrentRPM(ad->fb);
	type = hxcfe_getCurrentTrackType(ad->fb);

	for(i = 0 ; i < numberoftrack ; i++ )
	{
		for(j = 0 ; j < numberofside ; j++ )
		{
			statetracknb = (i<<1) | (j&1);

			if(!ad->ts[statetracknb].set)
			{

				hxcfe_pushTrack (ad->fb,rpm,i,j,type);

				hxcfe_setTrackSkew(ad->fb, (hxcfe_getCurrentSkew(ad->fb) + ( ad->skew_per_track * i ) + ( j * ad->skew_per_side ))%numberofsector );

				if( ( (offset + (sectorsize * numberofsector) )  > buffersize) || !diskdata )
				{
					ret = hxcfe_addSectors(ad->fb,j,i,0,0,numberofsector);
				}
				else
				{
					ret = hxcfe_addSectors(ad->fb,j,i,&diskdata[offset],(sectorsize * numberofsector),numberofsector);
				}

				offset = offset + (sectorsize * numberofsector);
				hxcfe_popTrack (ad->fb);
			}
			else
			{
				offset = ad->ts[statetracknb].base_adress + ad->ts[statetracknb].track_size;
			}
		}
	}

	return HXCFE_NOERROR;
}

int dumpDisk(AppData *ad,unsigned char * diskdata,int buffersize,FLOPPY * fp)
{
	int i,j,ret;
	int numberofsector,numberoftrack,numberofside,sectorsize;
	int type;
	int statetracknb;
	unsigned int rpm;
	int offset;

	offset = 0;
	sectorsize = hxcfe_getCurrentSectorSize(ad->fb);
	numberofsector = hxcfe_getCurrentNumberOfSector(ad->fb);
	numberoftrack = hxcfe_getCurrentNumberOfTrack(ad->fb);
	numberofside = hxcfe_getCurrentNumberOfSide(ad->fb);
	rpm = hxcfe_getCurrentRPM(ad->fb);
	type = hxcfe_getCurrentTrackType(ad->fb);

	for(i = 0 ; i < numberoftrack ; i++ )
	{
		for(j = 0 ; j < numberofside ; j++ )
		{
			statetracknb = (i<<1) | (j&1);

			if(!ad->ts[statetracknb].set)
			{

				hxcfe_pushTrack (ad->fb,rpm,i,j,type);

				hxcfe_setTrackSkew(ad->fb, (hxcfe_getCurrentSkew(ad->fb) + ( ad->skew_per_track * i ) + ( j * ad->skew_per_side ))%numberofsector );

				if( ( (offset + (sectorsize * numberofsector) )  > buffersize) || !diskdata )
				{
					ret = hxcfe_addSectors(ad->fb,j,i,0,0,numberofsector);
				}
				else
				{
					ret = hxcfe_addSectors(ad->fb,j,i,&diskdata[offset],(sectorsize * numberofsector),numberofsector);
				}

				offset = offset + (sectorsize * numberofsector);
				hxcfe_popTrack (ad->fb);
			}
			else
			{
				offset = ad->ts[statetracknb].base_adress + ad->ts[statetracknb].track_size;
			}
		}
	}

	return HXCFE_NOERROR;
}

int getnewstate(char * keyword, int currentstate)
{
	int i;

	i=0;

	while(keyword_list[i].keyword)
	{
		if( !strcmp( keyword_list[i].keyword ,keyword) && ( currentstate == keyword_list[i].prev_state) )
		{
			return keyword_list[i].state;
		}

		i++;
	}

	return -1;
}

int getstate(char * keyword, int currentstate)
{
	int i;

	i=0;

	while(keyword_list[i].keyword)
	{
		if( !strcmp( keyword_list[i].keyword ,keyword) && ( currentstate == keyword_list[i].state) )
		{
			return keyword_list[i].state;
		}

		i++;
	}

	return -1;
}

unsigned long ahextoi(char * str)
{
	unsigned long hexval;
	int i;
	unsigned char c;

	hexval = 0;
	i = 0;

	while(str[i])
	{
		c = str[i];
		
		if( ( c>='0' && c<='9' ) || ( c>='a' && c<='f' ) || ( c>='A' && c<='F' ) )
		{
			hexval = hexval << 4;

			if( c>='0' && c<='9' )
			{
				hexval = hexval | (c - '0');
			}
			else
			{
				if(( c>='a' && c<='f' ))
				{
					hexval = hexval | ( (c - 'a') + 10);
				}
				else
				{
					hexval = hexval | ( (c - 'A') + 10);
				}

			}
		}
		else
		{
			hexval = 0;
		}

		i++;
	}

	return hexval;
}

static void XMLCALL charhandler(void *data, const char *s, int len) 
{
	AppData	*ad = (AppData *) data;
	char buffer[512];
	int track;

	memcpy(buffer,s,len);
	buffer[len]=0;

	switch(ad->current_state)
	{
		case DISK_LAYOUT_NAME:
			strcpy((char*)&ad->name,(char*)&buffer);
		break;
		case DISK_LAYOUT_DESCRIPTION:
			strcpy((char*)&ad->description,(char*)&buffer);
		break;
		case INTERFACE_MODE:
			ad->interface_mode = hxcfe_getFloppyInterfaceModeID(ad->floppycontext,(char*)&buffer);
		break;
		case DOUBLE_STEP:
			if(!strcmp(buffer,"on"))
				ad->double_step = 1;

			if(!strcmp(buffer,"off"))
				ad->double_step = 0;
		break;
		case FILESIZE:

		break;
/*		case TRACKSIZE:
			ad->track_size = atoi(buffer);
		break;*/
		case NUMBEROFTRACK:
			if(!ad->xmlcheck)
				hxcfe_setNumberOfTrack (ad->fb,(unsigned short)atoi(buffer));
		break;
		case NUMBEROFSIDE:
			if(!ad->xmlcheck)
				hxcfe_setNumberOfSide (ad->fb,(unsigned char)atoi(buffer));
		break;
		case NUMBEROFSECTOR:
			if(!ad->xmlcheck)
				hxcfe_setNumberOfSector (ad->fb,(unsigned short)atoi(buffer));
		break;
		case SECTORSIZE:
			if(!ad->xmlcheck)
				hxcfe_setSectorSize(ad->fb,atoi(buffer));
		break;
		case INTERLEAVE:
			if(!ad->xmlcheck)
				hxcfe_setTrackInterleave(ad->fb,atoi(buffer));
		break;
		case SKEW:
			if(!ad->xmlcheck)
				hxcfe_setTrackSkew(ad->fb,atoi(buffer));
		break;
		case SKEW_PER_TRACK:
			if(!ad->xmlcheck)
				ad->skew_per_track = atoi(buffer);
		break;
		case SKEW_PER_SIDE:
			if(!ad->xmlcheck)
				ad->skew_per_side = atoi(buffer);
		break;
		case FORMATVALUE:
			if(!ad->xmlcheck)
				hxcfe_setSectorFill (ad->fb,(unsigned char)ahextoi(buffer));
		break;
		case PREGAP:
			if(!ad->xmlcheck)
				hxcfe_setTrackPreGap (ad->fb,(unsigned short)atoi(buffer));
		break;
		case GAP3:
			if(!ad->xmlcheck)
				hxcfe_setSectorGap3 (ad->fb,(unsigned char)atoi(buffer));
		break;
		case RPM:
			if(!ad->xmlcheck)
				hxcfe_setRPM (ad->fb,(unsigned short)atoi(buffer));
		break;
		case SECTORIDSTART:
			if(!ad->xmlcheck)
				hxcfe_setStartSectorID(ad->fb,(unsigned char)atoi(buffer));
		break;
		case BITRATE:
			if(!ad->xmlcheck)
				hxcfe_setTrackBitrate(ad->fb,atoi(buffer));
		break;
		case DATAOFFSET:
			if(!ad->xmlcheck)
			{
				track = ad->statestack[ad->stack_ptr].cur_track;
				ad->ts[track].base_adress =  ahextoi(buffer);
			}
		break;
	}
}

static void XMLCALL start(void *data, const char *el, const char **attr)
{
	int i;
	int newstate;
	int track,side,sector,sectorsize;
	AppData	*ad = (AppData *) data;

	newstate = getnewstate((char*)el, ad->current_state);
	if ( newstate >= 0 )
	{
		ad->stack_ptr++;
		memcpy(&ad->statestack[ad->stack_ptr], &ad->statestack[ad->stack_ptr-1],sizeof(parse_stack));
		ad->statestack[ad->stack_ptr].state = newstate;
		ad->current_state = newstate;

		if(!ad->xmlcheck)
		{
			switch(ad->current_state)
			{
				case LAYOUT:
					ad->fb = hxcfe_initFloppy(ad->floppycontext,80,2);
				break;
				case TRACK:
					i=0;
					side=0;
					track=0;

					while(attr[i] && strcmp("track_number",attr[i]) ) 
					{
						i++;
					}
					if(attr[i+1])
					{
						track = atoi(attr[i+1]);
					}				

					i=0;
					while(attr[i] && strcmp("side_number",attr[i]) ) 
					{
						i++;
					}
					if(attr[i+1])
					{
						side = atoi(attr[i+1]);
					}

					ad->statestack[ad->stack_ptr].cur_track = (track<<1) | (side&1);
					ad->ts[(track<<1) | (side&1)].set=1;

					hxcfe_pushTrackPFS (ad->fb,track,side);
				break;

				case SECTOR:
					i=0;
					while(attr[i] && strcmp("sector_id",attr[i]) ) 
					{
						i++;
					}
					if(attr[i+1])
					{
						sector = atoi(attr[i+1]);
					}

					i=0;
					while(attr[i] && strcmp("sector_size",attr[i]) ) 
					{
						i++;
					}
					if(attr[i+1])
					{
						sectorsize = atoi(attr[i+1]);
					}
					
					track = ad->statestack[ad->stack_ptr].cur_track;

					hxcfe_pushSector(ad->fb);
					hxcfe_setSectorID(ad->fb,(unsigned char)sector);
					hxcfe_setSectorSize(ad->fb,sectorsize);
					if(ad->ts[track].base_adress + ad->ts[track].track_size + sectorsize < ad->buffer_size)
					{
						hxcfe_setSectorData(ad->fb,&ad->image_data[ad->ts[track].base_adress + ad->ts[track].track_size],sectorsize);
					}
					ad->ts[track].track_size +=  sectorsize;

				break;

			}
		}
	}
	else
	{
		XML_StopParser(ad->p, 0);
	}
}

static void XMLCALL end(void *data, const char *el)
{
	AppData *ad = (AppData *) data;
	int state;

	state = getstate((char*)el, ad->current_state);

	if(!ad->xmlcheck)
	{

		switch(state)
		{
			case LAYOUT:
				generateDisk(ad,ad->image_data,ad->buffer_size);
				ad->floppy = hxcfe_getFloppy (ad->fb);
				if(ad->interface_mode>=0)
					hxcfe_floppySetInterfaceMode(ad->floppycontext,ad->floppy,ad->interface_mode);
			break;

			case TRACK:		
				hxcfe_popTrack (ad->fb);
			break;

			case SECTOR:
				hxcfe_popSector(ad->fb);
			break;
		}
	}

	if(ad->stack_ptr)
	{
		ad->stack_ptr--;
	}

	ad->current_state = ad->statestack[ad->stack_ptr].state;

}

static void XMLCALL ns_start(void *data, const char *prefix, const char *uri)
{

}

static void XMLCALL ns_end(void *data, const char *prefix)
{

}

XmlFloppyBuilder* hxcfe_initXmlFloppy(HXCFLOPPYEMULATOR* floppycontext)
{
	AppData *ad;
	XmlFloppyBuilder * rfw;
	int i;

	rfw = malloc(sizeof(XmlFloppyBuilder));
	if(rfw)
	{
		memset(rfw,0,sizeof(XmlFloppyBuilder));

		rfw->xml_parser = XML_ParserCreate(NULL);
		ad = malloc(sizeof(AppData));
		if(!ad)
		{
			return 0;
		}
		memset(ad,0,sizeof(AppData));
		
		ad->interface_mode = -1;
		
		i=0;
		while( disklayout_list[i])
		{
			if(disklayout_list[i]->unpacked_data) 
			{
				free(disklayout_list[i]->unpacked_data);
				disklayout_list[i]->unpacked_data = 0;
			}

			disklayout_list[i]->unpacked_data = data_unpack(disklayout_list[i]->data,disklayout_list[i]->csize,0,disklayout_list[i]->size);
			i++;
		}

		ad->floppycontext = floppycontext;
		ad->current_state = ENTRY_STATE;
		ad->stack_ptr = 0;
		memset(ad->statestack,0xFF,sizeof(int) * 32);
		ad->statestack[0].state = ad->current_state;
		ad->p = rfw->xml_parser;

		memset(ad->ts,0,sizeof(track_state)*256);

		XML_ParserReset(rfw->xml_parser, NULL);

		XML_SetUserData(rfw->xml_parser, (void *) ad);
		XML_SetElementHandler(rfw->xml_parser, start, end);

		XML_SetCharacterDataHandler(rfw->xml_parser, charhandler);
		XML_SetNamespaceDeclHandler(rfw->xml_parser, ns_start, ns_end);

		rfw->ad = ad;
	}

	return rfw;
}

void hxcfe_deinitXmlFloppy(XmlFloppyBuilder* context)
{
	int i;

	XML_ParserFree(context->xml_parser);

	i=0;
	while( disklayout_list[i])
	{
		if(disklayout_list[i]->unpacked_data) 
		{
			free(disklayout_list[i]->unpacked_data);
			disklayout_list[i]->unpacked_data = 0;
		}
		i++;
	}

	free(context);
}

int hxcfe_getXmlLayoutID(XmlFloppyBuilder* context,char * container)
{
	int i;
	AppData	*ad = (AppData *) context->ad;

	i = 0;
	do
	{
		XML_ParserReset(context->xml_parser, NULL);
		XML_SetUserData(context->xml_parser, (void *) ad);
		XML_SetElementHandler(context->xml_parser, start, end);
		XML_SetCharacterDataHandler(context->xml_parser, charhandler);
		XML_SetNamespaceDeclHandler(context->xml_parser, ns_start, ns_end);

		ad->xmlcheck = 1;
		XML_Parse(context->xml_parser, disklayout_list[i]->unpacked_data, disklayout_list[i]->size, 1);

		if(!strcmp((char*)ad->name,container))
		{
			return i;
		}

		memset(ad->name,0,512);

		i++;
	}while(disklayout_list[i]);

	return -1;
}

const char* hxcfe_getXmlLayoutDesc(XmlFloppyBuilder* context,int moduleID)
{
	AppData	*ad = (AppData *) context->ad;

	if(hxcfe_numberOfXmlLayout(context) > moduleID)
	{
		ad->xmlcheck = 1;
		XML_ParserReset(context->xml_parser, NULL);
		XML_SetUserData(context->xml_parser, (void *) ad);
		XML_SetElementHandler(context->xml_parser, start, end);
		XML_SetCharacterDataHandler(context->xml_parser, charhandler);
		XML_SetNamespaceDeclHandler(context->xml_parser, ns_start, ns_end);

		XML_Parse(context->xml_parser, disklayout_list[moduleID]->unpacked_data, disklayout_list[moduleID]->size, 1);

		return (const char*)ad->description;
	}

	return NULL;
}

const char* hxcfe_getXmlLayoutName(XmlFloppyBuilder* context,int moduleID)
{
	AppData	*ad = (AppData *) context->ad;

	if(hxcfe_numberOfXmlLayout(context) > moduleID)
	{
		ad->xmlcheck = 1;
		XML_ParserReset(context->xml_parser, NULL);
		XML_SetUserData(context->xml_parser, (void *) ad);
		XML_SetElementHandler(context->xml_parser, start, end);
		XML_SetCharacterDataHandler(context->xml_parser, charhandler);
		XML_SetNamespaceDeclHandler(context->xml_parser, ns_start, ns_end);

		XML_Parse(context->xml_parser, disklayout_list[moduleID]->unpacked_data, disklayout_list[moduleID]->size, 1);

		return (const char*)ad->name;
	}

	return NULL;
}

int	hxcfe_numberOfXmlLayout(XmlFloppyBuilder* context)
{
	int i;

	i = 0;
	while( disklayout_list[i] )
	{
		i++;		
	}

	return i;
}


int	hxcfe_selectXmlFloppyLayout(XmlFloppyBuilder* context,int layoutid)
{
	AppData	*ad = (AppData *) context->ad;

	if(hxcfe_numberOfXmlLayout(context) > layoutid)
	{
		ad->layout_id = layoutid;
		return HXCFE_NOERROR;
	}
	return HXCFE_BADPARAMETER;
}

FLOPPY* hxcfe_generateXmlFloppy (XmlFloppyBuilder* context,unsigned char * rambuffer,unsigned buffersize)
{
	AppData *ad;

	ad = context->ad;

	ad->xmlcheck = 0;
	
	ad->image_data = rambuffer;
	ad->buffer_size = buffersize;

	XML_ParserReset(context->xml_parser, NULL);
	XML_SetUserData(context->xml_parser, (void *) ad);
	XML_SetElementHandler(context->xml_parser, start, end);
	XML_SetCharacterDataHandler(context->xml_parser, charhandler);
	XML_SetNamespaceDeclHandler(context->xml_parser, ns_start, ns_end);

	XML_Parse(context->xml_parser, disklayout_list[ad->layout_id]->unpacked_data, disklayout_list[ad->layout_id]->size, 1);

	return ad->floppy;
}

FLOPPY* hxcfe_generateXmlFileFloppy (XmlFloppyBuilder* context,char *file)
{
	FILE * f;
	unsigned int filesize;
	FLOPPY* ret;
	unsigned char * buffer;

	f = fopen(file,"rb");
	if(f)
	{
		fseek(f,0,SEEK_END);
		filesize = ftell(f);
		fseek(f,0,SEEK_SET);

		buffer = malloc(filesize);
		memset(buffer,0,filesize);
		fread(buffer, filesize, 1, f);
		fclose(f);

		ret = hxcfe_generateXmlFloppy (context,buffer,filesize);

		return ret; 
	}

	return 0;
}

/*int hxcfe_checkCompatibilityXmlFileFloppy (XmlFloppyBuilder* context,FLOPPY * fp)
{
	FLOPPY * refFloppy;

	refFloppy = hxcfe_generateXmlFloppy (context,0,0);
	if( refFloppy )
	{

		

	}
}*/
