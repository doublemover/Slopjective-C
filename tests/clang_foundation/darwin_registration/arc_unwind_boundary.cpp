extern "C" int darwin_cpp_boundary(int payload, bool raise) {
  if (raise)
    throw payload;
  return payload;
}
