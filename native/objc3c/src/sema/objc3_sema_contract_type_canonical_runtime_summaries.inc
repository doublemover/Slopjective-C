enum class Objc3SemaAtomicMemoryOrder : std::uint8_t {
  Relaxed = 0,
  Acquire = 1,
  Release = 2,
  AcqRel = 3,
  SeqCst = 4,
  Unsupported = 5,
};

struct Objc3AtomicMemoryOrderMappingSummary {
  std::size_t relaxed = 0;
  std::size_t acquire = 0;
  std::size_t release = 0;
  std::size_t acq_rel = 0;
  std::size_t seq_cst = 0;
  std::size_t unsupported = 0;
  bool deterministic = true;

  std::size_t total() const {
    return relaxed + acquire + release + acq_rel + seq_cst + unsupported;
  }
};

struct Objc3VectorTypeLoweringSummary {
  std::size_t return_annotations = 0;
  std::size_t param_annotations = 0;
  std::size_t i32_annotations = 0;
  std::size_t bool_annotations = 0;
  std::size_t lane2_annotations = 0;
  std::size_t lane4_annotations = 0;
  std::size_t lane8_annotations = 0;
  std::size_t lane16_annotations = 0;
  std::size_t unsupported_annotations = 0;
  bool deterministic = true;

  std::size_t total() const {
    return return_annotations + param_annotations;
  }
};
