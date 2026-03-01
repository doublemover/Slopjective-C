import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
TESTS_DOC = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
PACKAGE_JSON = ROOT / "package.json"
COMPILER_CLOSEOUT_WORKFLOW = ROOT / ".github" / "workflows" / "compiler-closeout.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m188_integration_actor_isolation_sendability_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M188 integration actor isolation and sendability contract",
        "check:objc3c:m188-actor-isolation-sendability-contracts",
        "check:compiler-closeout:m188",
        ".github/workflows/compiler-closeout.yml",
        "tests/tooling/test_objc3c_m188_frontend_actor_isolation_sendability_parser_contract.py",
        "tests/tooling/test_objc3c_m188_validation_actor_isolation_sendability_contract.py",
        "tests/tooling/test_objc3c_m188_conformance_actor_isolation_sendability_contract.py",
        "tests/tooling/test_objc3c_m188_integration_actor_isolation_sendability_contract.py",
        "M188-A001 and M188-D001 packet-specific artifacts are landed in this workspace.",
        "M188-B001 and M188-C001 packet-specific artifacts are not landed in this workspace as of this wiring change.",
        "This initial M188-E001 gate deterministically replays currently landed lane surfaces via the M188-A001 frontend parser contract plus the M188-D001 validation/conformance packet.",
        "M188-B001 sema surfaces and M188-C001 lowering surfaces are fail-closed via M188-D001 replay packet anchors in this integration gate.",
        "The integration gate fail-closes on these currently landed lane surfaces plus this M188-E001 wiring contract.",
    ):
        assert text in library_api_doc


def test_m188_e001_integration_runbook_section_is_documented() -> None:
    tests_doc = _read(TESTS_DOC)

    for text in (
        "## M188 integration actor isolation and sendability contract runbook (M188-E001)",
        "python -m pytest tests/tooling/test_objc3c_m188_frontend_actor_isolation_sendability_parser_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m188_validation_actor_isolation_sendability_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m188_conformance_actor_isolation_sendability_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m188_integration_actor_isolation_sendability_contract.py -q",
        "npm run check:objc3c:m188-actor-isolation-sendability-contracts",
        "npm run check:compiler-closeout:m188",
        "Enforce M188 actor isolation/sendability packet/docs contract",
        "Run M188 actor isolation/sendability integration gate",
    ):
        assert text in tests_doc


def test_m188_integration_actor_isolation_sendability_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(COMPILER_CLOSEOUT_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m188-actor-isolation-sendability-contracts" in scripts
    assert scripts["check:objc3c:m188-actor-isolation-sendability-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m188_frontend_actor_isolation_sendability_parser_contract.py "
        "tests/tooling/test_objc3c_m188_validation_actor_isolation_sendability_contract.py "
        "tests/tooling/test_objc3c_m188_conformance_actor_isolation_sendability_contract.py "
        "tests/tooling/test_objc3c_m188_integration_actor_isolation_sendability_contract.py -q"
    )

    assert "check:compiler-closeout:m188" in scripts
    assert scripts["check:compiler-closeout:m188"] == (
        "npm run check:objc3c:m188-actor-isolation-sendability-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m188" in scripts["check:task-hygiene"]

    assert "Enforce M188 actor isolation/sendability packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m188" in workflow
    assert "Run M188 actor isolation/sendability integration gate" in workflow
    assert "npm run check:objc3c:m188-actor-isolation-sendability-contracts" in workflow

    assert workflow.index("Run M183 NSError bridging integration gate") < workflow.index(
        "Enforce M188 actor isolation/sendability packet/docs contract"
    )
    assert workflow.index("Enforce M188 actor isolation/sendability packet/docs contract") < workflow.index(
        "Run M188 actor isolation/sendability integration gate"
    )
    assert workflow.index("Run M188 actor isolation/sendability integration gate") < workflow.index(
        "Enforce M189 task-runtime interop/cancellation packet/docs contract"
    )

