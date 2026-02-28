import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TESTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
PACKAGE_JSON = ROOT / "package.json"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m142_validation_cli_c_api_parity_runbook_section_present() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M142 validation/perf CLI and C API parity harness runbook",
        "npm run test:objc3c:m142-lowering-parity",
        "npm run check:objc3c:library-cli-parity:source",
        "npm run check:compiler-closeout:m142",
        "tmp/artifacts/objc3c-native/m142/library-cli-parity/work/",
        "tmp/artifacts/objc3c-native/m142/library-cli-parity/summary.json",
        "artifacts/bin/objc3c-native.exe",
        "artifacts/bin/objc3c-frontend-c-api-runner.exe",
        "python -m pytest tests/tooling/test_objc3c_m142_validation_cli_c_api_parity_contract.py -q",
    ):
        assert text in fragment


def test_m142_validation_cli_c_api_parity_commands_available() -> None:
    package_payload = json.loads(_read(PACKAGE_JSON))
    scripts = package_payload["scripts"]

    for script_name in (
        "test:objc3c:m142-lowering-parity",
        "check:objc3c:library-cli-parity:source",
        "check:compiler-closeout:m142",
    ):
        assert script_name in scripts

    assert "tests/tooling/test_objc3c_library_cli_parity.py" in scripts["test:objc3c:m142-lowering-parity"]
    assert "tests/tooling/test_objc3c_c_api_runner_extraction.py" in scripts["test:objc3c:m142-lowering-parity"]
    assert "python scripts/check_objc3c_library_cli_parity.py" in scripts["check:objc3c:library-cli-parity:source"]
    assert "python scripts/check_m142_frontend_lowering_parity_contract.py" in scripts["check:compiler-closeout:m142"]
