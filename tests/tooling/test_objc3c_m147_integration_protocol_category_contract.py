from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m147_integration_protocol_category_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M147 integration @protocol/@category grammar",
        "check:objc3c:m147-protocol-category",
        "tests/tooling/test_objc3c_m147_frontend_protocol_category_contract.py",
        "tests/tooling/test_objc3c_m147_sema_protocol_category_contract.py",
        "tests/tooling/test_objc3c_m147_lowering_protocol_category_contract.py",
        "tests/tooling/test_objc3c_m147_validation_protocol_category_contract.py",
        "tests/tooling/test_objc3c_m147_integration_protocol_category_contract.py",
    ):
        assert text in library_api_doc


def test_m147_integration_protocol_category_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)

    assert '"check:objc3c:m147-protocol-category"' in package_json
    assert (
        "python -m pytest tests/tooling/test_objc3c_m147_frontend_protocol_category_contract.py "
        "tests/tooling/test_objc3c_m147_sema_protocol_category_contract.py "
        "tests/tooling/test_objc3c_m147_lowering_protocol_category_contract.py "
        "tests/tooling/test_objc3c_m147_validation_protocol_category_contract.py "
        "tests/tooling/test_objc3c_m147_integration_protocol_category_contract.py -q"
    ) in package_json

    assert "Run M147 @protocol/@category grammar gate" in workflow
    assert "npm run check:objc3c:m147-protocol-category" in workflow
