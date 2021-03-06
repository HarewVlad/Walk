inline bool CheckFormatSupport(D3D12_FEATURE_DATA_FORMAT_SUPPORT format_support,
                               D3D12_FORMAT_SUPPORT1 support1) {
  return (format_support.Support1 & support1) != 0;
}

inline bool CheckUAVSupport(D3D12_FEATURE_DATA_FORMAT_SUPPORT format_support) {
  return (format_support.Support1 &
          D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW != 0) &&
         (format_support.Support1 &
          D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD != 0) &&
         (format_support.Support1 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE != 0);
}

static Texture* CreateTexture(const std::wstring& filename, Directx* directx) {
  // TODO: Create descriptor allocator
  auto device2 = directx->device2;
  auto current_back_buffer_index = directx->current_back_buffer_index;
  auto cbv_srv_uav_descriptor_heap = directx->cbv_srv_uav_descriptor_heap;
  D3D12_CPU_DESCRIPTOR_HANDLE srv_handle =
      cbv_srv_uav_descriptor_heap->GetCPUDescriptorHandleForHeapStart(); // Now taking 1 descriptor for texture
  auto command_list = directx->command_list;
  auto command_allocator =
      directx->command_allocators[current_back_buffer_index];
  auto command_queue = directx->command_queue;

  Texture* texture = new Texture;

  TexMetadata texture_metadata;
  ScratchImage scratch_image;

  std::wstring extension = filename.substr(filename.length() - 3);
  if (extension == L"jpg") {
    if (LoadFromWICFile(filename.c_str(), WIC_FLAGS_NONE, &texture_metadata,
                        scratch_image)) {
      return nullptr;
    }
  } else if (extension == L"hdr") {
    if (LoadFromHDRFile(filename.c_str(), &texture_metadata, scratch_image) !=
        S_OK) {
      return nullptr;
    }
  }

  D3D12_RESOURCE_DESC texture_descriptor = {};
  switch (texture_metadata.dimension) {
    case TEX_DIMENSION_TEXTURE1D:
      break;
    case TEX_DIMENSION_TEXTURE2D:
      texture_descriptor = CD3DX12_RESOURCE_DESC::Tex2D(
          texture_metadata.format, static_cast<UINT64>(texture_metadata.width),
          static_cast<UINT>(texture_metadata.height),
          static_cast<UINT16>(texture_metadata.arraySize));
      break;
    case TEX_DIMENSION_TEXTURE3D:
      break;
  }

  ComPtr<ID3D12Resource> texture_resource;
  if (device2->CreateCommittedResource(
          &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
          D3D12_HEAP_FLAG_NONE, &texture_descriptor,
          D3D12_RESOURCE_STATE_COMMON, nullptr,
          IID_PPV_ARGS(&texture_resource)) != S_OK) {
    return nullptr;
  }

  std::vector<D3D12_SUBRESOURCE_DATA> texture_subresources(
      scratch_image.GetImageCount());
  const Image* images = scratch_image.GetImages();
  for (int i = 0; i < scratch_image.GetImageCount(); ++i) {
    auto& texture_subresource = texture_subresources[i];
    texture_subresource.RowPitch = images[i].rowPitch;
    texture_subresource.SlicePitch = images[i].slicePitch;
    texture_subresource.pData = images[i].pixels;
  }

  // Upload texture
  const UINT64 required_size = GetRequiredIntermediateSize(
      texture_resource.Get(), 0,
      static_cast<uint32_t>(texture_subresources.size()));

  ComPtr<ID3D12Resource> texture_upload;
  if (device2->CreateCommittedResource(
          &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
          D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(required_size),
          D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
          IID_PPV_ARGS(&texture_upload))) {
    return nullptr;
  }

  command_allocator->Reset();
  command_list->Reset(command_allocator.Get(), nullptr);

  UpdateSubresources(command_list.Get(), texture_resource.Get(),
                     texture_upload.Get(), 0, 0,
                     static_cast<uint32_t>(texture_subresources.size()),
                     texture_subresources.data());

  CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
      texture_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
      D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
  command_list->ResourceBarrier(1, &barrier);

  // Execute copy command list
  command_list->Close();

  ID3D12CommandList* const command_lists[] = {command_list.Get()};
  command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

  // DirectxWaitForGPU(directx);

  ComPtr<ID3D12Fence> fence;
  if (device2->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)) !=
      S_OK) {
    return nullptr;
  }

  HANDLE fence_event = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (!fence_event) {
    return nullptr;
  }

  uint64_t fence_value = 0;
  fence_value = Signal(command_queue, fence, fence_value);

  WaitForFenceValue(fence, fence_value, fence_event);

  // Create SRV
  D3D12_SHADER_RESOURCE_VIEW_DESC srv_descriptor = {};
  srv_descriptor.Shader4ComponentMapping =
      D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
  srv_descriptor.Format = texture_descriptor.Format;
  srv_descriptor.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
  srv_descriptor.Texture2D.MipLevels = 1;
  device2->CreateShaderResourceView(texture_resource.Get(), &srv_descriptor,
                                    srv_handle);

  texture->texture_resource = texture_resource;

  return texture;
}