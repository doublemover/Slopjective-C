import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
COMPILER_CLOSEOUT_WORKFLOW = ROOT / ".github" / "workflows" / "compiler-closeout.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m191_integration_unsafe_pointer_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M191 integration unsafe-pointer extension gating contract",
        "check:objc3c:m191-unsafe-pointer-contracts",
        "check:compiler-closeout:m191",
        ".github/workflows/compiler-closeout.yml",
        "tests/tooling/test_objc3c_m195_frontend_system_extension_policy_contract.py",
        "tests/tooling/test_objc3c_m195_sema_system_extension_policy_contract.py",
        "tests/tooling/test_objc3c_m195_lowering_system_extension_policy_contract.py",
        "tests/tooling/test_objc3c_m195_validation_system_extension_policy_contract.py",
        "tests/tooling/test_objc3c_m191_integration_unsafe_pointer_contract.py",
        "M191-A001 through M191-D001 packet-specific artifacts are not landed in this workspace as of this wiring change.",
        "This initial M191-E001 gate deterministically replays currently landed low-level lane surfaces via M195 frontend/sema/lowering/validation contracts.",
    ):
        assert text in library_api_doc


def test_m191_integration_unsafe_pointer_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(COMPILER_CLOSEOUT_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m191-unsafe-pointer-contracts" in scripts
    assert scripts["check:objc3c:m191-unsafe-pointer-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m195_frontend_system_extension_policy_contract.py "
        "tests/tooling/test_objc3c_m195_sema_system_extension_policy_contract.py "
        "tests/tooling/test_objc3c_m195_lowering_system_extension_policy_contract.py "
        "tests/tooling/test_objc3c_m195_validation_system_extension_policy_contract.py "
        "tests/tooling/test_objc3c_m191_integration_unsafe_pointer_contract.py -q"
    )

    assert "check:compiler-closeout:m191" in scripts
    assert scripts["check:compiler-closeout:m191"] == (
        "npm run check:objc3c:m191-unsafe-pointer-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m191" in scripts["check:task-hygiene"]

    assert "Enforce M191 unsafe-pointer packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m191" in workflow
    assert "Run M191 unsafe-pointer integration gate" in workflow
    assert "npm run check:objc3c:m191-unsafe-pointer-contracts" in workflow

    assert workflow.index("Enforce M191 unsafe-pointer packet/docs contract") < workflow.index(
        "Run M191 unsafe-pointer integration gate"
    )
    assert workflow.index("npm run check:compiler-closeout:m191") < workflow.index(
        "npm run check:objc3c:m191-unsafe-pointer-contracts"
    )
