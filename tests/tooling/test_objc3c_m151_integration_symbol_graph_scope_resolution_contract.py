from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m151_integration_symbol_graph_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M151 integration symbol graph and scope resolution overhaul",
        "check:objc3c:m151-symbol-graph-scope-resolution",
        "tests/tooling/test_objc3c_m151_frontend_symbol_graph_scope_resolution_contract.py",
        "tests/tooling/test_objc3c_m151_sema_symbol_graph_scope_resolution_contract.py",
        "tests/tooling/test_objc3c_m151_lowering_symbol_graph_scope_resolution_contract.py",
        "tests/tooling/test_objc3c_m151_validation_symbol_graph_scope_resolution_contract.py",
        "tests/tooling/test_objc3c_m151_integration_symbol_graph_scope_resolution_contract.py",
    ):
        assert text in library_api_doc


def test_m151_integration_symbol_graph_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)

    assert '"check:objc3c:m151-symbol-graph-scope-resolution"' in package_json
    assert (
        "python -m pytest tests/tooling/test_objc3c_m151_frontend_symbol_graph_scope_resolution_contract.py "
        "tests/tooling/test_objc3c_m151_sema_symbol_graph_scope_resolution_contract.py "
        "tests/tooling/test_objc3c_m151_lowering_symbol_graph_scope_resolution_contract.py "
        "tests/tooling/test_objc3c_m151_validation_symbol_graph_scope_resolution_contract.py "
        "tests/tooling/test_objc3c_m151_integration_symbol_graph_scope_resolution_contract.py -q"
    ) in package_json

    assert "Run M151 symbol-graph/scope-resolution gate" in workflow
    assert "npm run check:objc3c:m151-symbol-graph-scope-resolution" in workflow
