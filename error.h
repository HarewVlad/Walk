#define CheckIfS_OK(x) \
  if (!SUCCEEDED(x)) MessageBoxA(NULL, #x, "Error", MB_OK);

inline void PrintLastError(const std::string& function_name) {
  std::string error_message =
      function_name + " error (" + std::to_string(GetLastError()) + ")";

  MessageBoxA(NULL, error_message.c_str(), "", MB_OK);
}