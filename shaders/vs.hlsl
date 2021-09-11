struct ModelViewProjection {
	matrix MVP;
};

struct InstanceData {
	float4 offset;
};

ConstantBuffer<ModelViewProjection> MVP_CB : register(b0);
StructuredBuffer<InstanceData> ID_SB : register(t0);

struct VertexInput {
	float3 position : position;
	float2 uv : texcoord;
	uint iid : SV_INSTANCEID;
};

struct VertexOutput {
	float4 position : SV_POSITION;
	float2 uv : texcoord;
};

VertexOutput main(VertexInput input) {
	VertexOutput output;

	output.position = mul(MVP_CB.MVP, float4(input.position + ID_SB[input.iid].offset.xyz, 1.0f));
	output.uv = input.uv;

	return output;
}