from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TESTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m147_validation_protocol_category_runbook_exists() -> None:
    tests_fragment = _read(TESTS_DOC_FRAGMENT)
    grammar_fragment = _read(GRAMMAR_DOC_FRAGMENT)

    for text in (
        "## M147 validation @protocol/@category runbook",
        "python -m pytest tests/tooling/test_objc3c_m147_frontend_protocol_category_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m147_sema_protocol_category_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m147_lowering_protocol_category_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m147_validation_protocol_category_contract.py -q",
        "npm run check:objc3c:m147-protocol-category",
    ):
        assert text in tests_fragment

    assert "## M147 frontend @protocol/@category grammar" in grammar_fragment
