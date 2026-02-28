import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
TESTS_DOC = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
PACKAGE_JSON = ROOT / "package.json"
COMPILER_CLOSEOUT_WORKFLOW = ROOT / ".github" / "workflows" / "compiler-closeout.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m182_integration_result_like_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M182 integration result-like lowering contract",
        "check:objc3c:m182-result-like-contracts",
        "check:compiler-closeout:m182",
        ".github/workflows/compiler-closeout.yml",
        "tests/tooling/test_objc3c_m182_frontend_result_like_parser_contract.py",
        "tests/tooling/test_objc3c_m182_sema_result_like_contract.py",
        "tests/tooling/test_objc3c_m182_lowering_result_like_contract.py",
        "tests/tooling/test_objc3c_m182_validation_result_like_lowering_contract.py",
        "tests/tooling/test_objc3c_m182_conformance_result_like_lowering_contract.py",
        "tests/tooling/test_objc3c_m182_integration_result_like_lowering_contract.py",
        "M182-A001 through M182-D001 outputs are landed in this workspace.",
        "The integration gate fail-closes on parser/sema/lowering/validation/conformance surfaces plus this M182-E001 wiring contract.",
    ):
        assert text in library_api_doc


def test_m182_e001_integration_runbook_section_is_documented() -> None:
    tests_doc = _read(TESTS_DOC)

    for text in (
        "## M182 integration result-like lowering contract runbook (M182-E001)",
        "python -m pytest tests/tooling/test_objc3c_m182_frontend_result_like_parser_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m182_sema_result_like_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m182_lowering_result_like_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m182_validation_result_like_lowering_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m182_conformance_result_like_lowering_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m182_integration_result_like_lowering_contract.py -q",
        "npm run check:objc3c:m182-result-like-contracts",
        "npm run check:compiler-closeout:m182",
        "Enforce M182 result-like lowering packet/docs contract",
        "Run M182 result-like lowering integration gate",
    ):
        assert text in tests_doc


def test_m182_integration_result_like_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(COMPILER_CLOSEOUT_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m182-result-like-contracts" in scripts
    assert scripts["check:objc3c:m182-result-like-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m182_frontend_result_like_parser_contract.py "
        "tests/tooling/test_objc3c_m182_sema_result_like_contract.py "
        "tests/tooling/test_objc3c_m182_lowering_result_like_contract.py "
        "tests/tooling/test_objc3c_m182_validation_result_like_lowering_contract.py "
        "tests/tooling/test_objc3c_m182_conformance_result_like_lowering_contract.py "
        "tests/tooling/test_objc3c_m182_integration_result_like_lowering_contract.py -q"
    )

    assert "check:compiler-closeout:m182" in scripts
    assert scripts["check:compiler-closeout:m182"] == (
        "npm run check:objc3c:m182-result-like-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m182" in scripts["check:task-hygiene"]

    assert "Enforce M182 result-like lowering packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m182" in workflow
    assert "Run M182 result-like lowering integration gate" in workflow
    assert "npm run check:objc3c:m182-result-like-contracts" in workflow

    assert workflow.index("Enforce M182 result-like lowering packet/docs contract") < workflow.index(
        "Run M182 result-like lowering integration gate"
    )
    assert workflow.index("npm run check:compiler-closeout:m182") < workflow.index(
        "npm run check:objc3c:m182-result-like-contracts"
    )
