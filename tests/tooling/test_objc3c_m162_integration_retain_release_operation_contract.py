import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m162_integration_retain_release_operation_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M162 integration retain-release operation semantics contract",
        "check:objc3c:m162-retain-release-operation-contracts",
        "check:compiler-closeout:m162",
        "tests/tooling/test_objc3c_m162_frontend_retain_release_parser_contract.py",
        "tests/tooling/test_objc3c_m162_sema_retain_release_contract.py",
        "tests/tooling/test_objc3c_m162_lowering_retain_release_operation_contract.py",
        "tests/tooling/test_objc3c_m162_validation_retain_release_operation_contract.py",
        "tests/tooling/test_objc3c_m162_integration_retain_release_operation_contract.py",
    ):
        assert text in library_api_doc


def test_m162_integration_retain_release_operation_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m162-retain-release-operation-contracts" in scripts
    assert scripts["check:objc3c:m162-retain-release-operation-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m162_frontend_retain_release_parser_contract.py "
        "tests/tooling/test_objc3c_m162_sema_retain_release_contract.py "
        "tests/tooling/test_objc3c_m162_lowering_retain_release_operation_contract.py "
        "tests/tooling/test_objc3c_m162_validation_retain_release_operation_contract.py "
        "tests/tooling/test_objc3c_m162_integration_retain_release_operation_contract.py -q"
    )

    assert "check:compiler-closeout:m162" in scripts
    assert scripts["check:compiler-closeout:m162"] == (
        "npm run check:objc3c:m162-retain-release-operation-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m162" in scripts["check:task-hygiene"]

    assert "Enforce M162 retain-release operation packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m162" in workflow
    assert "Run M162 retain-release operation integration gate" in workflow
    assert "npm run check:objc3c:m162-retain-release-operation-contracts" in workflow
