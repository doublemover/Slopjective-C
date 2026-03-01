import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
TESTS_DOC = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
PACKAGE_JSON = ROOT / "package.json"
COMPILER_CLOSEOUT_WORKFLOW = ROOT / ".github" / "workflows" / "compiler-closeout.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m189_integration_task_runtime_interop_cancellation_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M189 integration task runtime interop and cancellation contract",
        "check:objc3c:m189-task-runtime-interop-cancellation-contracts",
        "check:compiler-closeout:m189",
        ".github/workflows/compiler-closeout.yml",
        "tests/tooling/test_objc3c_m195_frontend_system_extension_policy_contract.py",
        "tests/tooling/test_objc3c_m195_sema_system_extension_policy_contract.py",
        "tests/tooling/test_objc3c_m189_sema_task_runtime_interop_cancellation_contract.py",
        "tests/tooling/test_objc3c_m189_lowering_task_runtime_interop_cancellation_contract.py",
        "tests/tooling/test_objc3c_m189_validation_task_runtime_interop_cancellation_contract.py",
        "tests/tooling/test_objc3c_m189_conformance_task_runtime_interop_cancellation_contract.py",
        "tests/tooling/test_objc3c_m189_integration_task_runtime_interop_cancellation_contract.py",
        "M189-A001, M189-B001, M189-C001, and M189-D001 packet-specific artifacts are landed in this workspace.",
        "This M189-E001 gate deterministically replays currently landed low-level lane surfaces via M195 frontend/sema contracts plus M189-B001 sema, M189-C001 lowering, and M189-D001 validation/conformance packets.",
    ):
        assert text in library_api_doc


def test_m189_e001_integration_runbook_section_is_documented() -> None:
    tests_doc = _read(TESTS_DOC)

    for text in (
        "## M189 integration task runtime interop and cancellation contract runbook (M189-E001)",
        "python -m pytest tests/tooling/test_objc3c_m195_frontend_system_extension_policy_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m195_sema_system_extension_policy_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m189_sema_task_runtime_interop_cancellation_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m189_lowering_task_runtime_interop_cancellation_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m189_validation_task_runtime_interop_cancellation_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m189_conformance_task_runtime_interop_cancellation_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m189_integration_task_runtime_interop_cancellation_contract.py -q",
        "npm run check:objc3c:m189-task-runtime-interop-cancellation-contracts",
        "npm run check:compiler-closeout:m189",
        "Enforce M189 task-runtime interop/cancellation packet/docs contract",
        "Run M189 task-runtime interop/cancellation integration gate",
    ):
        assert text in tests_doc


def test_m189_integration_task_runtime_interop_cancellation_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(COMPILER_CLOSEOUT_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m189-task-runtime-interop-cancellation-contracts" in scripts
    assert scripts["check:objc3c:m189-task-runtime-interop-cancellation-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m195_frontend_system_extension_policy_contract.py "
        "tests/tooling/test_objc3c_m195_sema_system_extension_policy_contract.py "
        "tests/tooling/test_objc3c_m189_sema_task_runtime_interop_cancellation_contract.py "
        "tests/tooling/test_objc3c_m189_lowering_task_runtime_interop_cancellation_contract.py "
        "tests/tooling/test_objc3c_m189_validation_task_runtime_interop_cancellation_contract.py "
        "tests/tooling/test_objc3c_m189_conformance_task_runtime_interop_cancellation_contract.py "
        "tests/tooling/test_objc3c_m189_integration_task_runtime_interop_cancellation_contract.py -q"
    )

    assert "check:compiler-closeout:m189" in scripts
    assert scripts["check:compiler-closeout:m189"] == (
        "npm run check:objc3c:m189-task-runtime-interop-cancellation-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m189" in scripts["check:task-hygiene"]

    assert "Enforce M189 task-runtime interop/cancellation packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m189" in workflow
    assert "Run M189 task-runtime interop/cancellation integration gate" in workflow
    assert "npm run check:objc3c:m189-task-runtime-interop-cancellation-contracts" in workflow

    assert workflow.index("Run M183 NSError bridging integration gate") < workflow.index(
        "Enforce M189 task-runtime interop/cancellation packet/docs contract"
    )
    assert workflow.index("Enforce M189 task-runtime interop/cancellation packet/docs contract") < workflow.index(
        "Run M189 task-runtime interop/cancellation integration gate"
    )
    assert workflow.index("Run M189 task-runtime interop/cancellation integration gate") < workflow.index(
        "Enforce M190 concurrency replay/race-guard packet/docs contract"
    )
