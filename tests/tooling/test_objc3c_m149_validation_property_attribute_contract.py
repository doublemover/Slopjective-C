from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TESTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m149_validation_property_attribute_runbook_exists() -> None:
    tests_fragment = _read(TESTS_DOC_FRAGMENT)
    grammar_fragment = _read(GRAMMAR_DOC_FRAGMENT)

    for text in (
        "## M149 validation @property grammar and attribute parsing runbook",
        "python -m pytest tests/tooling/test_objc3c_m149_frontend_property_attribute_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m149_sema_property_attribute_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m149_lowering_property_attribute_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m149_validation_property_attribute_contract.py -q",
        "npm run check:objc3c:m149-property-attributes",
    ):
        assert text in tests_fragment

    assert "## M149 frontend @property grammar and attribute parsing" in grammar_fragment
