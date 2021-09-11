inline bool IsTearingSupported() {
  bool result = false;

  ComPtr<IDXGIFactory4> factory4;
  if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4)))) {
    ComPtr<IDXGIFactory5> factory5;
    if (SUCCEEDED(factory4.As(&factory5))) {
      factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &result,
                                    sizeof(BOOL));
    }
  }

  return result;
}

static bool DirectxMoveToNextFrame(Directx *directx) {
  auto &current_back_buffer_index = directx->current_back_buffer_index;
  auto &fence_value = directx->fence_values[current_back_buffer_index];
  auto command_queue = directx->command_queue;
  auto fence = directx->fence;
  auto fence_event = directx->fence_event;
  auto swap_chain4 = directx->swap_chain4;

  const uint64_t current_fence_value = fence_value;
  if (command_queue->Signal(fence.Get(), current_fence_value) != S_OK) {
    return false;
  }

  current_back_buffer_index = swap_chain4->GetCurrentBackBufferIndex();

  if (fence->GetCompletedValue() < fence_value) {
    if (fence->SetEventOnCompletion(fence_value, fence_event) != S_OK) {
      return false;
    }
    WaitForSingleObjectEx(fence_event, INFINITE, FALSE);
  }

  fence_value = current_fence_value + 1;

  return true;
}

static DXGI_SAMPLE_DESC DeviceGetMultisampleQualityLevels(
    ComPtr<ID3D12Device2> device2, DXGI_FORMAT format) {
  DXGI_SAMPLE_DESC result = {1, 0};

  D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS quality_levels = {};
  quality_levels.Format = format;
  quality_levels.SampleCount = 1;
  quality_levels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
  quality_levels.NumQualityLevels = 0;

  while (quality_levels.SampleCount < D3D12_MAX_MULTISAMPLE_SAMPLE_COUNT &&
         SUCCEEDED(device2->CheckFeatureSupport(
             D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &quality_levels,
             sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS))) &&
         quality_levels.NumQualityLevels > 0) {
    result.Count = quality_levels.SampleCount;
    result.Quality = quality_levels.NumQualityLevels - 1;

    quality_levels.SampleCount *= 2;
  }

  return result;
}

static ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
    ComPtr<ID3D12Device2> device2, D3D12_DESCRIPTOR_HEAP_TYPE type,
    uint32_t descriptors_count) {
  ComPtr<ID3D12DescriptorHeap> descriptor_heap;

  D3D12_DESCRIPTOR_HEAP_DESC dhd = {};
  dhd.NumDescriptors = descriptors_count;
  dhd.Type = type;

  if (device2->CreateDescriptorHeap(&dhd, IID_PPV_ARGS(&descriptor_heap)) !=
      S_OK) {
    return nullptr;
  }

  return descriptor_heap;
}

inline uint64_t Signal(ComPtr<ID3D12CommandQueue> command_queue,
                       ComPtr<ID3D12Fence> fence, uint64_t &fence_value) {
  uint64_t new_fence_value = ++fence_value;
  if (command_queue->Signal(fence.Get(), new_fence_value) != S_OK) {
    return -1;
  }

  return new_fence_value;
}

inline bool WaitForFenceValue(
    ComPtr<ID3D12Fence> fence, uint64_t fence_value, HANDLE fence_event,
    std::chrono::milliseconds duration = std::chrono::milliseconds::max()) {
  if (fence->GetCompletedValue() < fence_value) {
    if (fence->SetEventOnCompletion(fence_value, fence_event) != S_OK) {
      return false;
    }
    ::WaitForSingleObject(fence_event, static_cast<DWORD>(duration.count()));
  }

  return true;
}

static void DirectxUpdate(Directx *directx) {
  static uint64_t frame_counter = 0;
  static double elapsed_seconds = 0.0;
  static std::chrono::high_resolution_clock clock;
  static auto t0 = clock.now();

  frame_counter++;
  auto t1 = clock.now();
  auto dt = t1 - t0;
  t0 = t1;

  elapsed_seconds += dt.count() * 1e-9;
  if (elapsed_seconds > 1.0) {
    char buffer[256];
    auto fps = frame_counter / elapsed_seconds;
    sprintf_s(buffer, 256, "FPS: %f\n", fps);
    OutputDebugString(buffer);

    frame_counter = 0;
    elapsed_seconds = 0.0;
  }
}

