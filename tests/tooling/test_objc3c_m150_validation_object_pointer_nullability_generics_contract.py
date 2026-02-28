from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TESTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m150_validation_object_pointer_runbook_exists() -> None:
    tests_fragment = _read(TESTS_DOC_FRAGMENT)
    grammar_fragment = _read(GRAMMAR_DOC_FRAGMENT)

    for text in (
        "## M150 validation object-pointer declarators, nullability, lightweight generics parse runbook",
        "python -m pytest tests/tooling/test_objc3c_m150_frontend_object_pointer_nullability_generics_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m150_sema_object_pointer_nullability_generics_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m150_lowering_object_pointer_nullability_generics_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m150_validation_object_pointer_nullability_generics_contract.py -q",
        "npm run check:objc3c:m150-object-pointer-nullability-generics",
    ):
        assert text in tests_fragment

    assert "## M150 frontend object pointer declarators, nullability, lightweight generics parse" in grammar_fragment
