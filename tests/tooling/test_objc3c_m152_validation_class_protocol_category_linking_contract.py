from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TESTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m152_validation_class_protocol_category_runbook_exists() -> None:
    tests_fragment = _read(TESTS_DOC_FRAGMENT)
    grammar_fragment = _read(GRAMMAR_DOC_FRAGMENT)

    for text in (
        "## M152 validation class-protocol-category semantic linking runbook",
        "python -m pytest tests/tooling/test_objc3c_m152_frontend_class_protocol_category_linking_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m152_sema_class_protocol_category_linking_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m152_lowering_class_protocol_category_linking_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m152_validation_class_protocol_category_linking_contract.py -q",
        "npm run check:objc3c:m152-class-protocol-category-linking",
    ):
        assert text in tests_fragment

    assert "## M152 frontend class-protocol-category semantic-linking parser surface" in grammar_fragment
