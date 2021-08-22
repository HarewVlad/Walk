static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                LPARAM lParam) {
  switch (msg) {
    case WM_CLOSE:
      PostQuitMessage(0);
      break;
      default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
  }
}

static Window *CreateWindow(HINSTANCE instance, UINT width,
                                    UINT height) {
  Window *result = new Window;

  WNDCLASS wc;
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = instance;
  wc.hIcon = LoadIcon(0, IDI_APPLICATION);
  wc.hCursor = LoadCursor(0, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszMenuName = 0;
  wc.lpszClassName = "Main Window";

  if (!RegisterClass(&wc)) {
    PrintLastError("RegisterClass");
    return nullptr;
  }

  RECT rect = {0, 0, width, height};
  AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
  HWND hwnd =
      CreateWindowEx(0, "Main Window", "Main Window", WS_OVERLAPPEDWINDOW,
                   CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left,
                   rect.bottom - rect.top, 0, 0, instance, 0);

  if (!hwnd) {
  	PrintLastError("Create Window");
  	return nullptr;
  }

  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);

  result->hwnd = hwnd;
  result->width = width;
  result->height = height;
  result->instance = instance;

  return result;
}