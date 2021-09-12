static Entry *CreateEntry(Window *window, Directx *directx, Camera *camera, Cube *cube) {
  Entry *result = new Entry;

  result->window = window;
  result->directx = directx;
  result->camera = camera;
  result->cube = cube;

  int width = window->width;
  int height = window->height;
  float aspect_ratio = width / static_cast<float>(height);
  result->projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), aspect_ratio,
                                        0.1f, FOV);

  return result;
}

inline void EntryRender(Entry *entry, float dt) {
  auto directx = entry->directx;
  auto cube = entry->cube;
  auto window = entry->window;
  auto pipeline_state = cube->pipeline_state;
  auto camera = entry->camera;
  auto view = camera->view;
  auto projection = entry->projection;
  auto look_direction = camera->look_direction;

  DirectxRenderBegin(directx, pipeline_state);
  CubeRender(cube, directx, look_direction, view, projection);
  DirectxRenderEnd(directx);
}

inline void EntryUpdate(Entry *entry, float dt) {
  Cube *cube = entry->cube;
  Camera *camera = entry->camera;

  CameraUpdate(camera, dt);
  CubeUpdate(cube, dt);
}

static void EntryMainLoop(Entry *entry) {
  MSG msg = {};

  float update_timer = 0;
  float frame_time = 1 / (float)144;

  TimePoint last_time = GetTime();

  while (msg.message != WM_QUIT) {
    TimePoint start_time = GetTime();

    float passed_time = GetDuration(start_time, last_time);

    update_timer += passed_time;

    bool is_should_render = false;
    if (update_timer >= frame_time) {
      EntryUpdate(entry, frame_time);

      update_timer -= frame_time;
      is_should_render = true;
    }

    if (is_should_render) {
      EntryRender(entry, frame_time);
    }

    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    last_time = start_time;
  }
}