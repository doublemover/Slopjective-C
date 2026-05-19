enum class Objc3SemanticCanonicalTypeKind : std::uint8_t {
  Unknown = 0,
  Scalar = 1,
  Function = 2,
  Block = 3,
  Object = 4,
  ClassObject = 5,
  Selector = 6,
  ProtocolObject = 7,
  Instancetype = 8,
  ObjectPointer = 9,
  ForeignObject = 10,
  Vector = 11,
};

enum class Objc3SemanticCanonicalNullability : std::uint8_t {
  Unspecified = 0,
  Nullable = 1,
  Nonnull = 2,
  ImplicitlyUnwrapped = 3,
  NullResettable = 4,
  Inherited = 5,
};

enum class Objc3SemanticCanonicalOwnership : std::uint8_t {
  Unspecified = 0,
  Strong = 1,
  Copy = 2,
  Retain = 3,
  Weak = 4,
  Unowned = 5,
  UnsafeUnretained = 6,
  Assign = 7,
};
