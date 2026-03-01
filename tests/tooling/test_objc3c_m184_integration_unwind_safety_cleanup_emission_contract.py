import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
TESTS_DOC = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
PACKAGE_JSON = ROOT / "package.json"
COMPILER_CLOSEOUT_WORKFLOW = ROOT / ".github" / "workflows" / "compiler-closeout.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m184_integration_unwind_safety_cleanup_emission_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M184 integration unwind safety and cleanup emission contract",
        "check:objc3c:m184-unwind-safety-cleanup-emission-contracts",
        "check:compiler-closeout:m184",
        ".github/workflows/compiler-closeout.yml",
        "tests/tooling/test_objc3c_m183_frontend_ns_error_bridging_parser_contract.py",
        "tests/tooling/test_objc3c_m183_lowering_ns_error_bridging_contract.py",
        "tests/tooling/test_objc3c_m183_validation_ns_error_bridging_contract.py",
        "tests/tooling/test_objc3c_m183_conformance_ns_error_bridging_contract.py",
        "tests/tooling/test_objc3c_m184_integration_unwind_safety_cleanup_emission_contract.py",
        "M184-A001, M184-B001, M184-C001, and M184-D001 packet-specific artifacts are not landed in this workspace as of this wiring change.",
        "This initial M184-E001 gate deterministically replays currently landed lane surfaces via M183-A001 frontend parsing, M183-B001 fail-closed sema replay anchors from M183-D001, M183-C001 lowering replay anchors, and the M183-D001 validation/conformance packet.",
        "M184-A001 parser surfaces, M184-B001 sema surfaces, M184-C001 lowering surfaces, and M184-D001 validation/conformance surfaces are fail-closed via M183-E001 replay anchors in this integration gate.",
        "The integration gate fail-closes on these currently landed lane surfaces plus this M184-E001 wiring contract.",
    ):
        assert text in library_api_doc


def test_m184_e001_integration_runbook_section_is_documented() -> None:
    tests_doc = _read(TESTS_DOC)

    for text in (
        "## M184 integration unwind safety and cleanup emission contract runbook (M184-E001)",
        "python -m pytest tests/tooling/test_objc3c_m183_frontend_ns_error_bridging_parser_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m183_lowering_ns_error_bridging_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m183_validation_ns_error_bridging_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m183_conformance_ns_error_bridging_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m184_integration_unwind_safety_cleanup_emission_contract.py -q",
        "npm run check:objc3c:m184-unwind-safety-cleanup-emission-contracts",
        "npm run check:compiler-closeout:m184",
        "Enforce M184 unwind safety/cleanup emission packet/docs contract",
        "Run M184 unwind safety/cleanup emission integration gate",
    ):
        assert text in tests_doc


def test_m184_integration_unwind_safety_cleanup_emission_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(COMPILER_CLOSEOUT_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m184-unwind-safety-cleanup-emission-contracts" in scripts
    assert scripts["check:objc3c:m184-unwind-safety-cleanup-emission-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m183_frontend_ns_error_bridging_parser_contract.py "
        "tests/tooling/test_objc3c_m183_lowering_ns_error_bridging_contract.py "
        "tests/tooling/test_objc3c_m183_validation_ns_error_bridging_contract.py "
        "tests/tooling/test_objc3c_m183_conformance_ns_error_bridging_contract.py "
        "tests/tooling/test_objc3c_m184_integration_unwind_safety_cleanup_emission_contract.py -q"
    )

    assert "check:compiler-closeout:m184" in scripts
    assert scripts["check:compiler-closeout:m184"] == (
        "npm run check:objc3c:m184-unwind-safety-cleanup-emission-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m184" in scripts["check:task-hygiene"]

    assert "Enforce M184 unwind safety/cleanup emission packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m184" in workflow
    assert "Run M184 unwind safety/cleanup emission integration gate" in workflow
    assert "npm run check:objc3c:m184-unwind-safety-cleanup-emission-contracts" in workflow

    assert workflow.index("Run M183 NSError bridging integration gate") < workflow.index(
        "Enforce M184 unwind safety/cleanup emission packet/docs contract"
    )
    assert workflow.index("Enforce M184 unwind safety/cleanup emission packet/docs contract") < workflow.index(
        "Run M184 unwind safety/cleanup emission integration gate"
    )
    assert workflow.index("Run M184 unwind safety/cleanup emission integration gate") < workflow.index(
        "Enforce M186 async grammar/continuation IR packet/docs contract"
    )
