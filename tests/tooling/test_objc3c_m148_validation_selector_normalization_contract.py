from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TESTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m148_validation_selector_normalization_runbook_exists() -> None:
    tests_fragment = _read(TESTS_DOC_FRAGMENT)
    grammar_fragment = _read(GRAMMAR_DOC_FRAGMENT)

    for text in (
        "## M148 validation selector-normalized method declaration runbook",
        "python -m pytest tests/tooling/test_objc3c_m148_frontend_selector_normalization_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m148_sema_selector_normalization_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m148_lowering_selector_normalization_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m148_validation_selector_normalization_contract.py -q",
        "npm run check:objc3c:m148-selector-normalization",
    ):
        assert text in tests_fragment

    assert "## M148 frontend selector-normalized method declaration grammar" in grammar_fragment
