import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m170_integration_block_determinism_perf_baseline_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M170 integration block determinism/perf baseline contract",
        "check:objc3c:m170-block-determinism-perf-baseline-contracts",
        "check:compiler-closeout:m170",
        "tests/tooling/test_objc3c_m170_frontend_block_determinism_perf_baseline_parser_contract.py",
        "tests/tooling/test_objc3c_m170_sema_block_determinism_perf_baseline_contract.py",
        "tests/tooling/test_objc3c_m170_lowering_block_determinism_perf_baseline_contract.py",
        "tests/tooling/test_objc3c_m170_validation_block_determinism_perf_baseline_contract.py",
        "tests/tooling/test_objc3c_m170_integration_block_determinism_perf_baseline_contract.py",
    ):
        assert text in library_api_doc


def test_m170_integration_block_determinism_perf_baseline_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m170-block-determinism-perf-baseline-contracts" in scripts
    assert scripts["check:objc3c:m170-block-determinism-perf-baseline-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m170_frontend_block_determinism_perf_baseline_parser_contract.py "
        "tests/tooling/test_objc3c_m170_sema_block_determinism_perf_baseline_contract.py "
        "tests/tooling/test_objc3c_m170_lowering_block_determinism_perf_baseline_contract.py "
        "tests/tooling/test_objc3c_m170_validation_block_determinism_perf_baseline_contract.py "
        "tests/tooling/test_objc3c_m170_integration_block_determinism_perf_baseline_contract.py -q"
    )

    assert "check:compiler-closeout:m170" in scripts
    assert scripts["check:compiler-closeout:m170"] == (
        "npm run check:objc3c:m170-block-determinism-perf-baseline-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m170" in scripts["check:task-hygiene"]

    assert "Enforce M170 block-determinism/perf-baseline packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m170" in workflow
    assert "Run M170 block determinism/perf baseline integration gate" in workflow
    assert "npm run check:objc3c:m170-block-determinism-perf-baseline-contracts" in workflow
