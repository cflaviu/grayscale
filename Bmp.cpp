#include "PCH.h"

bool Header::read( FILE* const item)
{
    fread( &type, 2, 1, item);
    fread( &size, 4, 1, item);
    fseek( item, 4, SEEK_CUR);
    fread( &offset, 4, 1, item);
    return ftell( item) == 14 && type == 19778;
}

BmpType InfoHeader::type() const
{
    BmpType result;
    switch( bitCount)
    {
        case 1: result = bt1bpp; break;
        case 8: result = bt8bpp; break;
        case 16: result = compression == 3 ? bt16bpp_565 : bt16bpp_1555; break;
        case 24: result = bt24bpp_888; break;
        case 32: result = bt32bpp_8888; break;
        default: result = btUnknown; break;

    }

    return result;
}

bool InfoHeader::read( FILE* const item)
{
    fread( &size, 4, 1, item);
    fread( &width, 4, 1, item);
    fread( &height, 4, 1, item);
    fread( &planes, 2, 1, item);
    fread( &bitCount, 2, 1, item);
    fread( &compression, 4, 1, item);
    fread( &imageSize, 4, 1, item);
    fread( &xPixelsPerMeter, 4, 1, item);
    fread( &yPixelsPerMeter, 4, 1, item);
    fread( &colorsUsed, 4, 1, item);
    fread( &colorImportant, 4, 1, item);
    if ( size >= 56)
    {
        fread( &redMask, 4, 1, item);
        fread( &greenMask, 4, 1, item);
        fread( &blueMask, 4, 1, item);
        fread( &alphaMask, 4, 1, item);
    }
    else
    {
        redMask = 0;
        greenMask = 0;
        blueMask = 0;
        alphaMask = 0;
    }

    return ftell( item) >= 40;
}


bool BmpInfo::open( FILE* const item)
{
    iOpen = iHeader.read( item) && iInfoHeader.read( item);
    return iOpen;
}

void BmpInfo::close()
{
    iHeader.clear();
    iInfoHeader.clear();
    iOpen = false;
}

bool readBitmap( void* buffer, FILE* const file, const uint4 offset, const uint4 imageSize)
{
    return !fseek( file, offset, SEEK_SET) && fread( buffer, 1, imageSize, file) == imageSize;
}

bool readBitmap( void* buffer, const char* fileName, const uint4 offset, const uint4 imageSize)
{
    FILE* const file = fopen( fileName, "rb");
    bool result;
    if ( file)
    {
        result = readBitmap( buffer, file, offset, imageSize);
        fclose( file);
    }
    else
    {
        result = false;
    }

    return result;
}
