from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m203_lowering_compile_time_eval_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M203 lowering/runtime compile-time evaluation engine",
        "tmp/artifacts/compilation/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/",
        "tmp/reports/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/",
        "TryGetCompileTimeI32ExprInContext",
        "IsCompileTimeNilReceiverExprInContext",
        "IsCompileTimeKnownNonNilExprInContext",
        "receiver_is_compile_time_zero",
        "receiver_is_compile_time_nonzero",
        "global_proofs_invalidated",
        "runtime_dispatch_symbol",
        "runtime_dispatch_arg_slots",
        "selector_global_ordering",
        "python -m pytest tests/tooling/test_objc3c_m203_lowering_compile_time_eval_contract.py -q",
    ):
        assert text in fragment


def test_m203_lowering_compile_time_eval_markers_map_to_sources() -> None:
    emitter_source = _read(IR_EMITTER_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)

    for marker in (
        "bool TryGetCompileTimeI32ExprInContext(const Expr *expr, const FunctionContext &ctx, int &value) const {",
        "bool IsCompileTimeNilReceiverExprInContext(const Expr *expr, const FunctionContext &ctx) const {",
        "bool IsCompileTimeKnownNonNilExprInContext(const Expr *expr, const FunctionContext &ctx) const {",
        "const bool has_assigned_const_value =",
        "const bool has_assigned_nil_value = op == \"=\" && value_expr != nullptr && IsCompileTimeNilReceiverExprInContext(value_expr, ctx);",
        "const bool has_clause_const_value = TryGetCompileTimeI32ExprInContext(clause.value.get(), ctx, clause_const_value);",
        "const bool has_let_const_value = TryGetCompileTimeI32ExprInContext(let->value.get(), ctx, let_const_value);",
        "ctx.const_value_ptrs.erase(ptr);",
        "ctx.nil_bound_ptrs.erase(ptr);",
        "ctx.nonzero_bound_ptrs.erase(ptr);",
        "ctx.global_proofs_invalidated = true;",
        "lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);",
        "lowered.receiver_is_compile_time_nonzero = IsCompileTimeKnownNonNilExprInContext(expr->receiver.get(), ctx);",
    ):
        assert marker in emitter_source

    for marker in (
        "Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary)",
        "invalid lowering contract runtime_dispatch_symbol",
        "return \"runtime_dispatch_symbol=\" + boundary.runtime_dispatch_symbol +",
    ):
        assert marker in lowering_contract_source

    for marker in (
        'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
        '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";',
    ):
        assert marker in artifacts_source
