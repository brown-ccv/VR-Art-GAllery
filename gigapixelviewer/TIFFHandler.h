#ifndef TIFFHANDLER_H
#define TIFFHANDLER_H

#include <tiffio.h>
#include <string>
#include "common.h"

namespace GiGaPixelViewer
{
	/**
	 * This class manages a TIFF file handler
	  TO DO: Evaluate if his class needs to be generalized to manage other format files
	 */


	struct SizePerDirectory
	{
		int width;
		int height;
		int bitsPerSample;
		int samplesPerPixel;
	};

	class TIFFHandler
	{
	public:
		TIFFHandler();
		~TIFFHandler();
        int virtual openTIFF(const std::string& file);
		
	
		int width(int directory);
		int height(int directory);
		int bitsPerSample(int directory);
		int samplesPerPixel(int directory);
		int bitsPerPixel();
		int dirCount();
	    
		TIFF* tiff();

	protected:
		TIFF * myTiff;
		int myWidth;
		int myHeight;
		int myBitsPerSample;
		int mySamplesPerPixel;
		int myBitsPerPixel;
		int myDircount;

		std::vector<SizePerDirectory> directorySizes;


	private:

	};
}


#endif