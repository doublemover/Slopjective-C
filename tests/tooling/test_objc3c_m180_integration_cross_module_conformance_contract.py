import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
TESTS_DOC = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
PACKAGE_JSON = ROOT / "package.json"
COMPILER_CLOSEOUT_WORKFLOW = ROOT / ".github" / "workflows" / "compiler-closeout.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m180_integration_cross_module_conformance_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M180 integration cross-module conformance contract",
        "check:objc3c:m180-cross-module-conformance-contracts",
        "check:compiler-closeout:m180",
        ".github/workflows/compiler-closeout.yml",
        "tests/tooling/test_objc3c_m180_sema_cross_module_conformance_contract.py",
        "tests/tooling/test_objc3c_m180_integration_cross_module_conformance_contract.py",
        "M180-A001, M180-C001, and M180-D001 outputs are not yet landed in this workspace.",
        "while remaining forward-compatible for future M180-A001/M180-C001/M180-D001 additions.",
    ):
        assert text in library_api_doc


def test_m180_e001_integration_runbook_section_is_documented() -> None:
    tests_doc = _read(TESTS_DOC)

    for text in (
        "## M180 integration cross-module conformance contract runbook (M180-E001)",
        "python -m pytest tests/tooling/test_objc3c_m180_sema_cross_module_conformance_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m180_integration_cross_module_conformance_contract.py -q",
        "npm run check:objc3c:m180-cross-module-conformance-contracts",
        "npm run check:compiler-closeout:m180",
        "Enforce M180 cross-module conformance packet/docs contract",
        "Run M180 cross-module conformance integration gate",
    ):
        assert text in tests_doc


def test_m180_integration_cross_module_conformance_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(COMPILER_CLOSEOUT_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m180-cross-module-conformance-contracts" in scripts
    assert scripts["check:objc3c:m180-cross-module-conformance-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m180_sema_cross_module_conformance_contract.py "
        "tests/tooling/test_objc3c_m180_integration_cross_module_conformance_contract.py -q"
    )

    assert "check:compiler-closeout:m180" in scripts
    assert scripts["check:compiler-closeout:m180"] == (
        "npm run check:objc3c:m180-cross-module-conformance-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m180" in scripts["check:task-hygiene"]

    assert "Enforce M180 cross-module conformance packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m180" in workflow
    assert "Run M180 cross-module conformance integration gate" in workflow
    assert "npm run check:objc3c:m180-cross-module-conformance-contracts" in workflow
