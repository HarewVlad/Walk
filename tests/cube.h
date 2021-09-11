struct CubeVertex {
  XMFLOAT3 position;
  XMFLOAT2 uv;
};

struct CubeInstance {
  XMFLOAT4 offset;
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
  std::vector<CubeInstance> instances;
  ComPtr<ID3D12Resource> instance_buffer;
  ComPtr<ID3D12Resource> intermediate_instance_buffer;
};