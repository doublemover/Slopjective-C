import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m155_integration_id_class_sel_object_pointer_typecheck_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M155 integration id/class/SEL/object-pointer typecheck contract",
        "check:objc3c:m155-id-class-sel-object-pointer-typecheck-contracts",
        "check:compiler-closeout:m155",
        "tests/tooling/test_objc3c_m155_frontend_id_class_sel_object_pointer_typecheck_contract.py",
        "tests/tooling/test_objc3c_m155_sema_id_class_sel_object_pointer_typecheck_contract.py",
        "tests/tooling/test_objc3c_m155_lowering_id_class_sel_object_pointer_typecheck_contract.py",
        "tests/tooling/test_objc3c_m155_validation_id_class_sel_object_pointer_typecheck_contract.py",
        "tests/tooling/test_objc3c_m155_integration_id_class_sel_object_pointer_typecheck_contract.py",
    ):
        assert text in library_api_doc


def test_m155_integration_id_class_sel_object_pointer_typecheck_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m155-id-class-sel-object-pointer-typecheck-contracts" in scripts
    assert scripts["check:objc3c:m155-id-class-sel-object-pointer-typecheck-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m155_frontend_id_class_sel_object_pointer_typecheck_contract.py "
        "tests/tooling/test_objc3c_m155_sema_id_class_sel_object_pointer_typecheck_contract.py "
        "tests/tooling/test_objc3c_m155_lowering_id_class_sel_object_pointer_typecheck_contract.py "
        "tests/tooling/test_objc3c_m155_validation_id_class_sel_object_pointer_typecheck_contract.py "
        "tests/tooling/test_objc3c_m155_integration_id_class_sel_object_pointer_typecheck_contract.py -q"
    )

    assert "check:compiler-closeout:m155" in scripts
    assert scripts["check:compiler-closeout:m155"] == (
        "npm run check:objc3c:m155-id-class-sel-object-pointer-typecheck-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m155" in scripts["check:task-hygiene"]

    assert "Enforce M155 id/class/SEL/object-pointer typecheck packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m155" in workflow
    assert "Run M155 id/class/SEL/object-pointer typecheck integration gate" in workflow
    assert "npm run check:objc3c:m155-id-class-sel-object-pointer-typecheck-contracts" in workflow