static bool DirectxRenderBegin(
    Directx *directx, ComPtr<ID3D12PipelineState> pipeline_state = nullptr) {
  auto &current_back_buffer_index = directx->current_back_buffer_index;
  auto command_allocator =
      directx->command_allocators[current_back_buffer_index];
  auto command_list = directx->command_list;
  auto back_buffer = directx->back_buffers[current_back_buffer_index];
  auto rtv_descriptor_heap = directx->rtv_descriptor_heap;
  auto rtv_descriptor_size = directx->rtv_descriptor_size;
  auto dsv_descriptor_heap = directx->dsv_descriptor_heap;
  auto dsv_descriptor_handle =
      dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
  auto viewport = directx->viewport;
  auto scissor_rect = directx->scissor_rect;
  CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_descriptor_handle(
      rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart(),
      current_back_buffer_index, rtv_descriptor_size);

  command_allocator->Reset();
  command_list->Reset(command_allocator.Get(), pipeline_state.Get());

  // Clear
  CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
      back_buffer.Get(), D3D12_RESOURCE_STATE_PRESENT,
      D3D12_RESOURCE_STATE_RENDER_TARGET);

  command_list->ResourceBarrier(1, &barrier);

  FLOAT clear_color[] = {0.4f, 0.6f, 0.9f, 1.0f};
  command_list->ClearRenderTargetView(rtv_descriptor_handle, clear_color, 0,
                                      nullptr);
  command_list->ClearDepthStencilView(
      dsv_descriptor_handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

  // Populate command list
  command_list->RSSetViewports(1, &viewport);
  command_list->RSSetScissorRects(1, &scissor_rect);

  command_list->OMSetRenderTargets(1, &rtv_descriptor_handle, FALSE,
                                   &dsv_descriptor_handle);

  return true;
}

static bool DirectxRenderEnd(Directx *directx) {
  auto &current_back_buffer_index = directx->current_back_buffer_index;
  auto command_list = directx->command_list;
  auto back_buffer = directx->back_buffers[current_back_buffer_index];
  auto command_queue = directx->command_queue;
  auto swap_chain4 = directx->swap_chain4;
  auto vsync = directx->vsync;
  auto fence = directx->fence;
  auto &fence_values = directx->fence_values;
  auto &fence_value = directx->fence_value;
  auto fence_event = directx->fence_event;

  // Present
  CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
      back_buffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
      D3D12_RESOURCE_STATE_PRESENT);
  command_list->ResourceBarrier(1, &barrier);

  command_list->Close();

  ID3D12CommandList *const command_lists[] = {command_list.Get()};
  command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

  UINT sync_interval = 0;  // NO VSync
  UINT present_flags =
      IsTearingSupported() && !vsync ? DXGI_PRESENT_ALLOW_TEARING : 0;
  if (swap_chain4->Present(sync_interval, present_flags) != S_OK) {
    return false;
  }

  if (!DirectxMoveToNextFrame(directx)) {
    return false;
  }

  return true;
}

inline void Flush(ComPtr<ID3D12CommandQueue> command_queue,
                  ComPtr<ID3D12Fence> fence, uint64_t &fence_value,
                  HANDLE fence_event) {
  uint64_t new_fence_value = Signal(command_queue, fence, fence_value);
  WaitForFenceValue(fence, new_fence_value, fence_event);
}

// NOTE: We don't do resize
// static bool DirectxResize(Directx *directx, Window *window, UINT width, UINT
// height) {
//   if (window->width != width || window->height != height) {
//     window->width = width;
//     window->height = height;

//     auto command_queue = directx->command_queue;
//     auto fence = directx->fence;
//     auto fence_event = directx->fence_event;
//     auto &fence_value = directx->fence_value;

//     Flush(command_queue, fence, fence_value, fence_event);

