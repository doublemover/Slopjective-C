std::string EmitObjc3IRLocalI32Binding(
    const std::string &name, const Expr *value_expr, FunctionContext &ctx,
    bool mark_runtime_nonnull,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  const std::string value = callbacks.emit_expr(value_expr, ctx);
  if (ctx.terminated) {
    return value;
  }
  const Objc3IRLocalI32StorageMetadata metadata =
      CollectObjc3IRLocalI32StorageMetadata(value_expr, ctx, callbacks);
  const std::string ptr =
      "%" + name + ".addr." + std::to_string(ctx.temp_counter++);
  ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
  ctx.scopes.back()[name] = ptr;
  RecordObjc3IRLocalI32StorageMetadata(ptr, metadata, ctx,
                                       mark_runtime_nonnull);
  ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr +
                           ", align 4");
  return value;
}
