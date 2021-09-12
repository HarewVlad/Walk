struct CubeConstantBuffer {
	matrix mvp;
	matrix model;
};

struct CubeInstanceData {
	float4 offset;
	float3 color;
};

ConstantBuffer<CubeConstantBuffer> C_CB : register(b0);
StructuredBuffer<CubeInstanceData> CID_SB : register(t0);

struct VertexInput {
	float3 position : position;
	float3 normal : normal;
	float2 uv : texcoord;
	uint iid : SV_INSTANCEID;
};

struct VertexOutput {
	float4 position : SV_POSITION;
	float3 normal : normal;
	float3 color : color;
	float2 uv : texcoord;
};

VertexOutput main(VertexInput input) {
	VertexOutput output;

	float4 offset = CID_SB[input.iid].offset;
	float3 color = CID_SB[input.iid].color;

	output.position = mul(C_CB.mvp, float4(input.position + offset.xyz, 1.0f));
	output.uv = input.uv;
	output.color = color;
	output.normal = mul(C_CB.model, float4(input.normal, 1.0f));

	return output;
}