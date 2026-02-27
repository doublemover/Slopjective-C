from __future__ import annotations

import importlib.util
import io
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "check_dispatch_reopen_guardrails_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_dispatch_reopen_guardrails_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_dispatch_reopen_guardrails_contract.py")
module = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = module
SPEC.loader.exec_module(module)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "dispatch_reopen_guardrails_contract"
HAPPY_ROOT = FIXTURE_ROOT / "happy"
DRIFT_MANIFEST_PATH = FIXTURE_ROOT / "drift_missing_lane_b_validation" / "batch_manifest.md"
W2_DRIFT_ROOT = FIXTURE_ROOT / "w2_drift_freshness_provenance_gate_sequence"
HARD_FAIL_REVIEW_PATH = FIXTURE_ROOT / "hard_fail_invalid_utf8" / "dispatch_review.md"


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = module.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def test_happy_path_is_deterministic() -> None:
    args = [
        "--playbook",
        str(HAPPY_ROOT / "playbook.md"),
        "--dispatch-review",
        str(HAPPY_ROOT / "dispatch_review.md"),
        "--batch-manifest",
        str(HAPPY_ROOT / "batch_manifest.md"),
    ]
    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 0
    assert second_code == 0
    assert first_stderr == ""
    assert second_stderr == ""
    assert first_stdout == second_stdout
    assert first_stdout == (
        "dispatch-reopen-guardrails-contract: OK\n"
        "- mode=dispatch-reopen-guardrails-contract-v2\n"
        "- playbook=tests/tooling/fixtures/dispatch_reopen_guardrails_contract/happy/playbook.md\n"
        "- dispatch_review="
        "tests/tooling/fixtures/dispatch_reopen_guardrails_contract/happy/dispatch_review.md\n"
        "- batch_manifest="
        "tests/tooling/fixtures/dispatch_reopen_guardrails_contract/happy/batch_manifest.md\n"
        "- checks_passed=44\n"
        "- fail_closed=true\n"
    )


def test_drift_path_returns_exit_1_with_stable_diagnostics() -> None:
    args = [
        "--playbook",
        str(HAPPY_ROOT / "playbook.md"),
        "--dispatch-review",
        str(HAPPY_ROOT / "dispatch_review.md"),
        "--batch-manifest",
        str(DRIFT_MANIFEST_PATH),
    ]
    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 1
    assert second_code == 1
    assert first_stdout == ""
    assert second_stdout == ""
    assert first_stderr == second_stderr
    assert first_stderr == (
        "dispatch-reopen-guardrails-contract: contract drift detected (1 failed check(s)).\n"
        "drift findings:\n"
        "- batch_manifest:MAN-05\n"
        "  expected snippet: 2. Lane `B`: `python -m pytest tests/tooling -q`; "
        "`python scripts/spec_lint.py`\n"
        "remediation:\n"
        "1. Restore missing baseline-control snippet anchors, freshness/provenance "
        "invariants, and deterministic gate-sequencing parity in the listed artifact(s).\n"
        "2. Re-run validator:\n"
        "python scripts/check_dispatch_reopen_guardrails_contract.py --playbook "
        "tests/tooling/fixtures/dispatch_reopen_guardrails_contract/happy/playbook.md "
        "--dispatch-review tests/tooling/fixtures/dispatch_reopen_guardrails_contract/"
        "happy/dispatch_review.md --batch-manifest "
        "tests/tooling/fixtures/dispatch_reopen_guardrails_contract/"
        "drift_missing_lane_b_validation/batch_manifest.md\n"
    )


