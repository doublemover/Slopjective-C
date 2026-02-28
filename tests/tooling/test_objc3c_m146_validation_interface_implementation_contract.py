from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TESTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m146_validation_interface_implementation_runbook_exists() -> None:
    tests_fragment = _read(TESTS_DOC_FRAGMENT)
    grammar_fragment = _read(GRAMMAR_DOC_FRAGMENT)

    for text in (
        "## M146 validation @interface/@implementation runbook",
        "python -m pytest tests/tooling/test_objc3c_m146_frontend_interface_implementation_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m146_sema_interface_implementation_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m146_lowering_interface_implementation_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m146_validation_interface_implementation_contract.py -q",
        "npm run check:objc3c:m146-interface-implementation",
    ):
        assert text in tests_fragment

    assert "## M146 frontend @interface/@implementation grammar" in grammar_fragment
