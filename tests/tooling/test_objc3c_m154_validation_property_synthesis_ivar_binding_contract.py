from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TESTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m154_validation_property_synthesis_ivar_binding_runbook_exists() -> None:
    tests_fragment = _read(TESTS_DOC_FRAGMENT)
    sema_fragment = _read(SEMA_DOC_FRAGMENT)
    artifacts_fragment = _read(ARTIFACTS_DOC_FRAGMENT)

    for text in (
        "## M154 validation property synthesis and ivar binding semantics runbook",
        "python -m pytest tests/tooling/test_objc3c_m154_frontend_property_synthesis_ivar_binding_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m154_sema_property_synthesis_ivar_binding_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m154_lowering_property_synthesis_ivar_binding_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m154_validation_property_synthesis_ivar_binding_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m154_integration_property_synthesis_ivar_binding_contract.py -q",
        "npm run check:objc3c:m154-property-synthesis-ivar-bindings",
    ):
        assert text in tests_fragment

    assert "## M154 sema/type property synthesis and ivar binding semantics contract (M154-B001)" in sema_fragment
    assert "## Property synthesis/ivar binding lowering artifact contract (M154-C001)" in artifacts_fragment
