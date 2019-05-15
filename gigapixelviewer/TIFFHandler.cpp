#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  

#include "TIFFHandler.h"
#include <assert.h>


namespace GiGaPixelViewer
{
	TIFFHandler::TIFFHandler()
		:myWidth(0)
		, myHeight(0)
		, myBitsPerSample(0)
		, mySamplesPerPixel(0)
		, myDircount(0)
	{
	}


	TIFFHandler::~TIFFHandler()
	{
		//TIFFClose(myTiff);
	}

	int TIFFHandler::openTIFF(const std::string& file)
	{
		std::wstring str2(file.length(), L' '); // Make room for characters
		std::copy(file.begin(), file.end(), str2.begin());
		myTiff = TIFFOpen(file.c_str(), "r");
		if (myTiff == NULL) {
			fprintf(stderr, "Problem showing %s\n", file.c_str());
			assert(false);
		}

		/*if (!TIFFIsTiled(_tif)) {
			fprintf(stderr, "Not a tiled tiff!!\n");
			TIFFClose(_tif);
			return 1;
		}*/

		

		do {
			myDircount++;
		} while (TIFFReadDirectory(myTiff));
		

		directorySizes = std::vector<SizePerDirectory>(myDircount, SizePerDirectory());


		for (int i = myDircount - 1 ; i >= 0 ; --i)
		{
			TIFFSetDirectory(myTiff, i);

			SizePerDirectory& directorySize = directorySizes[i];

			TIFFGetField(myTiff, TIFFTAG_IMAGEWIDTH, &directorySize.width);
			TIFFGetField(myTiff, TIFFTAG_IMAGELENGTH, &directorySize.height);
			TIFFGetField(myTiff, TIFFTAG_BITSPERSAMPLE, &directorySize.bitsPerSample);
			TIFFGetField(myTiff, TIFFTAG_SAMPLESPERPIXEL, &directorySize.samplesPerPixel);
			 
		}


	
		
		return true;
	}



	int TIFFHandler::width(int directory)
	{
		return directorySizes[directory].width;
	}

	int TIFFHandler::height(int directory)
	{
		return  directorySizes[directory].height;
	}

	int TIFFHandler::bitsPerSample(int directory)
	{
		return directorySizes[directory].bitsPerSample;
	}

	int TIFFHandler::samplesPerPixel(int directory)
	{
		return directorySizes[directory].samplesPerPixel;
	}

	int TIFFHandler::bitsPerPixel()
	{
		return myBitsPerPixel;
	}

	int TIFFHandler::dirCount()
	{
		return myDircount;
	}

	TIFF* TIFFHandler::tiff()
	{
		return myTiff;
	}

}

