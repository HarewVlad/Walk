static Entry *CreateEntry(Window *window, Directx *directx, Cube *cube) {
  Entry *result = new Entry;

  result->window = window;
  result->directx = directx;
  result->cube = cube;

  return result;
}

inline void EntryRender(Entry *entry, float dt) {
  auto directx = entry->directx;
  auto cube = entry->cube;
  auto pipeline_state = cube->pipeline_state;

  DirectxRenderBegin(directx, pipeline_state);
#ifdef _DEBUG
  CubeRender(cube, directx);
#endif
  DirectxRenderEnd(directx);
}

inline void EntryUpdate(Entry *entry, float dt) {
  Cube *cube = entry->cube;

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