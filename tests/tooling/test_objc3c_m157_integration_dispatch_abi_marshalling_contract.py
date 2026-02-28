import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m157_integration_dispatch_abi_marshalling_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M157 integration dispatch ABI marshalling contract",
        "check:objc3c:m157-dispatch-abi-marshalling-contracts",
        "check:compiler-closeout:m157",
        "tests/tooling/test_objc3c_m157_frontend_dispatch_abi_marshalling_contract.py",
        "tests/tooling/test_objc3c_m157_sema_dispatch_abi_marshalling_contract.py",
        "tests/tooling/test_objc3c_m157_lowering_dispatch_abi_marshalling_contract.py",
        "tests/tooling/test_objc3c_m157_validation_dispatch_abi_marshalling_contract.py",
        "tests/tooling/test_objc3c_m157_integration_dispatch_abi_marshalling_contract.py",
    ):
        assert text in library_api_doc


def test_m157_integration_dispatch_abi_marshalling_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m157-dispatch-abi-marshalling-contracts" in scripts
    assert scripts["check:objc3c:m157-dispatch-abi-marshalling-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m157_frontend_dispatch_abi_marshalling_contract.py "
        "tests/tooling/test_objc3c_m157_sema_dispatch_abi_marshalling_contract.py "
        "tests/tooling/test_objc3c_m157_lowering_dispatch_abi_marshalling_contract.py "
        "tests/tooling/test_objc3c_m157_validation_dispatch_abi_marshalling_contract.py "
        "tests/tooling/test_objc3c_m157_integration_dispatch_abi_marshalling_contract.py -q"
    )

    assert "check:compiler-closeout:m157" in scripts
    assert scripts["check:compiler-closeout:m157"] == (
        "npm run check:objc3c:m157-dispatch-abi-marshalling-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m157" in scripts["check:task-hygiene"]

    assert "Enforce M157 dispatch ABI marshalling packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m157" in workflow
    assert "Run M157 dispatch ABI marshalling integration gate" in workflow
    assert "npm run check:objc3c:m157-dispatch-abi-marshalling-contracts" in workflow
