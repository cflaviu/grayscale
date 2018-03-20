__kernel void grayscale( read_only image2d_t in, write_only image2d_t out)
{
	int2 coordinate = (int2)( get_global_id( 0), get_global_id( 1));
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_CLAMP_TO_EDGE;
	uint4 pixel = read_imageui( in, sampler, coordinate);
	float y = 0.3f * (float)pixel.x + 0.59f * (float)pixel.y + 0.11f * (float)pixel.z;
	pixel.w = (uint)y;
	write_imageui( out, coordinate, pixel);
}
