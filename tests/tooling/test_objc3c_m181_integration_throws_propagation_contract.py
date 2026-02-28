import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
TESTS_DOC = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
PACKAGE_JSON = ROOT / "package.json"
COMPILER_CLOSEOUT_WORKFLOW = ROOT / ".github" / "workflows" / "compiler-closeout.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m181_integration_throws_propagation_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M181 integration throws propagation contract",
        "check:objc3c:m181-throws-propagation-contracts",
        "check:compiler-closeout:m181",
        ".github/workflows/compiler-closeout.yml",
        "tests/tooling/test_objc3c_m181_frontend_throws_parser_contract.py",
        "tests/tooling/test_objc3c_m181_lowering_throws_propagation_contract.py",
        "tests/tooling/test_objc3c_m181_validation_throws_propagation_contract.py",
        "tests/tooling/test_objc3c_m181_conformance_throws_propagation_contract.py",
        "tests/tooling/test_objc3c_m181_integration_throws_propagation_contract.py",
        "M181-A001, M181-C001, and M181-D001 outputs are landed in this workspace.",
        "A standalone M181-B001 sema contract test is not present in this workspace.",
    ):
        assert text in library_api_doc


def test_m181_e001_integration_runbook_section_is_documented() -> None:
    tests_doc = _read(TESTS_DOC)

    for text in (
        "## M181 integration throws propagation contract runbook (M181-E001)",
        "python -m pytest tests/tooling/test_objc3c_m181_frontend_throws_parser_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m181_lowering_throws_propagation_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m181_validation_throws_propagation_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m181_conformance_throws_propagation_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m181_integration_throws_propagation_contract.py -q",
        "npm run check:objc3c:m181-throws-propagation-contracts",
        "npm run check:compiler-closeout:m181",
        "Enforce M181 throws propagation packet/docs contract",
        "Run M181 throws propagation integration gate",
    ):
        assert text in tests_doc


def test_m181_integration_throws_propagation_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(COMPILER_CLOSEOUT_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m181-throws-propagation-contracts" in scripts
    assert scripts["check:objc3c:m181-throws-propagation-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m181_frontend_throws_parser_contract.py "
        "tests/tooling/test_objc3c_m181_lowering_throws_propagation_contract.py "
        "tests/tooling/test_objc3c_m181_validation_throws_propagation_contract.py "
        "tests/tooling/test_objc3c_m181_conformance_throws_propagation_contract.py "
        "tests/tooling/test_objc3c_m181_integration_throws_propagation_contract.py -q"
    )

    assert "check:compiler-closeout:m181" in scripts
    assert scripts["check:compiler-closeout:m181"] == (
        "npm run check:objc3c:m181-throws-propagation-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m181" in scripts["check:task-hygiene"]

    assert "Enforce M181 throws propagation packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m181" in workflow
    assert "Run M181 throws propagation integration gate" in workflow
    assert "npm run check:objc3c:m181-throws-propagation-contracts" in workflow
