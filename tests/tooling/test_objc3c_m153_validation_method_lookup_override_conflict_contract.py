from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TESTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m153_validation_method_lookup_override_conflict_runbook_exists() -> None:
    tests_fragment = _read(TESTS_DOC_FRAGMENT)
    sema_fragment = _read(SEMA_DOC_FRAGMENT)

    for text in (
        "## M153 validation method lookup, override, and conflict semantics runbook",
        "python -m pytest tests/tooling/test_objc3c_m153_frontend_method_lookup_override_conflict_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m153_sema_method_lookup_override_conflict_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m153_lowering_method_lookup_override_conflict_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m153_validation_method_lookup_override_conflict_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m153_integration_method_lookup_override_conflict_contract.py -q",
        "npm run check:objc3c:m153-method-lookup-override-conflicts",
    ):
        assert text in tests_fragment

    assert "## M153 sema/type method lookup, override, and conflict semantics contract (M153-B001)" in sema_fragment