//     auto current_back_buffer_index = directx->current_back_buffer_index;
//     for (int i = 0; i < Directx::FRAMES_COUNT; ++i) {
//       directx->back_buffers[i].Reset();
//       directx->fence_values[i] =
//       directx->fence_values[current_back_buffer_index];
//     }

//     auto swap_chain4 = directx->swap_chain4;
//     DXGI_SWAP_CHAIN_DESC scd = {};
//     if (swap_chain4->GetDesc(&scd) != S_OK) {
//       return false;
//     }

//     if (swap_chain4->ResizeBuffers(Directx::FRAMES_COUNT, width, height,
//     scd.BufferDesc.Format, scd.Flags) != S_OK) {
//       return false;
//     }

//     auto device = directx->device2;
//     auto rtv_descriptor_heap = directx->rtv_descriptor_heap;
//     auto back_buffer = directx->back_buffers;
//     UpdateRenderTargetViews(device, swap_chain4, back_buffers,
//     rtv_descriptor_heap);
//   }
// }

static Directx *CreateDirectx(Window *window) {
  Directx *result = new Directx;

#ifdef _DEBUG
  ComPtr<ID3D12Debug> debug_interface;
  D3D12GetDebugInterface(IID_PPV_ARGS(&debug_interface));
  debug_interface->EnableDebugLayer();
#endif

  // Create factory
  ComPtr<IDXGIFactory4> dxgi_factory;
  UINT factory_creation_falgs = 0;
#ifdef _DEBUG
  factory_creation_falgs = DXGI_CREATE_FACTORY_DEBUG;
#endif

  if (CreateDXGIFactory2(factory_creation_falgs, IID_PPV_ARGS(&dxgi_factory)) !=
      S_OK) {
    return nullptr;
  }

  // Query adapter
  ComPtr<IDXGIAdapter1> dxgi_adapter1;
  ComPtr<IDXGIAdapter4> dxgi_adapter4;
  size_t dedicated_video_memory_max = 0;
  for (UINT i = 0;
       dxgi_factory->EnumAdapters1(i, &dxgi_adapter1) != DXGI_ERROR_NOT_FOUND;
       ++i) {
    DXGI_ADAPTER_DESC1 dxgi_adapter_desc1;
    dxgi_adapter1->GetDesc1(&dxgi_adapter_desc1);

    if ((dxgi_adapter_desc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
        D3D12CreateDevice(dxgi_adapter1.Get(), D3D_FEATURE_LEVEL_11_0,
                          __uuidof(ID3D12Device), nullptr) &&
        dxgi_adapter_desc1.DedicatedVideoMemory > dedicated_video_memory_max) {
      dedicated_video_memory_max = dxgi_adapter_desc1.DedicatedVideoMemory;
      if (dxgi_adapter1.As(&dxgi_adapter4) != S_OK) {
        return nullptr;
      }
    }
  }

  // Create device
  ComPtr<ID3D12Device2> device2;
  if (D3D12CreateDevice(dxgi_adapter4.Get(), D3D_FEATURE_LEVEL_11_0,
                        IID_PPV_ARGS(&device2)) != S_OK) {
    return nullptr;
  }

  // Info queue
#ifdef _DEBUG
  ComPtr<ID3D12InfoQueue> info_queue;
  if (device2.As(&info_queue) == S_OK) {
    info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
    info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
    info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

    D3D12_MESSAGE_SEVERITY severities[] = {D3D12_MESSAGE_SEVERITY_INFO};

    D3D12_MESSAGE_ID ignore_ids[] = {
        D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
        D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
        D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
    };

    D3D12_INFO_QUEUE_FILTER filter = {};
    filter.DenyList.NumSeverities = _countof(severities);
    filter.DenyList.pSeverityList = severities;
    filter.DenyList.NumIDs = _countof(ignore_ids);
    filter.DenyList.pIDList = ignore_ids;

    if (info_queue->PushStorageFilter(&filter) != S_OK) {
      return nullptr;
    }
  }
#endif

  // Create command queue
  ComPtr<ID3D12CommandQueue> command_queue;

  D3D12_COMMAND_QUEUE_DESC qqd = {};
  qqd.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
  qqd.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
  qqd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

  if (device2->CreateCommandQueue(&qqd, IID_PPV_ARGS(&command_queue)) != S_OK) {
    return nullptr;
  }

  // Create swap chain
  ComPtr<IDXGISwapChain4> swap_chain4;
  ComPtr<IDXGIFactory4> factory4;
  UINT create_factory_flags = 0;

#ifdef _DEBUG
  create_factory_flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

  if (CreateDXGIFactory2(create_factory_flags, IID_PPV_ARGS(&factory4)) !=
      S_OK) {
    return nullptr;
  }

  UINT width = window->width;
  UINT height = window->height;
  HWND hwnd = window->hwnd;

  DXGI_SWAP_CHAIN_DESC1 scd = {};
  scd.Width = width;
  scd.Height = height;
  scd.BufferCount = FRAME_COUNT;
  scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  scd.Flags = IsTearingSupported() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
  scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  scd.SampleDesc = {1, 0};
  scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  scd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
  scd.Scaling = DXGI_SCALING_STRETCH;
  scd.Stereo = FALSE;

  ComPtr<IDXGISwapChain1> swap_chain1;
  if (factory4->CreateSwapChainForHwnd(command_queue.Get(), hwnd, &scd, nullptr,
                                       nullptr, &swap_chain1) != S_OK) {
    return nullptr;
  }

  // Manually handle toggle fullscreen
  if (factory4->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER) != S_OK) {
    return nullptr;
  }

  if (swap_chain1.As(&swap_chain4) != S_OK) {
    return nullptr;
  }

  // Get current back buffer index
  UINT current_back_buffer_index = swap_chain4->GetCurrentBackBufferIndex();

  // Create rtv descriptor heap
  ComPtr<ID3D12DescriptorHeap> rtv_descriptor_heap = CreateDescriptorHeap(
      device2, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, FRAME_COUNT);
  UINT rtv_descriptor_size =
      device2->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

  // Create render target views
  CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(
      rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart());

  for (int i = 0; i < FRAME_COUNT; ++i) {
    ComPtr<ID3D12Resource> back_buffer;
    if (swap_chain4->GetBuffer(i, IID_PPV_ARGS(&back_buffer)) != S_OK) {
      return false;
    }

    device2->CreateRenderTargetView(back_buffer.Get(), nullptr, rtv_handle);

    result->back_buffers[i] = back_buffer;

    rtv_handle.Offset(rtv_descriptor_size);
  }

  // Create command allocators
  for (int i = 0; i < FRAME_COUNT; ++i) {
    ComPtr<ID3D12CommandAllocator> command_allocator;

    if (device2->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                        IID_PPV_ARGS(&command_allocator)) !=
        S_OK) {
      return nullptr;
    }

    result->command_allocators[i] = command_allocator;
  }

  // Create command list
  ComPtr<ID3D12GraphicsCommandList> command_list;
  if (device2->CreateCommandList(
          0, D3D12_COMMAND_LIST_TYPE_DIRECT,
          result->command_allocators[current_back_buffer_index].Get(), nullptr,
          IID_PPV_ARGS(&command_list)) != S_OK) {
    return nullptr;
  }

  if (command_list->Close() != S_OK) {
    return nullptr;
  }

  // Create fence
  ComPtr<ID3D12Fence> fence;
  if (device2->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)) !=
      S_OK) {
    return nullptr;
  }

  // Create fence event
  HANDLE fence_event = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (!fence_event) {
    return nullptr;
  }

  // Scissor rect
  D3D12_RECT scissor_rect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);

  // Viewport
  D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT(
      0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));

  // Depth stencil view descriptor heap
  ComPtr<ID3D12DescriptorHeap> dsv_descriptor_heap;
  D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_descriptor = {};
  dsv_heap_descriptor.NumDescriptors = 1;
  dsv_heap_descriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
  dsv_heap_descriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  if (device2->CreateDescriptorHeap(
          &dsv_heap_descriptor, IID_PPV_ARGS(&dsv_descriptor_heap)) != S_OK) {
    return nullptr;
  }

  // Depth stencil view
  D3D12_CLEAR_VALUE clear_value = {};
  clear_value.Format = DXGI_FORMAT_D32_FLOAT;
  clear_value.DepthStencil = {1, 0};

  ComPtr<ID3D12Resource> depth_buffer;
  if (device2->CreateCommittedResource(
          &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
          D3D12_HEAP_FLAG_NONE,
          &CD3DX12_RESOURCE_DESC::Tex2D(
              DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0,
              D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
          D3D12_RESOURCE_STATE_DEPTH_WRITE, &clear_value,
          IID_PPV_ARGS(&depth_buffer)) != S_OK) {
    return nullptr;
  }

  D3D12_DEPTH_STENCIL_VIEW_DESC dsv_descriptor = {};
  dsv_descriptor.Format = DXGI_FORMAT_D32_FLOAT;
  dsv_descriptor.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
  dsv_descriptor.Texture2D.MipSlice = 0;
  dsv_descriptor.Flags = D3D12_DSV_FLAG_NONE;

  device2->CreateDepthStencilView(
      depth_buffer.Get(), &dsv_descriptor,
      dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart());

  // CBV, SRV, UAV descriptor heap
  D3D12_DESCRIPTOR_HEAP_DESC cbv_srv_uav_descriptor = {};
  cbv_srv_uav_descriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
  cbv_srv_uav_descriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
  cbv_srv_uav_descriptor.NumDescriptors = DESCRIPTOR_HEAP_MAX_HANDLES;

  ComPtr<ID3D12DescriptorHeap> cbv_srv_uav_descriptor_heap;
  if (device2->CreateDescriptorHeap(
          &cbv_srv_uav_descriptor,
          IID_PPV_ARGS(&cbv_srv_uav_descriptor_heap)) != S_OK) {
    return nullptr;
  }

  UINT cbv_srv_uav_descriptor_size =
      device2->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

  result->device2 = device2;
  result->command_queue = command_queue;
  result->swap_chain4 = swap_chain4;
  result->command_list = command_list;
  result->fence = fence;
  result->fence_event = fence_event;
  result->current_back_buffer_index = current_back_buffer_index;
  result->rtv_descriptor_heap = rtv_descriptor_heap;
  result->rtv_descriptor_size = rtv_descriptor_size;
  result->scissor_rect = scissor_rect;
  result->viewport = viewport;
  result->dsv_descriptor_heap = dsv_descriptor_heap;
  result->cbv_srv_uav_descriptor_heap = cbv_srv_uav_descriptor_heap;
  result->depth_buffer = depth_buffer;
  result->cbv_srv_uav_descriptor_size = cbv_srv_uav_descriptor_size;
  result->vsync = 0;
  result->fence_value = 0;
  ZeroMemory(result->fence_values, sizeof(uint64_t) * FRAME_COUNT);

  return result;
}

