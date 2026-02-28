from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TESTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
DOC_AGGREGATE = ROOT / "docs" / "objc3c-native.md"
PACKAGE_JSON = ROOT / "package.json"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m223_validation_perf_tutorial_is_documented() -> None:
    tests_fragment = _read(TESTS_DOC_FRAGMENT)
    aggregate_doc = _read(DOC_AGGREGATE)

    for text in (
        "## M223 validation/perf triage sequence",
        "npm run test:objc3c:m145-direct-llvm-matrix",
        "npm run test:objc3c:execution-smoke",
        "npm run test:objc3c:execution-replay-proof",
        "npm run test:objc3c:perf-budget",
        "tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json",
        "tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json",
        "tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json",
    ):
        assert text in tests_fragment
        assert text in aggregate_doc


def test_m223_validation_perf_tutorial_commands_exist() -> None:
    package_json = _read(PACKAGE_JSON)

    for script_name in (
        '"test:objc3c:m145-direct-llvm-matrix"',
        '"test:objc3c:execution-smoke"',
        '"test:objc3c:execution-replay-proof"',
        '"test:objc3c:perf-budget"',
    ):
        assert script_name in package_json
