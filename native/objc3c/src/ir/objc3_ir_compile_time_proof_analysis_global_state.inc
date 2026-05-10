void InvalidateObjc3IRGlobalProofState(FunctionContext &ctx) {
  ctx.global_proofs_invalidated = true;
  for (auto it = ctx.nil_bound_ptrs.begin(); it != ctx.nil_bound_ptrs.end();) {
    if (it->rfind("@", 0) == 0) {
      it = ctx.nil_bound_ptrs.erase(it);
    } else {
      ++it;
    }
  }
  for (auto it = ctx.nonzero_bound_ptrs.begin();
       it != ctx.nonzero_bound_ptrs.end();) {
    if (it->rfind("@", 0) == 0) {
      it = ctx.nonzero_bound_ptrs.erase(it);
    } else {
      ++it;
    }
  }
  for (auto it = ctx.const_value_ptrs.begin();
       it != ctx.const_value_ptrs.end();) {
    if (it->first.rfind("@", 0) == 0) {
      it = ctx.const_value_ptrs.erase(it);
    } else {
      ++it;
    }
  }
}
