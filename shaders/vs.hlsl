struct ModelViewProjection {
	matrix MVP;
};

struct InstanceData {
	float4 offset[1000];
	float4 padding[2];
};

ConstantBuffer<ModelViewProjection> MVP_CB : register(b0);
ConstantBuffer<InstanceData> ID_CB : register(b1);

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

	float3 new_position = {input.position.x + ID_CB.offset[input.iid].x, input.position.y + ID_CB.offset[input.iid].y, input.position.z + ID_CB.offset[input.iid].z};
	output.position = mul(MVP_CB.MVP, float4(new_position, 1.0f));
	output.uv = input.uv;

	return output;
}