from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FRONTEND_ANCHOR = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _assert_in_order(text: str, snippets: list[str]) -> None:
    cursor = -1
    for snippet in snippets:
        index = text.find(snippet)
        assert index != -1, f"missing snippet: {snippet}"
        assert index > cursor, f"snippet out of order: {snippet}"
        cursor = index


def test_c_api_frontend_prevalidates_lowering_contract_before_pipeline_run() -> None:
    source = _read(FRONTEND_ANCHOR)

    assert "TryNormalizeObjc3LoweringContract(frontend_options.lowering, normalized_lowering, lowering_error)" in source
    assert "result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;" in source
    assert "result->process_exit_code = 2;" in source
    assert "objc3c_frontend_set_error(context, lowering_error.c_str());" in source

    _assert_in_order(
        source,
        [
            "Objc3FrontendOptions frontend_options = BuildFrontendOptions(*options);",
            "if (!TryNormalizeObjc3LoweringContract(frontend_options.lowering, normalized_lowering, lowering_error)) {",
            "result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;",
            "return result->status;",
            "Objc3FrontendCompileProduct product = CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options);",
        ],
    )
