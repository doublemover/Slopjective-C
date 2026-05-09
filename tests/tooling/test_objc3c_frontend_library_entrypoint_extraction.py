from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FRONTEND_HEADER = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "objc3c_frontend.h"
OPTIONS_HEADER = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "objc3c_frontend_options.h"
ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
CLI_HEADER = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "objc3_cli_frontend.h"
CLI_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "objc3_cli_frontend.cpp"


def _read(path: Path) -> str:
    text = path.read_text(encoding="utf-8")
    expanded: list[str] = []
    for line in text.splitlines():
        expanded.append(line)
        stripped = line.strip()
        if not stripped.startswith('#include "'):
            continue
        include_path = stripped.split('"', 2)[1]
        target = ROOT / "native" / "objc3c" / "src" / include_path
        if target.exists():
            expanded.append(target.read_text(encoding="utf-8"))
    return "\n".join(expanded)


def _assert_in_order(text: str, snippets: list[str]) -> None:
    cursor = -1
    for snippet in snippets:
        index = text.find(snippet)
        assert index != -1, f"missing snippet: {snippet}"
        assert index > cursor, f"snippet out of order: {snippet}"
        cursor = index


def test_frontend_anchor_compile_entrypoints_are_pipeline_backed() -> None:
    source = _read(ANCHOR_CPP)

    assert '#include "libobjc3c_frontend/objc3_cli_frontend.h"' in source
    assert '#include "io/objc3_process.h"' in source
    assert "CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options)" in source
    assert "libobjc3c_frontend compile entrypoints are scaffolded only" not in source
    assert "OBJC3C_FRONTEND_STATUS_DIAGNOSTICS" in source
    assert "OBJC3C_FRONTEND_STATUS_EMIT_ERROR" in source
    assert "OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT" in source
    assert "RunIRCompileLLVMDirect(" in source
    assert "NormalizeLanguageVersion(options.language_version)" in source
    assert "NormalizeCompatibilityMode(options.compatibility_mode)" not in source
    assert "ValidateSupportedLanguageVersion(options->language_version, language_version_error)" in source
    assert "ValidateSupportedCompatibilityMode(options->compatibility_mode, compatibility_mode_error)" not in source
    assert "unsupported compile_options.language_version:" in source
    assert "unsupported compile_options.compatibility_mode:" not in source
    assert "Objc3FrontendOptions frontend_options = BuildFrontendOptions(*options);" in source
    assert "frontend_options.language_profile = Objc3FrontendLanguageProfile::kCanonical;" in source
    assert "OBJC3C_FRONTEND_COMPATIBILITY_MODE_LEGACY" not in source
    assert "Objc3FrontendCompileProduct product = CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options);" in source
    assert "std::vector<std::string> emit_diagnostics = product.artifact_bundle.post_pipeline_diagnostics;" in source
    assert "product.pipeline_result.stage_diagnostics" in source

    _assert_in_order(
        source,
        [
            "result->lex = BuildStageSummary(",
            "result->parse =",
            "result->sema = BuildStageSummary(",
            "result->lower = BuildStageSummary(",
            "result->emit = BuildStageSummary(",
        ],
    )

    _assert_in_order(
        source,
        [
            "if (!ValidateSupportedLanguageVersion(options->language_version, language_version_error)) {",
            "return SetUsageError(context, result, language_version_error);",
            "if (IsMissingBorrowedPath(options->input_path)) {",
        ],
    )


def test_cli_frontend_exports_reusable_pipeline_compile_product() -> None:
    header = _read(CLI_HEADER)
    source = _read(CLI_CPP)

    assert "struct Objc3FrontendCompileProduct" in header
    assert "Objc3FrontendPipelineResult pipeline_result;" in header
    assert "Objc3FrontendArtifactBundle artifact_bundle;" in header
    assert "Objc3FrontendCompileProduct CompileObjc3SourceWithPipeline(" in header

    assert "Objc3FrontendCompileProduct CompileObjc3SourceWithPipeline(" in source
    assert "product.pipeline_result = RunObjc3FrontendPipeline(source, options);" in source
    assert "product.artifact_bundle = BuildObjc3FrontendArtifacts(input_path, product.pipeline_result, options);" in source
    assert "Objc3FrontendCompileProduct product = CompileObjc3SourceWithPipeline(input_path, source, options);" in source
    assert "return std::move(product.artifact_bundle);" in source


def test_public_api_documents_pipeline_backed_compile_behavior() -> None:
    frontend_header = _read(FRONTEND_HEADER)
    options_header = _read(OPTIONS_HEADER)

    assert "Pipeline-backed behavior:" in frontend_header
    assert "Runs lexer/parser/sema/lower/emit through the extracted frontend pipeline." in frontend_header
    assert "Current implementation status: scaffolded" not in frontend_header
    assert "#define OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3 3u" in options_header
    assert "#define OBJC3C_FRONTEND_LANGUAGE_VERSION_DEFAULT OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3" in options_header
    assert "objc3c_frontend_borrowed_path_t input_path;" in options_header
    assert "objc3c_frontend_borrowed_text_t source_text;" in options_header
    assert "const char *input_path;" not in options_header
    assert "uint8_t language_version;" in options_header
