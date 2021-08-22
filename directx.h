struct Directx {
  static const UINT FRAMES_COUNT = 3;

  ComPtr<ID3D12Device2> device2;
  ComPtr<IDXGISwapChain4> swap_chain4;
  ComPtr<ID3D12CommandQueue> command_queue_direct;
  ComPtr<ID3D12CommandQueue> command_queue_copy;
  ComPtr<ID3D12Resource> back_buffers[FRAMES_COUNT];
  ComPtr<ID3D12GraphicsCommandList> command_list_direct;
  ComPtr<ID3D12GraphicsCommandList> command_list_copy;
  ComPtr<ID3D12CommandAllocator> command_allocators_direct[FRAMES_COUNT];
  ComPtr<ID3D12CommandAllocator> command_allocators_copy[FRAMES_COUNT];
  ComPtr<ID3D12DescriptorHeap> rtv_descriptor_heap;
  ComPtr<ID3D12DescriptorHeap> dsv_descriptor_heap;
  ComPtr<ID3D12DescriptorHeap> srv_descriptor_heap;
  ComPtr<ID3D12Resource> depth_buffer;
  D3D12_VIEWPORT viewport;
  D3D12_RECT scissor_rect;
  ComPtr<ID3D12Fence> fence;
  uint64_t fence_value;
  uint64_t fence_values[FRAMES_COUNT];
  HANDLE fence_event;
  UINT rtv_descriptor_size;
  UINT current_back_buffer_index;
  BOOL vsync;
};

#define DESCRIPTOR_HEAP_MAX_HANDLES 256