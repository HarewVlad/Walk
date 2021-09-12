inline bool CubeCreateIndexBuffer(Cube *cube, Directx *directx) {
  // const WORD cube_indices[36] = {0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6,
  //                                4, 5, 1, 4, 1, 0, 3, 2, 6, 3, 6, 7,
  //                                1, 5, 6, 1, 6, 2, 4, 0, 3, 4, 3, 7};

  const WORD cube_indices[] = {0,  1,  2,  0,  2,  3,  4,  5,  6,  4,  6,  7,
                               8,  9,  10, 8,  10, 11, 12, 13, 14, 12, 14, 15,
                               16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23};
  uint64_t indices_count = _countof(cube_indices);

  ComPtr<ID3D12Resource> index_buffer;
  ComPtr<ID3D12Resource> intermediate_index_buffer;
  DirectxUpdateBufferResource(
      directx, &index_buffer, &intermediate_index_buffer,
      _countof(cube_indices), sizeof(WORD), cube_indices);

  D3D12_INDEX_BUFFER_VIEW index_buffer_view = {};
  index_buffer_view.BufferLocation = index_buffer->GetGPUVirtualAddress();
  index_buffer_view.Format = DXGI_FORMAT_R16_UINT;
  index_buffer_view.SizeInBytes = sizeof(cube_indices);

  cube->indices_count = indices_count;
  cube->index_buffer = index_buffer;
  cube->index_buffer_view = index_buffer_view;
  cube->intermediate_index_buffer = intermediate_index_buffer;

  return true;
}

inline bool CubeCreateVertexBuffer(Cube *cube, Directx *directx) {
  // const CubeVertex cube_vertices[] = {
  //     {XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0, 1)},  // 0
  //     {XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0, 0)},   // 1
  //     {XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1, 0)},    // 2
  //     {XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1, 1)},   // 3
  //     {XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0, 1)},   // 4
  //     {XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0, 0)},    // 5
  //     {XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1, 0)},     // 6
  //     {XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1, 1)}     // 7
  // };
  const CubeVertex cube_vertices[] = {
      {XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f),
       XMFLOAT2(0.0f, 1.0f)},
      {XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f),
       XMFLOAT2(0.0f, 0.0f)},
      {XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f),
       XMFLOAT2(1.0f, 0.0f)},
      {XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f),
       XMFLOAT2(1.0f, 1.0f)},

      {XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
       XMFLOAT2(1.0f, 1.0f)},
      {XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
       XMFLOAT2(0.0f, 1.0f)},
      {XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
       XMFLOAT2(0.0f, 0.0f)},
      {XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
       XMFLOAT2(1.0f, 0.0f)},

      {XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
       XMFLOAT2(0.0f, 1.0f)},
      {XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
       XMFLOAT2(0.0f, 0.0f)},
      {XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
       XMFLOAT2(1.0f, 0.0f)},
      {XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
       XMFLOAT2(1.0f, 1.0f)},

      {XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),
       XMFLOAT2(1.0f, 1.0f)},
      {XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),
       XMFLOAT2(0.0f, 1.0f)},
      {XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),
       XMFLOAT2(0.0f, 0.0f)},
      {XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),
       XMFLOAT2(1.0f, 0.0f)},

      {XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
       XMFLOAT2(0.0f, 1.0f)},
      {XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
       XMFLOAT2(0.0f, 0.0f)},
      {XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
       XMFLOAT2(1.0f, 0.0f)},
      {XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
       XMFLOAT2(1.0f, 1.0f)},

      {XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
       XMFLOAT2(0.0f, 1.0f)},
      {XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
       XMFLOAT2(0.0f, 0.0f)},
      {XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
       XMFLOAT2(1.0f, 0.0f)},
      {XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
       XMFLOAT2(1.0f, 1.0f)},
  };

  ComPtr<ID3D12Resource> vertex_buffer;
  ComPtr<ID3D12Resource> intermediate_vertex_buffer;
  DirectxUpdateBufferResource(
      directx, &vertex_buffer, &intermediate_vertex_buffer,
      _countof(cube_vertices), sizeof(CubeVertex), cube_vertices);

  D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view = {};
  vertex_buffer_view.BufferLocation = vertex_buffer->GetGPUVirtualAddress();
  vertex_buffer_view.SizeInBytes = sizeof(cube_vertices);
  vertex_buffer_view.StrideInBytes = sizeof(CubeVertex);

  cube->vertex_buffer_view = vertex_buffer_view;
  cube->vertex_buffer = vertex_buffer;
  cube->intermediate_vertex_buffer = intermediate_vertex_buffer;

  return true;
}

