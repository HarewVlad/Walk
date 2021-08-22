struct Texture {
	ComPtr<ID3D12Resource> texture_resource;
	D3D12_FEATURE_DATA_FORMAT_SUPPORT format_support;
	D3D12_CPU_DESCRIPTOR_HANDLE srv_handle;
	// D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle;
	// D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle;
	// D3D12_CPU_DESCRIPTOR_HANDLE uav_handle;
};

// CreateTextureAsRTV, CreateTextureAsDSV, CreateTextureAsSRV ...