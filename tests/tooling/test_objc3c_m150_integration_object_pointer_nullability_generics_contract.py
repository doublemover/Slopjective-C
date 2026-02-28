from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m150_integration_object_pointer_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M150 integration object-pointer declarators, nullability, lightweight generics parse",
        "check:objc3c:m150-object-pointer-nullability-generics",
        "tests/tooling/test_objc3c_m150_frontend_object_pointer_nullability_generics_contract.py",
        "tests/tooling/test_objc3c_m150_sema_object_pointer_nullability_generics_contract.py",
        "tests/tooling/test_objc3c_m150_lowering_object_pointer_nullability_generics_contract.py",
        "tests/tooling/test_objc3c_m150_validation_object_pointer_nullability_generics_contract.py",
        "tests/tooling/test_objc3c_m150_integration_object_pointer_nullability_generics_contract.py",
    ):
        assert text in library_api_doc


def test_m150_integration_object_pointer_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)

    assert '"check:objc3c:m150-object-pointer-nullability-generics"' in package_json
    assert (
        "python -m pytest tests/tooling/test_objc3c_m150_frontend_object_pointer_nullability_generics_contract.py "
        "tests/tooling/test_objc3c_m150_sema_object_pointer_nullability_generics_contract.py "
        "tests/tooling/test_objc3c_m150_lowering_object_pointer_nullability_generics_contract.py "
        "tests/tooling/test_objc3c_m150_validation_object_pointer_nullability_generics_contract.py "
        "tests/tooling/test_objc3c_m150_integration_object_pointer_nullability_generics_contract.py -q"
    ) in package_json

    assert "Run M150 object-pointer/nullability/generics grammar gate" in workflow
    assert "npm run check:objc3c:m150-object-pointer-nullability-generics" in workflow
