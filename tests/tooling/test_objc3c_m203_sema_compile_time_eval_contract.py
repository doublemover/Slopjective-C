from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_PASSES_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
STATIC_ANALYSIS_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_static_analysis.h"
STATIC_ANALYSIS_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_static_analysis.cpp"
IR_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m203_sema_compile_time_evaluation_engine_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M203 sema/type compile-time evaluation engine",
        "compile-time eval packet 1.1 deterministic sema global const-expression hooks",
        "m203_sema_global_const_eval_packet",
        "### 1.1 Deterministic sema global const-expression packet",
        "static bool EvalConstExpr(const Expr *expr, int &value,",
        "bool ResolveGlobalInitializerValues(const std::vector<Objc3ParsedGlobalDecl> &globals, std::vector<int> &values)",
        "if (!EvalConstExpr(global.value.get(), value, &resolved_global_values)) {",
        'MakeDiag(global.line, global.column, "O3S210", "global initializer must be constant expression")',
        'MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: global initializer failed const evaluation")',
        "compile-time eval packet 1.2 deterministic type/static-flow compile-time hooks",
        "m203_type_static_flow_compile_time_packet",
        "### 1.2 Deterministic type/static-flow compile-time packet",
        "using StaticScalarBindings = std::unordered_map<std::string, int>;",
        "bool TryEvalStaticScalarValue(const Expr *expr, int &value, const StaticScalarBindings *bindings);",
        "return TryEvalStaticArithmeticBinary(expr->op, lhs, rhs, value);",
        "return TryEvalStaticBitwiseShiftBinary(expr->op, lhs, rhs, value);",
        "if (TryEvalStaticScalarValue(stmt->switch_stmt->condition.get(), static_switch_value, bindings)) {",
        "bool IsCompileTimeNilReceiverExprInContext(const Expr *expr, const FunctionContext &ctx) const {",
        "bool TryGetCompileTimeI32ExprInContext(const Expr *expr, const FunctionContext &ctx, int &value) const {",
        "ctx.const_value_ptrs[ptr] = assigned_const_value;",
        "lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);",
        "python -m pytest tests/tooling/test_objc3c_m203_sema_compile_time_eval_contract.py -q",
    ):
        assert text in fragment


def test_m203_sema_compile_time_evaluation_markers_map_to_sources() -> None:
    sema_passes_source = _read(SEMA_PASSES_SOURCE)
    static_analysis_header = _read(STATIC_ANALYSIS_HEADER)
    static_analysis_source = _read(STATIC_ANALYSIS_SOURCE)
    ir_source = _read(IR_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    for marker in (
        "static bool EvalConstExpr(const Expr *expr, int &value,",
        "bool ResolveGlobalInitializerValues(const std::vector<Objc3ParsedGlobalDecl> &globals, std::vector<int> &values)",
        "if (!EvalConstExpr(global.value.get(), value, &resolved_global_values)) {",
        'MakeDiag(global.line, global.column, "O3S210", "global initializer must be constant expression")',
    ):
        assert marker in sema_passes_source

    for marker in (
        "using StaticScalarBindings = std::unordered_map<std::string, int>;",
        "bool TryEvalStaticScalarValue(const Expr *expr, int &value, const StaticScalarBindings *bindings);",
    ):
        assert marker in static_analysis_header

    for marker in (
        "return TryEvalStaticArithmeticBinary(expr->op, lhs, rhs, value);",
        "return TryEvalStaticBitwiseShiftBinary(expr->op, lhs, rhs, value);",
        "if (TryEvalStaticScalarValue(stmt->switch_stmt->condition.get(), static_switch_value, bindings)) {",
    ):
        assert marker in static_analysis_source

    for marker in (
        "bool IsCompileTimeNilReceiverExprInContext(const Expr *expr, const FunctionContext &ctx) const {",
        "bool TryGetCompileTimeI32ExprInContext(const Expr *expr, const FunctionContext &ctx, int &value) const {",
        "ctx.const_value_ptrs[ptr] = assigned_const_value;",
        "lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);",
    ):
        assert marker in ir_source

    for marker in (
        'MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: global initializer failed const evaluation")',
    ):
        assert marker in artifacts_source
