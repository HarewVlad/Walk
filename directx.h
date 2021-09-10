#define DESCRIPTOR_HEAP_MAX_HANDLES 256
#define FRAME_COUNT 3

struct Directx {
  ComPtr<ID3D12Device2> device2;
  ComPtr<IDXGISwapChain4> swap_chain4;
  ComPtr<ID3D12CommandQueue> command_queue;
  ComPtr<ID3D12Resource> back_buffers[FRAME_COUNT];
  ComPtr<ID3D12GraphicsCommandList> command_list;
  ComPtr<ID3D12CommandAllocator> command_allocators[FRAME_COUNT];
  ComPtr<ID3D12DescriptorHeap> rtv_descriptor_heap;
  ComPtr<ID3D12DescriptorHeap> dsv_descriptor_heap;
  ComPtr<ID3D12DescriptorHeap> cbv_srv_uav_descriptor_heap;
  ComPtr<ID3D12Resource> depth_buffer;
  D3D12_VIEWPORT viewport;
  D3D12_RECT scissor_rect;
  ComPtr<ID3D12Fence> fence;
  uint64_t fence_value;
  uint64_t fence_values[FRAME_COUNT];
  HANDLE fence_event;
  UINT rtv_descriptor_size;
  UINT cbv_srv_uav_descriptor_size;
  UINT current_back_buffer_index;
  BOOL vsync;
};