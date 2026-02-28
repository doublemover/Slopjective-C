from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IR_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m203_frontend_compile_time_eval_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M203 frontend compile-time evaluation engine",
        "EvalConstExpr(...)",
        "ResolveGlobalInitializerValues(...)",
        "O3S210",
        "O3L300",
        "manifest semantic surface remains deterministic",
        "python -m pytest tests/tooling/test_objc3c_m203_frontend_compile_time_eval_contract.py -q",
    ):
        assert text in fragment


def test_m203_frontend_compile_time_eval_markers_map_to_sources() -> None:
    sema_source = _read(SEMA_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)
    ir_source = _read(IR_SOURCE)

    for marker in (
        "static bool EvalConstExpr(const Expr *expr, int &value,",
        "bool ResolveGlobalInitializerValues(const std::vector<Objc3ParsedGlobalDecl> &globals, std::vector<int> &values) {",
        'MakeDiag(global.line, global.column, "O3S210", "global initializer must be constant expression")',
    ):
        assert marker in sema_source

    assert "ResolveGlobalInitializerValues(program.globals, resolved_global_values)" in artifacts_source
    assert 'MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: global initializer failed const evaluation")' in (
        artifacts_source
    )
    assert "if (!ResolveGlobalInitializerValues(program_.globals, resolved_global_values) ||" in ir_source
    assert 'error = "global initializer failed const evaluation";' in ir_source
