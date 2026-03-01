import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
COMPILER_CLOSEOUT_WORKFLOW = ROOT / ".github" / "workflows" / "compiler-closeout.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m185_integration_error_diagnostics_ux_recovery_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M185 integration error diagnostics UX and recovery contract",
        "check:objc3c:m185-error-diagnostics-ux-recovery-contracts",
        "check:compiler-closeout:m185",
        ".github/workflows/compiler-closeout.yml",
        "tests/tooling/test_objc3c_m185_integration_error_diagnostics_ux_recovery_contract.py",
    ):
        assert text in library_api_doc


def test_m185_integration_error_diagnostics_ux_recovery_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(COMPILER_CLOSEOUT_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m185-error-diagnostics-ux-recovery-contracts" in scripts
    assert scripts["check:objc3c:m185-error-diagnostics-ux-recovery-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m185_frontend_error_diagnostics_recovery_parser_contract.py "
        "tests/tooling/test_objc3c_m185_sema_error_diagnostics_recovery_contract.py "
        "tests/tooling/test_objc3c_m185_lowering_error_diagnostics_recovery_contract.py "
        "tests/tooling/test_objc3c_m185_validation_error_diagnostics_recovery_contract.py "
        "tests/tooling/test_objc3c_m185_conformance_error_diagnostics_recovery_contract.py "
        "tests/tooling/test_objc3c_m185_integration_error_diagnostics_ux_recovery_contract.py -q"
    )

    assert "check:compiler-closeout:m185" in scripts
    assert scripts["check:compiler-closeout:m185"] == (
        "npm run check:objc3c:m185-error-diagnostics-ux-recovery-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m185" in scripts["check:task-hygiene"]

    assert "Enforce M185 error diagnostics/recovery packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m185" in workflow
    assert "Run M185 error diagnostics/recovery integration gate" in workflow
    assert "npm run check:objc3c:m185-error-diagnostics-ux-recovery-contracts" in workflow

    assert workflow.index("Run M184 unwind safety/cleanup emission integration gate") < workflow.index(
        "Enforce M185 error diagnostics/recovery packet/docs contract"
    )
    assert workflow.index("Enforce M185 error diagnostics/recovery packet/docs contract") < workflow.index(
        "Run M185 error diagnostics/recovery integration gate"
    )
    assert workflow.index("Run M185 error diagnostics/recovery integration gate") < workflow.index(
        "Enforce M186 async grammar/continuation IR packet/docs contract"
    )
