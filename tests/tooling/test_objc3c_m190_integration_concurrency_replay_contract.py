import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
TESTS_DOC = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
PACKAGE_JSON = ROOT / "package.json"
COMPILER_CLOSEOUT_WORKFLOW = ROOT / ".github" / "workflows" / "compiler-closeout.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m190_integration_concurrency_replay_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M190 integration concurrency replay-proof and race-guard contract",
        "check:objc3c:m190-concurrency-replay-race-guard-contracts",
        "check:compiler-closeout:m190",
        ".github/workflows/compiler-closeout.yml",
        "tests/tooling/test_objc3c_m190_integration_concurrency_replay_contract.py",
    ):
        assert text in library_api_doc


def test_m190_e001_integration_runbook_section_is_documented() -> None:
    tests_doc = _read(TESTS_DOC)

    for text in (
        "## M190 integration concurrency replay-proof and race-guard contract runbook (M190-E001)",
        "npm run check:objc3c:m190-concurrency-replay-race-guard-contracts",
        "npm run check:compiler-closeout:m190",
        "Enforce M190 concurrency replay/race-guard packet/docs contract",
        "Run M190 concurrency replay/race-guard integration gate",
    ):
        assert text in tests_doc


def test_m190_integration_concurrency_replay_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(COMPILER_CLOSEOUT_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m190-concurrency-replay-race-guard-contracts" in scripts
    assert scripts["check:objc3c:m190-concurrency-replay-race-guard-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m190_frontend_concurrency_replay_parser_contract.py "
        "tests/tooling/test_objc3c_m190_sema_concurrency_replay_race_guard_contract.py "
        "tests/tooling/test_objc3c_m190_lowering_concurrency_replay_race_guard_contract.py "
        "tests/tooling/test_objc3c_m190_validation_concurrency_replay_contract.py "
        "tests/tooling/test_objc3c_m190_conformance_concurrency_replay_contract.py "
        "tests/tooling/test_objc3c_m190_integration_concurrency_replay_race_guard_contract.py -q"
    )

    assert "check:compiler-closeout:m190" in scripts
    assert scripts["check:compiler-closeout:m190"] == (
        "npm run check:objc3c:m190-concurrency-replay-race-guard-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m190" in scripts["check:task-hygiene"]

    assert "Enforce M190 concurrency replay/race-guard packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m190" in workflow
    assert "Run M190 concurrency replay/race-guard integration gate" in workflow
    assert "npm run check:objc3c:m190-concurrency-replay-race-guard-contracts" in workflow

    assert workflow.index("Run M183 NSError bridging integration gate") < workflow.index(
        "Enforce M190 concurrency replay/race-guard packet/docs contract"
    )
    assert workflow.index("Enforce M190 concurrency replay/race-guard packet/docs contract") < workflow.index(
        "Run M190 concurrency replay/race-guard integration gate"
    )
    assert workflow.index("Run M190 concurrency replay/race-guard integration gate") < workflow.index(
        "Enforce M191 unsafe-pointer packet/docs contract"
    )
