from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
FRONTEND_ANCHOR = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m142_frontend_cli_c_api_parity_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M142 frontend CLI and C API parity harness",
        "frontend parity packet 1.1 deterministic compile option prevalidation",
        "m142_frontend_cli_c_api_prevalidation_packet",
        "ValidateSupportedLanguageVersion(options->language_version, language_version_error)",
        "ValidateSupportedCompatibilityMode(options->compatibility_mode, compatibility_mode_error)",
        "TryNormalizeObjc3LoweringContract(frontend_options.lowering, normalized_lowering, lowering_error)",
        "objc3c_frontend_set_error(context, lowering_error.c_str());",
        "CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options)",
        "python -m pytest tests/tooling/test_objc3c_m142_frontend_cli_c_api_parity_contract.py -q",
    ):
        assert text in fragment


def test_m142_frontend_cli_c_api_parity_markers_map_to_source() -> None:
    source = _read(FRONTEND_ANCHOR)
    for marker in (
        "ValidateSupportedLanguageVersion(options->language_version, language_version_error)",
        "ValidateSupportedCompatibilityMode(options->compatibility_mode, compatibility_mode_error)",
        "Objc3FrontendOptions frontend_options = BuildFrontendOptions(*options);",
        "TryNormalizeObjc3LoweringContract(frontend_options.lowering, normalized_lowering, lowering_error)",
        "result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;",
        "result->process_exit_code = 2;",
        "objc3c_frontend_set_error(context, lowering_error.c_str());",
        "frontend_options.lowering = normalized_lowering;",
        "Objc3FrontendCompileProduct product = CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options);",
    ):
        assert marker in source
