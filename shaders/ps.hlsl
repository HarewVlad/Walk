SamplerState linear_repeat_sampler : register(s0);
Texture2D cube_texture : register(t1);

struct PixelInput {
	float4 position : sv_position;
	float2 uv : texcoord;	
};

float4 main(PixelInput input) : sv_target {
	return cube_texture.Sample(linear_repeat_sampler, input.uv);
}