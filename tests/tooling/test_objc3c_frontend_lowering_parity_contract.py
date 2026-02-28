import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FRONTEND_ANCHOR = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
PACKAGE_JSON = ROOT / "package.json"
CLI_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "10-cli.md"
SEMANTICS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
ARTIFACTS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
TESTS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
CONTRACT_DOC = ROOT / "docs" / "contracts" / "frontend_lowering_parity_expectations.md"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _assert_in_order(text: str, snippets: list[str]) -> None:
    cursor = -1
    for snippet in snippets:
        index = text.find(snippet)
        assert index != -1, f"missing snippet: {snippet}"
        assert index > cursor, f"snippet out of order: {snippet}"
        cursor = index


def test_c_api_frontend_prevalidates_lowering_contract_before_pipeline_run() -> None:
    source = _read(FRONTEND_ANCHOR)

    assert "TryNormalizeObjc3LoweringContract(frontend_options.lowering, normalized_lowering, lowering_error)" in source
    assert "result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;" in source
    assert "result->process_exit_code = 2;" in source
    assert "objc3c_frontend_set_error(context, lowering_error.c_str());" in source

    _assert_in_order(
        source,
        [
            "Objc3FrontendOptions frontend_options = BuildFrontendOptions(*options);",
            "if (!TryNormalizeObjc3LoweringContract(frontend_options.lowering, normalized_lowering, lowering_error)) {",
            "result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;",
            "return result->status;",
            "Objc3FrontendCompileProduct product = CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options);",
        ],
    )


def test_m142_package_and_docs_wiring_contract() -> None:
    package_payload = json.loads(_read(PACKAGE_JSON))
    scripts = package_payload["scripts"]

    assert "check:objc3c:library-cli-parity:source" in scripts
    assert "--source" in scripts["check:objc3c:library-cli-parity:source"]
    assert "--cli-bin artifacts/bin/objc3c-native.exe" in scripts["check:objc3c:library-cli-parity:source"]
    assert "--c-api-bin artifacts/bin/objc3c-frontend-c-api-runner.exe" in scripts["check:objc3c:library-cli-parity:source"]
    assert "--cli-ir-object-backend clang" in scripts["check:objc3c:library-cli-parity:source"]

    assert "test:objc3c:m142-lowering-parity" in scripts
    assert "tests/tooling/test_objc3c_library_cli_parity.py" in scripts["test:objc3c:m142-lowering-parity"]
    assert "tests/tooling/test_objc3c_c_api_runner_extraction.py" in scripts["test:objc3c:m142-lowering-parity"]
    assert "tests/tooling/test_objc3c_frontend_lowering_parity_contract.py" in scripts["test:objc3c:m142-lowering-parity"]
    assert "tests/tooling/test_objc3c_sema_cli_c_api_parity_surface.py" in scripts["test:objc3c:m142-lowering-parity"]

    assert "check:compiler-closeout:m142" in scripts
    assert "python scripts/check_m142_frontend_lowering_parity_contract.py" in scripts["check:compiler-closeout:m142"]
    assert "npm run test:objc3c:m142-lowering-parity" in scripts["check:compiler-closeout:m142"]
    assert '--glob "docs/contracts/frontend_lowering_parity_expectations.md"' in scripts["check:compiler-closeout:m142"]
    assert "check:compiler-closeout:m142" in scripts["check:task-hygiene"]

    assert "## C API parity runner usage (M142-E001)" in _read(CLI_FRAGMENT)
    assert "## Frontend lowering parity harness contract (M142-E001)" in _read(SEMANTICS_FRAGMENT)
    assert "## Frontend lowering parity harness artifacts (M142-E001)" in _read(ARTIFACTS_FRAGMENT)
    assert "npm run check:compiler-closeout:m142" in _read(TESTS_FRAGMENT)
    assert "Contract ID: `objc3c-frontend-lowering-parity-contract/m142-v1`" in _read(CONTRACT_DOC)
