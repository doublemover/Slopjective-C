from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m149_integration_property_attribute_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M149 integration @property grammar and attribute parsing",
        "check:objc3c:m149-property-attributes",
        "tests/tooling/test_objc3c_m149_frontend_property_attribute_contract.py",
        "tests/tooling/test_objc3c_m149_sema_property_attribute_contract.py",
        "tests/tooling/test_objc3c_m149_lowering_property_attribute_contract.py",
        "tests/tooling/test_objc3c_m149_validation_property_attribute_contract.py",
        "tests/tooling/test_objc3c_m149_integration_property_attribute_contract.py",
    ):
        assert text in library_api_doc


def test_m149_integration_property_attribute_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)

    assert '"check:objc3c:m149-property-attributes"' in package_json
    assert (
        "python -m pytest tests/tooling/test_objc3c_m149_frontend_property_attribute_contract.py "
        "tests/tooling/test_objc3c_m149_sema_property_attribute_contract.py "
        "tests/tooling/test_objc3c_m149_lowering_property_attribute_contract.py "
        "tests/tooling/test_objc3c_m149_validation_property_attribute_contract.py "
        "tests/tooling/test_objc3c_m149_integration_property_attribute_contract.py -q"
    ) in package_json

    assert "Run M149 @property grammar and attribute parsing gate" in workflow
    assert "npm run check:objc3c:m149-property-attributes" in workflow
