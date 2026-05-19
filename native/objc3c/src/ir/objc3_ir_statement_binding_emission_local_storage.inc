namespace {

struct Objc3IRLocalI32StorageMetadata {
  bool has_const_value = false;
  int const_value = 0;
  bool has_nil_value = false;
};

Objc3IRLocalI32StorageMetadata CollectObjc3IRLocalI32StorageMetadata(
    const Expr *value_expr, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  Objc3IRLocalI32StorageMetadata metadata;
  metadata.has_const_value =
      callbacks.try_get_compile_time_i32_expr(value_expr, ctx,
                                              metadata.const_value);
  metadata.has_nil_value =
      callbacks.is_compile_time_nil_receiver_expr(value_expr, ctx);
  return metadata;
}

void RecordObjc3IRLocalI32StorageMetadata(
    const std::string &ptr,
    const Objc3IRLocalI32StorageMetadata &metadata, FunctionContext &ctx,
    bool mark_runtime_nonnull) {
  if (metadata.has_nil_value) {
    ctx.nil_bound_ptrs.insert(ptr);
  }
  if (metadata.has_const_value) {
    ctx.const_value_ptrs[ptr] = metadata.const_value;
  }
  if ((metadata.has_const_value && metadata.const_value != 0) ||
      mark_runtime_nonnull) {
    ctx.nonzero_bound_ptrs.insert(ptr);
  }
}

void RecordObjc3IRAssignmentStorageMetadata(
    const std::string &ptr, bool has_assigned_nil_value,
    bool has_assigned_const_value, int assigned_const_value,
    FunctionContext &ctx) {
  if (has_assigned_nil_value && ptr.rfind("@", 0) != 0) {
    ctx.nil_bound_ptrs.insert(ptr);
  }
  if (has_assigned_const_value) {
    ctx.const_value_ptrs[ptr] = assigned_const_value;
    if (assigned_const_value != 0) {
      ctx.nonzero_bound_ptrs.insert(ptr);
    }
  }
}

}  // namespace
