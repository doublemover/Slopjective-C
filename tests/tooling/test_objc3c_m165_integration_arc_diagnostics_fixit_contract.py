import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m165_integration_arc_diagnostics_fixit_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M165 integration ARC diagnostics/fix-it contract",
        "check:objc3c:m165-arc-diagnostics-fixit-contracts",
        "check:compiler-closeout:m165",
        "tests/tooling/test_objc3c_m165_frontend_arc_diagnostics_fixit_parser_contract.py",
        "tests/tooling/test_objc3c_m165_sema_arc_diagnostics_fixit_contract.py",
        "tests/tooling/test_objc3c_m165_lowering_arc_diagnostics_fixit_contract.py",
        "tests/tooling/test_objc3c_m165_validation_arc_diagnostics_fixit_contract.py",
        "tests/tooling/test_objc3c_m165_integration_arc_diagnostics_fixit_contract.py",
    ):
        assert text in library_api_doc


def test_m165_integration_arc_diagnostics_fixit_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m165-arc-diagnostics-fixit-contracts" in scripts
    assert scripts["check:objc3c:m165-arc-diagnostics-fixit-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m165_frontend_arc_diagnostics_fixit_parser_contract.py "
        "tests/tooling/test_objc3c_m165_sema_arc_diagnostics_fixit_contract.py "
        "tests/tooling/test_objc3c_m165_lowering_arc_diagnostics_fixit_contract.py "
        "tests/tooling/test_objc3c_m165_validation_arc_diagnostics_fixit_contract.py "
        "tests/tooling/test_objc3c_m165_integration_arc_diagnostics_fixit_contract.py -q"
    )

    assert "check:compiler-closeout:m165" in scripts
    assert scripts["check:compiler-closeout:m165"] == (
        "npm run check:objc3c:m165-arc-diagnostics-fixit-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m165" in scripts["check:task-hygiene"]

    assert "Enforce M165 ARC diagnostics/fix-it packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m165" in workflow
    assert "Run M165 ARC diagnostics/fix-it integration gate" in workflow
    assert "npm run check:objc3c:m165-arc-diagnostics-fixit-contracts" in workflow
