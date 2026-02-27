from __future__ import annotations

import importlib.util
import io
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "check_release_evidence.py"
SPEC = importlib.util.spec_from_file_location("check_release_evidence", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_release_evidence.py")
module = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = module
SPEC.loader.exec_module(module)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "release_evidence_contract"
INVALID_EMPTY_CONTRACT = FIXTURE_ROOT / "invalid_empty_pairs.json"
MISSING_SCHEMA_CONTRACT = FIXTURE_ROOT / "missing_schema_pair.json"
MULTI_FINDING_CONTRACT = FIXTURE_ROOT / "drift_multi_pair_unsorted.json"
HARD_FAIL_INVALID_JSON_CONTRACT = FIXTURE_ROOT / "hard_fail_invalid_contract_json.json"


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = module.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def test_default_contract_happy_path_is_deterministic() -> None:
    first_code, first_stdout, first_stderr = run_main([])
    second_code, second_stdout, second_stderr = run_main([])

    assert first_code == 0
    assert second_code == 0
    assert first_stdout == second_stdout == (
        "release-evidence: OK\n"
        "- mode=release-evidence-contract-v2\n"
        "- contract_id=release-evidence-contract/default\n"
        "- release_label=v0.11\n"
        "- schema_data_pairs=3\n"
        "- fail_closed=true\n"
    )
    assert first_stderr == second_stderr == ""


def test_contract_with_empty_schema_pairs_returns_drift_with_stable_output() -> None:
    code, stdout, stderr = run_main(["--contract", str(INVALID_EMPTY_CONTRACT)])

    assert code == 1
    assert stdout == ""
    assert stderr == (
        "release-evidence: contract drift detected (1 issue(s)).\n"
        "drift findings:\n"
        "- contract:CON-02\n"
        "  schema_data_pairs must be a non-empty array\n"
        "remediation:\n"
        "1. Restore missing/invalid release-evidence contract inputs or artifact references, then rerun.\n"
        "2. Re-run validator:\n"
        "python scripts/check_release_evidence.py --contract "
        "tests/tooling/fixtures/release_evidence_contract/invalid_empty_pairs.json\n"
    )


def test_contract_with_missing_schema_file_returns_drift_with_stable_output() -> None:
    code, stdout, stderr = run_main(["--contract", str(MISSING_SCHEMA_CONTRACT)])

    assert code == 1
    assert stdout == ""
    assert stderr == (
        "release-evidence: contract drift detected (1 issue(s)).\n"
        "drift findings:\n"
        "- pair[missing-schema]:PAIR-01\n"
        "  missing required schema file: "
        "tests/tooling/fixtures/release_evidence_contract/does_not_exist.schema.json\n"
        "remediation:\n"
        "1. Restore missing/invalid release-evidence contract inputs or artifact references, then rerun.\n"
        "2. Re-run validator:\n"
        "python scripts/check_release_evidence.py --contract "
        "tests/tooling/fixtures/release_evidence_contract/missing_schema_pair.json\n"
    )


def test_multi_finding_drift_order_is_deterministic() -> None:
    first_code, first_stdout, first_stderr = run_main(["--contract", str(MULTI_FINDING_CONTRACT)])
    second_code, second_stdout, second_stderr = run_main(
        ["--contract", str(MULTI_FINDING_CONTRACT)]
    )

    assert first_code == 1
    assert second_code == 1
    assert first_stdout == second_stdout == ""
    assert first_stderr == second_stderr == (
        "release-evidence: contract drift detected (4 issue(s)).\n"
        "drift findings:\n"
        "- pair[alpha]:PAIR-01\n"
        "  missing required schema file: tests/tooling/fixtures/release_evidence_contract/does_not_exist_alpha.schema.json\n"
        "- pair[alpha]:PAIR-03\n"
        "  missing required data file: tests/tooling/fixtures/release_evidence_contract/does_not_exist_alpha.data.json\n"
        "- pair[zeta]:PAIR-01\n"
        "  missing required schema file: tests/tooling/fixtures/release_evidence_contract/does_not_exist_zeta.schema.json\n"
        "- pair[zeta]:PAIR-03\n"
        "  missing required data file: tests/tooling/fixtures/release_evidence_contract/does_not_exist_zeta.data.json\n"
        "remediation:\n"
        "1. Restore missing/invalid release-evidence contract inputs or artifact references, then rerun.\n"
        "2. Re-run validator:\n"
        "python scripts/check_release_evidence.py --contract "
        "tests/tooling/fixtures/release_evidence_contract/drift_multi_pair_unsorted.json\n"
    )


def test_invalid_contract_json_returns_hard_fail() -> None:
    code, stdout, stderr = run_main(
        ["--contract", str(HARD_FAIL_INVALID_JSON_CONTRACT)]
    )

    assert code == 2
    assert stdout == ""
    assert stderr == (
        "release-evidence: error: contract file is not valid JSON: "
        "tests/tooling/fixtures/release_evidence_contract/hard_fail_invalid_contract_json.json "
        "(line 1 column 17: Expecting value)\n"
    )
