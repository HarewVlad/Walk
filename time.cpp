TimePoint GetTime() { return std::chrono::high_resolution_clock::now(); }

float GetDuration(TimePoint timeEnd, TimePoint timeStart) {
  return std::chrono::duration<float>(timeEnd - timeStart).count();
}
