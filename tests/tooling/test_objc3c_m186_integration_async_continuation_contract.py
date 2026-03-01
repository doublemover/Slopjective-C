import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
TESTS_DOC = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
PACKAGE_JSON = ROOT / "package.json"
COMPILER_CLOSEOUT_WORKFLOW = ROOT / ".github" / "workflows" / "compiler-closeout.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m186_integration_async_continuation_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M186 integration async grammar and continuation IR contract",
        "check:objc3c:m186-async-continuation-contracts",
        "check:compiler-closeout:m186",
        ".github/workflows/compiler-closeout.yml",
        "tests/tooling/test_objc3c_m186_integration_async_continuation_contract.py",
    ):
        assert text in library_api_doc


def test_m186_e001_integration_runbook_section_is_documented() -> None:
    tests_doc = _read(TESTS_DOC)

    for text in (
        "## M186 integration async grammar and continuation IR contract runbook (M186-E001)",
        "npm run check:objc3c:m186-async-continuation-contracts",
        "npm run check:compiler-closeout:m186",
        "Enforce M186 async grammar/continuation IR packet/docs contract",
        "Run M186 async grammar/continuation IR integration gate",
    ):
        assert text in tests_doc


def test_m186_integration_async_continuation_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(COMPILER_CLOSEOUT_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m186-async-continuation-contracts" in scripts
    assert scripts["check:objc3c:m186-async-continuation-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m186_frontend_async_continuation_parser_contract.py "
        "tests/tooling/test_objc3c_m186_sema_async_continuation_contract.py "
        "tests/tooling/test_objc3c_m186_lowering_async_continuation_contract.py "
        "tests/tooling/test_objc3c_m186_validation_async_continuation_contract.py "
        "tests/tooling/test_objc3c_m186_conformance_async_continuation_contract.py "
        "tests/tooling/test_objc3c_m186_integration_async_continuation_contract.py -q"
    )

    assert "check:compiler-closeout:m186" in scripts
    assert scripts["check:compiler-closeout:m186"] == (
        "npm run check:objc3c:m186-async-continuation-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m186" in scripts["check:task-hygiene"]

    assert "Enforce M186 async grammar/continuation IR packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m186" in workflow
    assert "Run M186 async grammar/continuation IR integration gate" in workflow
    assert "npm run check:objc3c:m186-async-continuation-contracts" in workflow

    assert workflow.index("Run M183 NSError bridging integration gate") < workflow.index(
        "Enforce M186 async grammar/continuation IR packet/docs contract"
    )
    assert workflow.index("Enforce M186 async grammar/continuation IR packet/docs contract") < workflow.index(
        "Run M186 async grammar/continuation IR integration gate"
    )
    assert workflow.index("Run M186 async grammar/continuation IR integration gate") < workflow.index(
        "Enforce M187 await lowering/suspension-state packet/docs contract"
    )
