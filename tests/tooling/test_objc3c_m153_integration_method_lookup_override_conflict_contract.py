import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m153_integration_method_lookup_override_conflict_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M153 integration method lookup override conflict contract",
        "check:objc3c:m153-method-lookup-override-conflicts",
        "check:compiler-closeout:m153",
        "tests/tooling/test_objc3c_m153_frontend_method_lookup_override_conflict_contract.py",
        "tests/tooling/test_objc3c_m153_sema_method_lookup_override_conflict_contract.py",
        "tests/tooling/test_objc3c_m153_lowering_method_lookup_override_conflict_contract.py",
        "tests/tooling/test_objc3c_m153_validation_method_lookup_override_conflict_contract.py",
        "tests/tooling/test_objc3c_m153_integration_method_lookup_override_conflict_contract.py",
    ):
        assert text in library_api_doc


def test_m153_integration_method_lookup_override_conflict_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m153-method-lookup-override-conflicts" in scripts
    assert scripts["check:objc3c:m153-method-lookup-override-conflicts"] == (
        "python -m pytest tests/tooling/test_objc3c_m153_frontend_method_lookup_override_conflict_contract.py "
        "tests/tooling/test_objc3c_m153_sema_method_lookup_override_conflict_contract.py "
        "tests/tooling/test_objc3c_m153_lowering_method_lookup_override_conflict_contract.py "
        "tests/tooling/test_objc3c_m153_validation_method_lookup_override_conflict_contract.py "
        "tests/tooling/test_objc3c_m153_integration_method_lookup_override_conflict_contract.py -q"
    )

    assert "check:compiler-closeout:m153" in scripts
    assert scripts["check:compiler-closeout:m153"] == (
        "npm run check:objc3c:m153-method-lookup-override-conflicts && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m153" in scripts["check:task-hygiene"]

    assert "Enforce M153 method-lookup/override-conflict packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m153" in workflow
    assert "Run M153 method-lookup/override-conflict integration gate" in workflow
    assert "npm run check:objc3c:m153-method-lookup-override-conflicts" in workflow
