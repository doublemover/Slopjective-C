import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m159_integration_super_dispatch_method_family_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M159 integration super-dispatch and method-family semantics contract",
        "check:objc3c:m159-super-dispatch-method-family-contracts",
        "check:compiler-closeout:m159",
        "tests/tooling/test_objc3c_m159_frontend_super_dispatch_method_family_contract.py",
        "tests/tooling/test_objc3c_m159_sema_super_dispatch_method_family_contract.py",
        "tests/tooling/test_objc3c_m159_lowering_super_dispatch_method_family_contract.py",
        "tests/tooling/test_objc3c_m159_validation_super_dispatch_method_family_contract.py",
        "tests/tooling/test_objc3c_m159_integration_super_dispatch_method_family_contract.py",
    ):
        assert text in library_api_doc


def test_m159_integration_super_dispatch_method_family_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m159-super-dispatch-method-family-contracts" in scripts
    assert scripts["check:objc3c:m159-super-dispatch-method-family-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m159_frontend_super_dispatch_method_family_contract.py "
        "tests/tooling/test_objc3c_m159_sema_super_dispatch_method_family_contract.py "
        "tests/tooling/test_objc3c_m159_lowering_super_dispatch_method_family_contract.py "
        "tests/tooling/test_objc3c_m159_validation_super_dispatch_method_family_contract.py "
        "tests/tooling/test_objc3c_m159_integration_super_dispatch_method_family_contract.py -q"
    )

    assert "check:compiler-closeout:m159" in scripts
    assert scripts["check:compiler-closeout:m159"] == (
        "npm run check:objc3c:m159-super-dispatch-method-family-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m159" in scripts["check:task-hygiene"]

    assert "Enforce M159 super-dispatch and method-family packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m159" in workflow
    assert "Run M159 super-dispatch and method-family integration gate" in workflow
    assert "npm run check:objc3c:m159-super-dispatch-method-family-contracts" in workflow
