from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
API_HEADER = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "api.h"
ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
CLI_HEADER = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "objc3_cli_frontend.h"
CLI_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "objc3_cli_frontend.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_frontend_anchor_compile_entrypoints_are_pipeline_backed() -> None:
    source = _read(ANCHOR_CPP)

    assert '#include "libobjc3c_frontend/objc3_cli_frontend.h"' in source
    assert "CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options)" in source
    assert "libobjc3c_frontend compile entrypoints are scaffolded only" not in source
    assert "OBJC3C_FRONTEND_STATUS_DIAGNOSTICS" in source
    assert "OBJC3C_FRONTEND_STATUS_EMIT_ERROR" in source


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


def test_public_api_documents_pipeline_backed_compile_behavior() -> None:
    api_header = _read(API_HEADER)

    assert "Pipeline-backed behavior:" in api_header
    assert "Runs lexer/parser/sema/lower/emit through the extracted frontend pipeline." in api_header
    assert "Current implementation status: scaffolded" not in api_header
