from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m206_frontend_canonical_optimization_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M206 frontend canonical optimization pipeline stage-1",
        "BuildObjc3AstFromTokens(tokens)",
        "manifest_function_names.insert(fn.name).second",
        '"function_signature_surface"',
        '"scalar_return_i32"',
        '"scalar_return_bool"',
        '"scalar_return_void"',
        '"scalar_param_i32"',
        '"scalar_param_bool"',
        '"declared_globals"',
        '"declared_functions"',
        '"resolved_global_symbols"',
        '"resolved_function_symbols"',
        "python -m pytest tests/tooling/test_objc3c_m206_frontend_canonical_optimization_contract.py -q",
    ):
        assert text in fragment


def test_m206_frontend_canonical_optimization_markers_map_to_sources() -> None:
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    assert "Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);" in pipeline_source
    assert "if (manifest_function_names.insert(fn.name).second) {" in artifacts_source
    assert '<< ",\\"function_signature_surface\\":{\\"scalar_return_i32\\":" << scalar_return_i32' in artifacts_source
    assert '<< ",\\"scalar_return_bool\\":" << scalar_return_bool' in artifacts_source
    assert '<< ",\\"scalar_return_void\\":" << scalar_return_void << ",\\"scalar_param_i32\\":" << scalar_param_i32' in artifacts_source
    assert '<< ",\\"scalar_param_bool\\":" << scalar_param_bool << "}}\\n";' in artifacts_source

    for marker in (
        'manifest << "      \\"semantic_surface\\": {\\"declared_globals\\":" << program.globals.size()',
        '<< ",\\"declared_functions\\":" << manifest_functions.size()',
        '<< ",\\"resolved_global_symbols\\":" << pipeline_result.integration_surface.globals.size()',
        '<< ",\\"resolved_function_symbols\\":" << pipeline_result.integration_surface.functions.size()',
    ):
        assert marker in artifacts_source
