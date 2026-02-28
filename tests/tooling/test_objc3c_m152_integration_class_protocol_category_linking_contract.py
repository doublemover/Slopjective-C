from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m152_integration_class_protocol_category_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M152 integration class-protocol-category semantic linking",
        "check:objc3c:m152-class-protocol-category-linking",
        "tests/tooling/test_objc3c_m152_frontend_class_protocol_category_linking_contract.py",
        "tests/tooling/test_objc3c_m152_sema_class_protocol_category_linking_contract.py",
        "tests/tooling/test_objc3c_m152_lowering_class_protocol_category_linking_contract.py",
        "tests/tooling/test_objc3c_m152_validation_class_protocol_category_linking_contract.py",
        "tests/tooling/test_objc3c_m152_integration_class_protocol_category_linking_contract.py",
    ):
        assert text in library_api_doc


def test_m152_integration_class_protocol_category_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)

    assert '"check:objc3c:m152-class-protocol-category-linking"' in package_json
    assert (
        "python -m pytest tests/tooling/test_objc3c_m152_frontend_class_protocol_category_linking_contract.py "
        "tests/tooling/test_objc3c_m152_sema_class_protocol_category_linking_contract.py "
        "tests/tooling/test_objc3c_m152_lowering_class_protocol_category_linking_contract.py "
        "tests/tooling/test_objc3c_m152_validation_class_protocol_category_linking_contract.py "
        "tests/tooling/test_objc3c_m152_integration_class_protocol_category_linking_contract.py -q"
    ) in package_json

    assert "Run M152 class-protocol-category semantic-linking gate" in workflow
    assert "npm run check:objc3c:m152-class-protocol-category-linking" in workflow
