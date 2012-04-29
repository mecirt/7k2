/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 1999 Enlight Software Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Filename    : OLZW.H
// Description : header file of LZW compression/decompression


#ifndef __OLZW_H
#define __OLZW_H

struct Dictionary;
class File;

class BitStream				// for reading token less than 16 bits
{
public:
	long bit_offset;

public:
	BitStream();
	virtual ~BitStream();
	virtual unsigned short input_bits(unsigned stringLen);
	virtual void output_bits(unsigned short stringCode, unsigned stringLen);
};

class BitMemStream : public BitStream
{
protected:
	unsigned char *bytePtr;

public:
	BitMemStream(unsigned char *p);
	unsigned short input_bits(unsigned stringLen);
	void output_bits(unsigned short stringCode, unsigned stringLen);
};


class BitFileRead : public BitStream
{
protected:
	File *filePtr;
	long last_offset;
	unsigned long residue;		// always the 4 bytes at last_offset

public:
	BitFileRead(File *f);
	unsigned short input_bits(unsigned stringLen);
	void output_bits(unsigned short stringCode, unsigned stringLen);
};


class BitFileWrite : public BitStream
{
protected:
	File *filePtr;
	long residue_len;
	unsigned long residue;		// always the 4 bytes at last_offset

public:
	BitFileWrite(File *f);
	~BitFileWrite();
	unsigned short input_bits(unsigned stringLen);
	void output_bits(unsigned short stringCode, unsigned stringLen);
};


class Lzw
{
private:
	Dictionary *dict;
	unsigned char *decode_stack;
	unsigned short next_code;
	unsigned current_code_bits;
	unsigned short next_bump_code;

public:
	Lzw();
	~Lzw();
	void initialize_storage();
	void free_storage();

	// nul output, to find output size
	long compress( unsigned char *inPtr, long inByteLen);

	// compressed data in memory
	long compress( unsigned char *inPtr, long inByteLen, unsigned char *outPtr);
	long expand( unsigned char *inPtr, long inBitLen, unsigned char *outPtr);

	// compressed data in file
	long compress( unsigned char *inPtr, long inByteLen, File *outFile);
	long expand( File *inFile, unsigned char *outPtr);

	long basic_compress( unsigned char *inPtr, long inByteLen, BitStream *outStream);
	long basic_expand( BitStream *inStream, unsigned char *outPtr);

private:
	void initialize_dictionary();
	unsigned short find_child_node( unsigned short parent_code, unsigned char child_character );
	unsigned int decode_string( unsigned int count, unsigned short code );
};

#endif
