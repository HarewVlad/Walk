SamplerState linear_repeat_sampler : register(s0);
Texture2D cube_texture : register(t1);

struct CubeLight {
  float4 ambient;
  float4 diffuse;
  float3 direction;
};

ConstantBuffer<CubeLight> CL_CB : register(b1);

struct PixelInput {
  float4 position : SV_POSITION;
  float3 normal : normal;
  float3 color : color;
  float2 uv : texcoord;
};

float random(float2 st) {
  return frac(sin(dot(st.xy, float2(12.9898, 78.233))) * 43758.5453123);
}

float4 main(PixelInput input) : SV_TARGET {
  // return cube_texture.Sample(linear_repeat_sampler, input.uv);

  float3 normal = normalize(input.normal);
  float3 color = input.color;
  float4 ambient = CL_CB.ambient;
  float4 diffuse = CL_CB.diffuse;
  float3 direction = CL_CB.direction;

  color *= ambient;
  color += saturate(dot(direction, normal) * diffuse * color);

  return float4(color, 1.0f);
}