inline bool CubeCreateStructuredBuffer(Cube *cube, Directx *directx,
                                       const XMFLOAT3 &position, int count) {
  auto device2 = directx->device2;
  auto cbv_srv_uav_descriptor_heap = directx->cbv_srv_uav_descriptor_heap;
  UINT cbv_srv_uav_descriptor_size = directx->cbv_srv_uav_descriptor_size;
  CD3DX12_CPU_DESCRIPTOR_HANDLE srv_descriptor_handle(
      cbv_srv_uav_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), 1,
      cbv_srv_uav_descriptor_size);  // 0 - For texture // TODO: Create
                                     // descriptor allocator
  auto &instances = cube->instances;

  const UINT cube_instances_count = count * count * count;
  const UINT cube_instances_size = cube_instances_count * sizeof(CubeInstance);
  instances.resize(cube_instances_count);
  for (int i = 0; i < count; ++i) {
    for (int j = 0; j < count; ++j) {
      for (int k = 0; k < count; ++k) {
        int index = (i * count + j) * count + k;
        instances[index].offset = {
            i * 10.0f + (1 + rand() % count) - count * 5.0f,
            j * 10.0f + (1 + rand() % count) - count * 5.0f,
            k * 10.0f + (1 + rand() % count) - count * 5.0f, 0};
        float r = rand() % 256;
        float g = rand() % 256;
        float b = rand() % 256;
        instances[index].color = {r * 0.00390625f, g * 0.00390625f,
                                  b * 0.00390625f};
      }
    }
  }

  ComPtr<ID3D12Resource> instance_buffer;
  ComPtr<ID3D12Resource> intermediate_instance_buffer;
  DirectxUpdateBufferResource(
      directx, &instance_buffer, &intermediate_instance_buffer,
      cube_instances_count, sizeof(CubeInstance), &instances[0]);

  D3D12_SHADER_RESOURCE_VIEW_DESC srv_descriptor = {};
  srv_descriptor.Shader4ComponentMapping =
      D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
  srv_descriptor.Format = DXGI_FORMAT_UNKNOWN;
  srv_descriptor.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
  srv_descriptor.Buffer.FirstElement = 0;
  srv_descriptor.Buffer.NumElements = cube_instances_count;
  srv_descriptor.Buffer.StructureByteStride = sizeof(CubeInstance);
  srv_descriptor.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

  device2->CreateShaderResourceView(instance_buffer.Get(), &srv_descriptor,
                                    srv_descriptor_handle);

  cube->instance_buffer = instance_buffer;
  cube->intermediate_instance_buffer = intermediate_instance_buffer;

  return true;
}

