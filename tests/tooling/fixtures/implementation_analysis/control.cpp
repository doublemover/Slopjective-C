int implementation_analysis_control() {
#ifdef DEFECTIVE
  int *value = nullptr;
  return *value;
#else
  int value = 42;
  return value;
#endif
}
