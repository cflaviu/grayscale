__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE;

__kernel void grayscalex( __global read_only float* coefs, read_only image2d_t in, write_only image2d_t out)
{
	const int2 pos = (int2)( get_global_id( 0), get_global_id( 1));
	uint4 pixel = read_imageui( in, sampler, pos);
	pixel.yzw = (uint) round( coefs[ 0] * pixel.x + coefs[ 1] * pixel.y + coefs[ 2] * pixel.z);
	write_imageui( out, pos, pixel);
}

//__kernel void colorTwist( __global read_only float* m, read_only image2d_t in, write_only image2d_t out)
__kernel void grayscale( __global read_only float* m, read_only image2d_t in, write_only image2d_t out)
{
	const int2 pos = (int2)( get_global_id( 0), get_global_id( 1));
	const uint4 pixel = read_imageui( in, sampler, pos);
	uint4 outPixel;
	outPixel.x = 0;
	outPixel.y = (uint) round( pixel.y * m[ 0] + pixel.z * m[ 1] + pixel.w * m[ 2]); // R
	outPixel.z = (uint) round( pixel.y * m[ 3] + pixel.z * m[ 4] + pixel.w * m[ 5]); // G
	outPixel.w = (uint) round( pixel.y * m[ 6] + pixel.z * m[ 7] + pixel.w * m[ 8]); // B
	write_imageui( out, pos, outPixel);
}

__kernel void grayscale1( __global read_only float* coefs, read_only image2d_t in, write_only image2d_t out)
{
	const int2 pos = (int2)( get_global_id( 0), get_global_id( 1));
	uint4 pixel = read_imageui( in, sampler, pos);
	pixel.yzw = (uint) round( coefs[ 0] * pixel.x + coefs[ 1] * pixel.y + coefs[ 2] * pixel.z);
	write_imageui( out, pos, pixel);
}

__kernel void tuneRgbChannels( __global read_only float* coefs, read_only image2d_t in, write_only image2d_t out)
{
	const int2 pos = (int2)( get_global_id( 0), get_global_id( 1));
	uint4 pixel = read_imageui( in, sampler, pos);
	pixel.y = (uint) round( coefs[ 0] * pixel.y); // B
	pixel.z = (uint) round( coefs[ 1] * pixel.z); // G
	pixel.w = (uint) round( coefs[ 2] * pixel.w); // R
	write_imageui( out, pos, pixel);
}

__kernel void negative( read_only image2d_t in, write_only image2d_t out)
{
	const int2 pos = (int2)( get_global_id( 0), get_global_id( 1));
	const uint4 originalPixel = read_imageui( in, sampler, pos);
	uint4 pixel = 255 - originalPixel;
	pixel.x = originalPixel.x;	
	write_imageui( out, pos, pixel);
}

__kernel void horizontalFlip( read_only image2d_t in, write_only image2d_t out)
{
	const int x = get_global_id( 0);
	const int y = get_global_id( 1);
	const int width = get_image_width( in);
	write_imageui( out, (int2)( x, y), read_imageui( in, sampler, (int2)( width - 1 - x, y)));
}

__kernel void verticalFlip( read_only image2d_t in, write_only image2d_t out)
{
	const int x = get_global_id( 0);
	const int y = get_global_id( 1);
	const int height = get_image_height( in);
	write_imageui(out, (int2)( x, y), read_imageui( in, sampler, (int2)( x, height - 1 - y)));
}

__kernel void horizontalAndVerticalFlip( read_only image2d_t in, write_only image2d_t out)
{
	const int x = get_global_id( 0);
	const int y = get_global_id( 1);
	const int width = get_image_width( in);
	const int height = get_image_height( in);
	write_imageui( out, (int2)( x, y), read_imageui( in, sampler, (int2)( width - 1 - x, height - 1 - y)));
}
