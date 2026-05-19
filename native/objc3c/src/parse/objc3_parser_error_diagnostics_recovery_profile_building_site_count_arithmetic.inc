bool TryAddErrorDiagnosticsRecoverySiteCounts(
    std::size_t lhs,
    std::size_t rhs,
    std::size_t &out) {
  if (lhs > std::numeric_limits<std::size_t>::max() - rhs) {
    return false;
  }
  out = lhs + rhs;
  return true;
}