def test_w2_drift_path_orders_multi_finding_diagnostics_deterministically() -> None:
    args = [
        "--playbook",
        str(W2_DRIFT_ROOT / "playbook.md"),
        "--dispatch-review",
        str(W2_DRIFT_ROOT / "dispatch_review.md"),
        "--batch-manifest",
        str(W2_DRIFT_ROOT / "batch_manifest.md"),
    ]
    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 1
    assert second_code == 1
    assert first_stdout == ""
    assert second_stdout == ""
    assert first_stderr == second_stderr
    assert first_stderr == (
        "dispatch-reopen-guardrails-contract: contract drift detected (6 failed check(s)).\n"
        "drift findings:\n"
        "- playbook:PBK-12\n"
        "  expected snippet: | `AR-DEP-M03-02` | `Hard` | Snapshot refresh (`AR-RUN-08`) "
        "and freshness (`AR-RUN-09`) evidence must be linked to the same reopen "
        "decision cycle as `AR-RUN-02` and `AR-DECISION-01`. |\n"
        "- playbook:PBK-18\n"
        "  deterministic W2 gate sequencing must be [\"AR-DRW1-G0-TRIGGER\", "
        "\"AR-DRW1-G1-EVIDENCE-LINKAGE\", \"AR-DRW1-G2-ACCEPTANCE-MATRIX\", "
        "\"AR-DRW1-G3-LANE-EVIDENCE\", \"AR-DRW1-G4-SPEC-LINT\", "
        "\"AR-DRW1-S1-T4-PROVENANCE\", \"AR-DRW1-S2-INTAKE-ORDER\"] "
        "(found [\"AR-DRW1-G0-TRIGGER\", \"AR-DRW1-G1-EVIDENCE-LINKAGE\", "
        "\"AR-DRW1-G3-LANE-EVIDENCE\", \"AR-DRW1-G2-ACCEPTANCE-MATRIX\", "
        "\"AR-DRW1-G4-SPEC-LINT\", \"AR-DRW1-S1-T4-PROVENANCE\", "
        "\"AR-DRW1-S2-INTAKE-ORDER\"])\n"
        "- playbook:PBK-19\n"
        "  freshness/provenance evidence row set must be [\"AR-CMD-08\", "
        "\"AR-CMD-09\", \"AR-SNAPSHOT-01\", \"AR-FRESHNESS-01\", "
        "\"AR-DECISION-01\"] (found [\"AR-CMD-08\", \"AR-CMD-09\", "
        "\"AR-SNAPSHOT-01\", \"AR-DECISION-01\"])\n"
        "- dispatch_review:REV-10\n"
        "  expected snippet: 3. Serialize GH closeout at terminal stage as `A -> B -> C -> "
        "D -> INT`, then close milestone.\n"
        "- dispatch_review:REV-12\n"
        "  deterministic GH closeout sequence must be [\"A\", \"B\", \"C\", "
        "\"D\", \"INT\"] (found [\"A\", \"C\", \"B\", \"D\", "
        "\"INT\"])\n"
        "- batch_manifest:MAN-12\n"
        "  deterministic intake sequencing rows must be [[\"A\", \"#904\"], "
        "[\"B\", \"#905\"], [\"C\", \"#906\"], [\"D\", \"#907\"], "
        "[\"INT\", \"#908\"]] (found [[\"B\", \"#905\"], [\"A\", "
        "\"#904\"], [\"C\", \"#906\"], [\"D\", \"#907\"], [\"INT\", "
        "\"#908\"]])\n"
        "remediation:\n"
        "1. Restore missing baseline-control snippet anchors, freshness/provenance "
        "invariants, and deterministic gate-sequencing parity in the listed artifact(s).\n"
        "2. Re-run validator:\n"
        "python scripts/check_dispatch_reopen_guardrails_contract.py --playbook "
        "tests/tooling/fixtures/dispatch_reopen_guardrails_contract/"
        "w2_drift_freshness_provenance_gate_sequence/playbook.md --dispatch-review "
        "tests/tooling/fixtures/dispatch_reopen_guardrails_contract/"
        "w2_drift_freshness_provenance_gate_sequence/dispatch_review.md --batch-manifest "
        "tests/tooling/fixtures/dispatch_reopen_guardrails_contract/"
        "w2_drift_freshness_provenance_gate_sequence/batch_manifest.md\n"
    )


def test_hard_fail_returns_exit_2_for_missing_playbook() -> None:
    code, stdout, stderr = run_main(
        [
            "--playbook",
            str(HAPPY_ROOT / "missing_playbook.md"),
            "--dispatch-review",
            str(HAPPY_ROOT / "dispatch_review.md"),
            "--batch-manifest",
            str(HAPPY_ROOT / "batch_manifest.md"),
        ]
    )

    assert code == 2
    assert stdout == ""
    assert stderr == (
        "dispatch-reopen-guardrails-contract: error: playbook file does not exist: "
        "tests/tooling/fixtures/dispatch_reopen_guardrails_contract/happy/missing_playbook.md\n"
    )


def test_hard_fail_returns_exit_2_for_invalid_utf8_dispatch_review() -> None:
    code, stdout, stderr = run_main(
        [
            "--playbook",
            str(HAPPY_ROOT / "playbook.md"),
            "--dispatch-review",
            str(HARD_FAIL_REVIEW_PATH),
            "--batch-manifest",
            str(HAPPY_ROOT / "batch_manifest.md"),
        ]
    )

    assert code == 2
    assert stdout == ""
    assert stderr == (
        "dispatch-reopen-guardrails-contract: error: dispatch_review file is not valid UTF-8: "
        "tests/tooling/fixtures/dispatch_reopen_guardrails_contract/"
        "hard_fail_invalid_utf8/dispatch_review.md\n"
    )
