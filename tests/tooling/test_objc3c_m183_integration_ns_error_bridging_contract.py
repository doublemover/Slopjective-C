import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
TESTS_DOC = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
PACKAGE_JSON = ROOT / "package.json"
COMPILER_CLOSEOUT_WORKFLOW = ROOT / ".github" / "workflows" / "compiler-closeout.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m183_integration_ns_error_bridging_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M183 integration NSError bridging contract",
        "check:objc3c:m183-ns-error-bridging-contracts",
        "check:compiler-closeout:m183",
        ".github/workflows/compiler-closeout.yml",
        "tests/tooling/test_objc3c_m183_validation_ns_error_bridging_contract.py",
        "tests/tooling/test_objc3c_m183_conformance_ns_error_bridging_contract.py",
        "tests/tooling/test_objc3c_m183_integration_ns_error_bridging_contract.py",
        "M183-A001, M183-B001, and M183-C001 outputs are not yet landed in this workspace.",
        "while remaining forward-compatible for future M183-A001/M183-B001/M183-C001 additions.",
    ):
        assert text in library_api_doc


def test_m183_e001_integration_runbook_section_is_documented() -> None:
    tests_doc = _read(TESTS_DOC)

    for text in (
        "## M183 integration NSError bridging contract runbook (M183-E001)",
        "python -m pytest tests/tooling/test_objc3c_m183_validation_ns_error_bridging_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m183_conformance_ns_error_bridging_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m183_integration_ns_error_bridging_contract.py -q",
        "npm run check:objc3c:m183-ns-error-bridging-contracts",
        "npm run check:compiler-closeout:m183",
        "Enforce M183 NSError bridging packet/docs contract",
        "Run M183 NSError bridging integration gate",
    ):
        assert text in tests_doc


def test_m183_integration_ns_error_bridging_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(COMPILER_CLOSEOUT_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m183-ns-error-bridging-contracts" in scripts
    assert scripts["check:objc3c:m183-ns-error-bridging-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m183_validation_ns_error_bridging_contract.py "
        "tests/tooling/test_objc3c_m183_conformance_ns_error_bridging_contract.py "
        "tests/tooling/test_objc3c_m183_integration_ns_error_bridging_contract.py -q"
    )

    assert "check:compiler-closeout:m183" in scripts
    assert scripts["check:compiler-closeout:m183"] == (
        "npm run check:objc3c:m183-ns-error-bridging-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m183" in scripts["check:task-hygiene"]

    assert "Enforce M183 NSError bridging packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m183" in workflow
    assert "Run M183 NSError bridging integration gate" in workflow
    assert "npm run check:objc3c:m183-ns-error-bridging-contracts" in workflow
