from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m146_integration_interface_implementation_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M146 integration @interface/@implementation grammar",
        "check:objc3c:m146-interface-implementation",
        "tests/tooling/test_objc3c_m146_frontend_interface_implementation_contract.py",
        "tests/tooling/test_objc3c_m146_sema_interface_implementation_contract.py",
        "tests/tooling/test_objc3c_m146_lowering_interface_implementation_contract.py",
        "tests/tooling/test_objc3c_m146_validation_interface_implementation_contract.py",
        "tests/tooling/test_objc3c_m146_integration_interface_implementation_contract.py",
    ):
        assert text in library_api_doc


def test_m146_integration_interface_implementation_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)

    assert '"check:objc3c:m146-interface-implementation"' in package_json
    assert (
        "python -m pytest tests/tooling/test_objc3c_m146_frontend_interface_implementation_contract.py "
        "tests/tooling/test_objc3c_m146_sema_interface_implementation_contract.py "
        "tests/tooling/test_objc3c_m146_lowering_interface_implementation_contract.py "
        "tests/tooling/test_objc3c_m146_validation_interface_implementation_contract.py "
        "tests/tooling/test_objc3c_m146_integration_interface_implementation_contract.py -q"
    ) in package_json

    assert "Run M146 @interface/@implementation grammar gate" in workflow
    assert "npm run check:objc3c:m146-interface-implementation" in workflow
