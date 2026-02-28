import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m160_integration_runtime_shim_host_link_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M160 integration runtime-shim host-link semantics contract",
        "check:objc3c:m160-runtime-shim-host-link-contracts",
        "check:compiler-closeout:m160",
        "tests/tooling/test_objc3c_m160_frontend_runtime_shim_host_link_contract.py",
        "tests/tooling/test_objc3c_m160_sema_runtime_shim_host_link_contract.py",
        "tests/tooling/test_objc3c_m160_lowering_runtime_shim_host_link_contract.py",
        "tests/tooling/test_objc3c_m160_validation_runtime_shim_host_link_contract.py",
        "tests/tooling/test_objc3c_m160_integration_runtime_shim_host_link_contract.py",
    ):
        assert text in library_api_doc


def test_m160_integration_runtime_shim_host_link_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m160-runtime-shim-host-link-contracts" in scripts
    assert scripts["check:objc3c:m160-runtime-shim-host-link-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m160_frontend_runtime_shim_host_link_contract.py "
        "tests/tooling/test_objc3c_m160_sema_runtime_shim_host_link_contract.py "
        "tests/tooling/test_objc3c_m160_lowering_runtime_shim_host_link_contract.py "
        "tests/tooling/test_objc3c_m160_validation_runtime_shim_host_link_contract.py "
        "tests/tooling/test_objc3c_m160_integration_runtime_shim_host_link_contract.py -q"
    )

    assert "check:compiler-closeout:m160" in scripts
    assert scripts["check:compiler-closeout:m160"] == (
        "npm run check:objc3c:m160-runtime-shim-host-link-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m160" in scripts["check:task-hygiene"]

    assert "Enforce M160 runtime-shim host-link packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m160" in workflow
    assert "Run M160 runtime-shim host-link integration gate" in workflow
    assert "npm run check:objc3c:m160-runtime-shim-host-link-contracts" in workflow
