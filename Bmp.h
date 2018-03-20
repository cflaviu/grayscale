#pragma once

enum BmpType
{
    btUnknown,
    bt1bpp,
    bt8bpp,
    bt16bpp_565,
    bt16bpp_1555,
    bt24bpp_888,
    bt32bpp_8888,
};

struct Header
{
    uint2 type;
    uint4 size;
    uint2 reserved1;
    uint2 reserved2;
    uint4 offset;

    Header() { clear(); }
    bool read( FILE* const item);
    void clear() { memset( this, 0, sizeof(Header)); }
};

struct InfoHeader
{
    uint4   size;
    int4    width;
    int4    height;
    uint2   planes;
    uint2   bitCount;
    uint4   compression;
    uint4   imageSize;
    int4    xPixelsPerMeter;
    int4    yPixelsPerMeter;
    uint4   colorsUsed;
    uint4   colorImportant;
    uint4   redMask;
    uint4   greenMask;
    uint4   blueMask;
    uint4   alphaMask;

    InfoHeader() { clear(); }
    BmpType type() const;
    bool read( FILE* const item);
    void clear() { memset( this, 0, sizeof(InfoHeader)); }
};

class BmpInfo
{
public:
    BmpInfo(): iOpen( false) {}

    bool    isOpen()            const { return iOpen; }
    BmpType type()              const { return iInfoHeader.type(); }
    uint4   width()             const { return iInfoHeader.width; }
    uint4   height()            const { return iInfoHeader.height; }
    uint4   imageSize()         const { return iInfoHeader.imageSize; }
    uint2   depth()             const { return iInfoHeader.bitCount; }
    int4    xPixelsPerMeter()   const { return iInfoHeader.xPixelsPerMeter; }
    int4    yPixelsPerMeter()   const { return iInfoHeader.yPixelsPerMeter; }
    uint4   maskRed()           const { return iInfoHeader.redMask; }
    uint4   maskGreen()         const { return iInfoHeader.greenMask; }
    uint4   maskBlue()          const { return iInfoHeader.blueMask; }
    uint4   maskAlpha()         const { return iInfoHeader.alphaMask; }
    uint4   offset()            const { return iHeader.offset; }
    uint4   fileSize()          const { return iHeader.size; }

    bool open( FILE* const item);
    void close();

	const Header& header() const { return iHeader; }
	const InfoHeader& infoHeader() const { return iInfoHeader; }

private:
    Header      iHeader;
    InfoHeader  iInfoHeader;
    bool        iOpen;
};

bool readBitmap( void* buffer, FILE* const file, const uint4 offset, const uint4 imageSize);
bool readBitmap( void* buffer, const char* fileName, const uint4 offset, const uint4 imageSize);