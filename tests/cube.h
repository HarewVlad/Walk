struct CubeVertex {
	XMFLOAT3 position;
	XMFLOAT2 uv;
};

struct CubeConstantBuffer {
	XMFLOAT4 offsets[1000];
	float padding[32]; // 256 byte alignment
};

struct Cube {
	ComPtr<ID3D12Resource> intermediate_vertex_buffer;
	ComPtr<ID3D12Resource> vertex_buffer;
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;
	ComPtr<ID3D12Resource> intermediate_index_buffer;
	ComPtr<ID3D12Resource> index_buffer;
	uint64_t indices_count;
	D3D12_INDEX_BUFFER_VIEW index_buffer_view;
	ComPtr<ID3D12RootSignature> root_signature;
	ComPtr<ID3D12PipelineState> pipeline_state;
	Texture *texture;
	XMMATRIX model;
	CubeConstantBuffer *constant_buffer_data;
	ComPtr<ID3D12Resource> constant_buffer;
	void *constant_buffer_address;
};