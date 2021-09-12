#include "main.h"
#include "time.cpp"
#include "window.cpp"
#include "directx.cpp"
#include "texture.cpp"
#include "camera.cpp"

// Tests
#include "tests\cube.cpp"
#include "entry.cpp"

// TODO: Descriptor allocator

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int) {
  Window *window = CreateWindow(instance, 1920, 1080);
  if (!window) {
    return 1;
  }

  Directx *directx = CreateDirectx(window);
  if (!directx) {
    return 1;
  }

  Texture *texture = CreateTexture(L"test_image_jpg.jpg", directx);
  Cube *cube = CreateCube(directx, texture, {0, 0, 0}, 100);
  if (!cube) {
    return 1;
  }

  Camera *camera = CreateCamera({0, 0, 50});
  if (!camera) {
    return 1;
  }

  Entry *entry = CreateEntry(window, directx, camera, cube);
  if (!entry) {
    return 1;
  }
  
  EntryMainLoop(entry);
}