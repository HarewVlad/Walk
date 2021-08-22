#include "main.h"
#include "time.cpp"
#include "window.cpp"
// #include "descriptor_heap.cpp"
#include "directx.cpp"
#include "texture.cpp"

// Tests
#include "tests\cube.cpp"

#include "entry.cpp"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int) {
  Window *window = CreateWindow(instance, 1920, 1080);
  if (!window) {
    return 1;
  }

  Directx *directx = CreateDirectx(window);
  if (!directx) {
    return 1;
  }

  Texture *texture = CreateTexture(L"test_image_hdr.hdr", directx);

  Cube *cube = CreateCube(directx, texture);
  if (!cube) {
    return 1;
  }

  // TODO: Instead of "cube" pass:
  // struct Tests {
  //   Cube *cube;
  //   // Sphere *sphere;
  //   // Sky *sky;
  // };

  // Descriptor *srv_descriptor = CreateDescriptor(directx->device2, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
  // D3D12_CPU_DESCRIPTOR_HANDLE srv_handle = DescriptorGetCPUHandle(srv_descriptor);

  // Texture *texture = CreateTexture(L"test_image_hdr.hdr", directx->device2, srv_handle);

  Entry *entry = CreateEntry(window, directx, cube);
  EntryMainLoop(entry);
}