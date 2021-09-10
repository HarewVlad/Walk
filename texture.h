struct Texture {
	ComPtr<ID3D12Resource> texture_resource;
	D3D12_FEATURE_DATA_FORMAT_SUPPORT format_support;
};

// CreateTextureAsRTV, CreateTextureAsDSV, CreateTextureAsSRV ...