inline bool CubeCreateRootSignature(Cube *cube, ComPtr<ID3D12Device2> device2) {
  D3D12_FEATURE_DATA_ROOT_SIGNATURE feature_data = {};
  feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
  if (FAILED(device2->CheckFeatureSupport(
          D3D12_FEATURE_ROOT_SIGNATURE, &feature_data, sizeof(feature_data)))) {
    feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
  }

  CD3DX12_DESCRIPTOR_RANGE1 descriptor_ranges[2];
  descriptor_ranges[0].Init(
      D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0,
      D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);  // Texture
  descriptor_ranges[1].Init(
      D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0,
      D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);  // Structured buffer
  CD3DX12_ROOT_PARAMETER1 root_parameters[3];
  root_parameters[0].InitAsConstants(sizeof(CubeConstantBuffer) * 0.25, 0, 0,
                                     D3D12_SHADER_VISIBILITY_VERTEX);
  root_parameters[1].InitAsConstants(sizeof(CubeLight) * 0.25, 1, 0,
                                     D3D12_SHADER_VISIBILITY_PIXEL);

  root_parameters[2].InitAsDescriptorTable(_countof(descriptor_ranges),
                                           descriptor_ranges,
                                           D3D12_SHADER_VISIBILITY_ALL);

  CD3DX12_STATIC_SAMPLER_DESC linear_repeat_sampler(
      0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);

  D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags =
      D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

  CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_descriptor;
  root_signature_descriptor.Init_1_1(_countof(root_parameters), root_parameters,
                                     1, &linear_repeat_sampler,
                                     root_signature_flags);

  ComPtr<ID3DBlob> root_signature_blob;
  ComPtr<ID3DBlob> error_blob;
  if (D3DX12SerializeVersionedRootSignature(
          &root_signature_descriptor, feature_data.HighestVersion,
          &root_signature_blob, &error_blob)) {
    return false;
  }

  ComPtr<ID3D12RootSignature> root_signature;
  if (device2->CreateRootSignature(0, root_signature_blob->GetBufferPointer(),
                                   root_signature_blob->GetBufferSize(),
                                   IID_PPV_ARGS(&root_signature)) != S_OK) {
    return false;
  }

  cube->root_signature = root_signature;

  return true;
}

inline bool CubeCreatePSO(Cube *cube, ComPtr<ID3D12Device2> device2) {
  auto root_signature = cube->root_signature;

  ComPtr<ID3DBlob> vertex_shader_blob;
  if (D3DCompileFromFile(L"shaders\\vs.hlsl", nullptr, nullptr, "main",
                         "vs_5_1", 0, 0, &vertex_shader_blob,
                         nullptr) != S_OK) {
    return nullptr;
  }

  ComPtr<ID3DBlob> pixel_shader_blob;
  if (D3DCompileFromFile(L"shaders\\ps.hlsl", nullptr, nullptr, "main",
                         "ps_5_1", 0, 0, &pixel_shader_blob, nullptr) != S_OK) {
    return nullptr;
  }

  // Search the best multisample quality
  DXGI_SAMPLE_DESC sample_descriptor =
      DeviceGetMultisampleQualityLevels(device2, DXGI_FORMAT_R8G8B8A8_UNORM);

  D3D12_INPUT_ELEMENT_DESC input_element_descriptor[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
       D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
       0},
      {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
       D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
       0},
      {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
  };

  // Depth stencil state
  CD3DX12_DEPTH_STENCIL_DESC depth_stencil_state(D3D12_DEFAULT);
  depth_stencil_state.DepthEnable = TRUE;

  // Blend state
  CD3DX12_BLEND_DESC blend_state(D3D12_DEFAULT);

  // Rasterizer state
  CD3DX12_RASTERIZER_DESC rasterizer_state(D3D12_DEFAULT);
  rasterizer_state.MultisampleEnable = TRUE;

  D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_state_descriptor = {};
  pipeline_state_descriptor.InputLayout = {input_element_descriptor,
                                           _countof(input_element_descriptor)};
  pipeline_state_descriptor.pRootSignature = root_signature.Get();
  pipeline_state_descriptor.VS =
      CD3DX12_SHADER_BYTECODE(vertex_shader_blob.Get());
  pipeline_state_descriptor.PS =
      CD3DX12_SHADER_BYTECODE(pixel_shader_blob.Get());
  pipeline_state_descriptor.RasterizerState = rasterizer_state;
  pipeline_state_descriptor.BlendState = blend_state;
  pipeline_state_descriptor.DepthStencilState = depth_stencil_state;
  pipeline_state_descriptor.DSVFormat =
      DXGI_FORMAT_D32_FLOAT;  // Error with debug layer if not specified
  pipeline_state_descriptor.SampleMask = UINT_MAX;
  pipeline_state_descriptor.PrimitiveTopologyType =
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
  pipeline_state_descriptor.NumRenderTargets = 1;
  pipeline_state_descriptor.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
  pipeline_state_descriptor.SampleDesc = {1, 0};

  ComPtr<ID3D12PipelineState> pipeline_state;
  if (device2->CreateGraphicsPipelineState(
          &pipeline_state_descriptor, IID_PPV_ARGS(&pipeline_state)) != S_OK) {
    return false;
  }

  cube->pipeline_state = pipeline_state;

  return true;
}