static void DirectxUpdateBufferResource(
    Directx *directx, ID3D12Resource **destination,
    ID3D12Resource **intermediate, size_t elements_count, size_t elements_size,
    const void *buffer_data,
    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE) {
  auto device2 = directx->device2;
  auto command_list = directx->command_list;

  size_t buffer_size = elements_count * elements_size;

  device2->CreateCommittedResource(
      &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
      &CD3DX12_RESOURCE_DESC::Buffer(buffer_size, flags),
      D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(destination));

  if (buffer_data) {
    device2->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(buffer_size, flags),
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(intermediate));

    D3D12_SUBRESOURCE_DATA sd = {};
    sd.pData = buffer_data;
    sd.RowPitch = buffer_size;
    sd.SlicePitch = sd.RowPitch;

    UpdateSubresources(command_list.Get(), *destination, *intermediate, 0, 0, 1,
                       &sd);
  }
}

static bool DirectxWaitForGPU(Directx *directx) {
  auto command_queue = directx->command_queue;
  auto fence = directx->fence;
  auto fence_event = directx->fence_event;
  auto &fence_value = directx->fence_values[directx->current_back_buffer_index];

  if (command_queue->Signal(fence.Get(), fence_value) != S_OK) {
    return false;
  }

  if (fence->SetEventOnCompletion(fence_value, fence_event) != S_OK) {
    return false;
  }

  WaitForSingleObjectEx(fence_event, INFINITE, FALSE);

  fence_value++;

  return true;
}