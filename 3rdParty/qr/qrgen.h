#ifndef __QRGEN_H__
#define __QRGEN_H__

#include "stdafx.h"
#include <string>
#include <errno.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>
#include <Windows.h>

#include "LibQREncode\qrencode.h"

//
//// BMP defines
//
//typedef unsigned short	WORD;
//typedef unsigned long	DWORD;
//typedef signed long		LONG;
//
//#define BI_RGB			0L
//
//#pragma pack(push, 2)
//
//typedef struct
//{
//	WORD    bfType;
//	DWORD   bfSize;
//	WORD    bfReserved1;
//	WORD    bfReserved2;
//	DWORD   bfOffBits;
//} BITMAPFILEHEADER;
//
//typedef struct
//{
//	DWORD      biSize;
//	LONG       biWidth;
//	LONG       biHeight;
//	WORD       biPlanes;
//	WORD       biBitCount;
//	DWORD      biCompression;
//	DWORD      biSizeImage;
//	LONG       biXPelsPerMeter;
//	LONG       biYPelsPerMeter;
//	DWORD      biClrUsed;
//	DWORD      biClrImportant;
//} BITMAPINFOHEADER;
//
//#pragma pack(pop)

extern int qrGenerate(std::string input, std::string outPath, int scale, int r = 0, int g = 0, int b = 0) {
	unsigned int	unWidth, x, y, l, n, unWidthAdjusted, unWidthWithBorder,  unDataBytes;
	unsigned char*	pRGBData, *pSourceData, *pDestData;
	QRcode*			pQRC;
	FILE*			f;

	if (pQRC = QRcode_encodeString(input.c_str(), 0, QR_ECLEVEL_Q, QR_MODE_8, 1))
	{
		unWidth = pQRC->width;
		unWidthAdjusted = unWidth * scale * 3;
		if (unWidthAdjusted % 4)
			unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
		unDataBytes = unWidthAdjusted * unWidth * scale;

		// Allocate pixels buffer

		if (!(pRGBData = (unsigned char*)malloc(unDataBytes)))
		{
			printf("Out of memory");
			exit(-1);
		}

		// Preset to white

		memset(pRGBData, 0xff, unDataBytes);


		// Prepare bmp headers

		BITMAPFILEHEADER kFileHeader;
		kFileHeader.bfType = 0x4d42;  // "BM"
		kFileHeader.bfSize = sizeof(BITMAPFILEHEADER) +
			sizeof(BITMAPINFOHEADER) +
			unDataBytes;
		kFileHeader.bfReserved1 = 0;
		kFileHeader.bfReserved2 = 0;
		kFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) +
			sizeof(BITMAPINFOHEADER);

		BITMAPINFOHEADER kInfoHeader;
		kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		kInfoHeader.biWidth = unWidth * scale;
		kInfoHeader.biHeight = -((int)unWidth * scale);
		kInfoHeader.biPlanes = 1;
		kInfoHeader.biBitCount = 24;
		kInfoHeader.biCompression = BI_RGB;
		kInfoHeader.biSizeImage = 0;
		kInfoHeader.biXPelsPerMeter = 0;
		kInfoHeader.biYPelsPerMeter = 0;
		kInfoHeader.biClrUsed = 0;
		kInfoHeader.biClrImportant = 0;


		// Convert QrCode bits to bmp pixels

		pSourceData = pQRC->data;
		for (y = 0; y < unWidth; y++)
		{
			pDestData = pRGBData + unWidthAdjusted * y * scale;
			for (x = 0; x < unWidth; x++)
			{
				if (*pSourceData & 1)
				{
					for (l = 0; l < scale; l++)
					{
						for (n = 0; n < scale; n++)
						{

							*(pDestData + 0 + n * 3 + unWidthAdjusted * l) = r;
							*(pDestData + 1 + n * 3 + unWidthAdjusted * l) = g;
							*(pDestData + 2 + n * 3 + unWidthAdjusted * l) = b;

						}
					}
				}
				pDestData += 3 * scale;
				pSourceData++;
			}
		}


		// Output the bmp file

		if (!(fopen_s(&f, outPath.c_str(), "w")))
		{
			fwrite(&kFileHeader, sizeof(BITMAPFILEHEADER), 1, f);
			fwrite(&kInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);
			fwrite(pRGBData, sizeof(unsigned char), unDataBytes, f);

			fclose(f);
		}
		else
		{
			printf("Unable to open qr ouput file");
		}

		// Free data

		free(pRGBData);
		QRcode_free(pQRC);
	}



}







#endif
