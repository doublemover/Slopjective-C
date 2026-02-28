from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TESTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m151_validation_symbol_graph_runbook_exists() -> None:
    tests_fragment = _read(TESTS_DOC_FRAGMENT)
    grammar_fragment = _read(GRAMMAR_DOC_FRAGMENT)

    for text in (
        "## M151 validation symbol graph and scope resolution overhaul runbook",
        "python -m pytest tests/tooling/test_objc3c_m151_frontend_symbol_graph_scope_resolution_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m151_sema_symbol_graph_scope_resolution_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m151_lowering_symbol_graph_scope_resolution_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m151_validation_symbol_graph_scope_resolution_contract.py -q",
        "npm run check:objc3c:m151-symbol-graph-scope-resolution",
    ):
        assert text in tests_fragment

    assert "## M151 frontend symbol graph and scope-resolution parser surface" in grammar_fragment
