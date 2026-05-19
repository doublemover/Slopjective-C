std::string BuildObjc3BlockEscapeShapeSymbol(
    Objc3BlockLiteralSourceUseKind use_kind) {
  switch (use_kind) {
  case Objc3BlockLiteralSourceUseKind::ExpressionSite:
    return "expression-site";
  case Objc3BlockLiteralSourceUseKind::GlobalInitializer:
    return "global-initializer";
  case Objc3BlockLiteralSourceUseKind::LocalBindingInitializer:
    return "binding-initializer";
  case Objc3BlockLiteralSourceUseKind::AssignmentValue:
    return "assignment-value";
  case Objc3BlockLiteralSourceUseKind::ReturnValue:
    return "return-value";
  case Objc3BlockLiteralSourceUseKind::CallArgument:
    return "call-argument";
  case Objc3BlockLiteralSourceUseKind::MessageArgument:
    return "message-argument";
  }
  return "expression-site";
}

bool Objc3BlockEscapeShapePromotesToHeapCandidate(
    Objc3BlockLiteralSourceUseKind use_kind) {
  return use_kind != Objc3BlockLiteralSourceUseKind::ExpressionSite;
}

std::string BuildObjc3BlockHelperIntentProfile(
    std::size_t mutated_capture_count,
    std::size_t byref_capture_count,
    bool copy_helper_intent_required,
    bool dispose_helper_intent_required,
    Objc3BlockLiteralSourceUseKind use_kind) {
  std::ostringstream out;
  out << "block-helper-intent:mutable-captures=" << mutated_capture_count
      << ";byref-captures=" << byref_capture_count
      << ";copy-helper="
      << (copy_helper_intent_required ? "candidate" : "elided")
      << ";dispose-helper="
      << (dispose_helper_intent_required ? "candidate" : "elided")
      << ";escape-shape=" << BuildObjc3BlockEscapeShapeSymbol(use_kind);
  return out.str();
}

std::string BuildObjc3BlockEscapeShapeProfile(
    Objc3BlockLiteralSourceUseKind use_kind,
    bool promotes_to_heap_candidate,
    std::size_t capture_count,
    std::size_t byref_capture_count) {
  std::ostringstream out;
  out << "block-escape-shape:site=" << BuildObjc3BlockEscapeShapeSymbol(use_kind)
      << ";heap-candidate="
      << (promotes_to_heap_candidate ? "true" : "false")
      << ";captures=" << capture_count
      << ";byref-captures=" << byref_capture_count;
  return out.str();
}