static Cube *CreateCube(Directx *directx, Texture *texture,
                        const XMFLOAT3 &position, int count) {
  Cube *result = new Cube;

  auto device2 = directx->device2;
  auto current_back_buffer_index = directx->current_back_buffer_index;
  auto cbv_srv_uav_descriptor_heap = directx->cbv_srv_uav_descriptor_heap;
  auto command_queue = directx->command_queue;
  auto command_list = directx->command_list;
  auto command_allocator =
      directx->command_allocators[current_back_buffer_index];

  command_allocator->Reset();
  command_list->Reset(command_allocator.Get(), nullptr);

  if (!CubeCreateVertexBuffer(result, directx)) {
    return nullptr;
  }

  if (!CubeCreateIndexBuffer(result, directx)) {
    return nullptr;
  }

  if (!CubeCreateStructuredBuffer(result, directx, position, count)) {
    return nullptr;
  }

  if (!CubeCreateRootSignature(result, device2)) {
    return nullptr;
  }

  if (!CubeCreatePSO(result, device2)) {
    return nullptr;
  }

  command_list->Close();

  ID3D12CommandList *const command_lists[] = {command_list.Get()};
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

  result->texture = texture;
  return result;
}

static bool CubeRender(Cube *cube, Directx *directx, const XMFLOAT3& light_direction, const XMMATRIX &view,
                       const XMMATRIX &projection) {
  auto current_back_buffer_index = directx->current_back_buffer_index;
  auto command_list = directx->command_list;
  auto pipeline_state = cube->pipeline_state;
  auto root_signature = cube->root_signature;
  auto vertex_buffer_view = cube->vertex_buffer_view;
  auto index_buffer_view = cube->index_buffer_view;
  auto indices_count = cube->indices_count;
  auto model = cube->model;
  auto texture = cube->texture;
  auto texture_resouruce = texture->texture_resource;
  auto cbv_srv_uav_descriptor_heap = directx->cbv_srv_uav_descriptor_heap;
  const UINT instances_count = cube->instances.size();

  command_list->SetPipelineState(pipeline_state.Get());
  command_list->SetGraphicsRootSignature(root_signature.Get());

  ID3D12DescriptorHeap *descriptor_heaps[] = {
      cbv_srv_uav_descriptor_heap.Get()};
  command_list->SetDescriptorHeaps(_countof(descriptor_heaps),
                                   descriptor_heaps);

  XMMATRIX mvp_matrix = XMMatrixMultiply(model, view);
  mvp_matrix = XMMatrixMultiply(mvp_matrix, projection);
  CubeConstantBuffer cube_constant_buffer = {mvp_matrix, model};

  XMFLOAT4 ambient = {0.4f, 0.4f, 0.4f, 1.0f};
  XMFLOAT4 diffuse = {1.0f, 1.0f, 1.0f, 1.0f};
  CubeLight cube_light = {ambient, diffuse, light_direction};

  // Faster access since no indirection
  command_list->SetGraphicsRoot32BitConstants(0, sizeof(CubeConstantBuffer) * 0.25f,
                                              &cube_constant_buffer, 0);
  command_list->SetGraphicsRoot32BitConstants(1, sizeof(CubeLight) * 0.25f,
                                              &cube_light, 0);
  // Descriptor tables have 2 indirections before access data
  command_list->SetGraphicsRootDescriptorTable(
      2, cbv_srv_uav_descriptor_heap->GetGPUDescriptorHandleForHeapStart());

  command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);
  command_list->IASetIndexBuffer(&index_buffer_view);

  command_list->DrawIndexedInstanced(indices_count, instances_count, 0, 0, 0);

  return true;
}

static bool CubeUpdate(Cube *cube, float dt) {
  auto &model = cube->model;

  // Rotation
  // static float angle = 0;
  // angle += dt;
  // const XMVECTOR rotation_axis = XMVectorSet(0, 0, 1, 0);
  // model = XMMatrixRotationAxis(rotation_axis, XMConvertToRadians(angle));
  model = XMMatrixIdentity();

  return true;
}