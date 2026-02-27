import importlib.util
import io
import json
import os
import subprocess
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "generate_quality_gate_decision.py"
SPEC = importlib.util.spec_from_file_location("generate_quality_gate_decision", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/generate_quality_gate_decision.py")
generate_quality_gate_decision = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = generate_quality_gate_decision
SPEC.loader.exec_module(generate_quality_gate_decision)

FIXTURE_DIR = Path(__file__).resolve().parent / "fixtures" / "quality_gate_decision"


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = generate_quality_gate_decision.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def test_happy_path_matches_fixtures_and_keeps_hold_semantics(tmp_path: Path) -> None:
    expected_md = (FIXTURE_DIR / "expected_happy_path.md").read_bytes()
    expected_status = (FIXTURE_DIR / "expected_happy_path.status.json").read_bytes()
    output_md = tmp_path / "quality_gate.md"
    output_status = tmp_path / "quality_gate.status.json"

    code, stdout, stderr = run_main(
        [
            "--output-md",
            str(output_md),
            "--output-status",
            str(output_status),
            "--generated-at",
            "2026-02-24T00:00:00Z",
        ]
    )

    assert code == 0
    assert stderr == ""
    assert stdout == "quality-gate-generator: OK (decision=hold, qg04=fail, recommendation=no-go, ev_items=3)\n"
    assert b"\r" not in expected_md
    assert b"\r" not in expected_status
    assert output_md.read_bytes() == expected_md
    assert output_status.read_bytes() == expected_status
    assert output_md.read_bytes().endswith(b"\n")
    assert output_status.read_bytes().endswith(b"\n")

    status_payload = json.loads(expected_status.decode("utf-8"))
    assert [row["evidence_id"] for row in status_payload["evidence_items"]] == [
        "EV-06",
        "EV-07",
        "EV-08",
    ]
    assert [row["evidence_id"] for row in status_payload["ev_contract_mapping"]] == [
        "EV-06",
        "EV-07",
        "EV-08",
    ]
    assert status_payload["overall_decision"] == "hold"
    assert status_payload["qg_04_result"] == "fail"
    assert status_payload["recommendation_signal"] == "no-go"
    assert status_payload["downstream_consumers"] == ["V013-CONF-03", "V013-REL-01"]


def test_hold_approve_semantics_are_explicit() -> None:
    assert generate_quality_gate_decision.determine_decision("pass") == "approve"
    assert generate_quality_gate_decision.determine_decision("conditional-pass") == "hold"
    assert generate_quality_gate_decision.determine_decision("fail") == "hold"
    assert generate_quality_gate_decision.determine_decision("blocked") == "hold"


def test_contract_drift_returns_exit_1_and_writes_nothing(
    monkeypatch: object, tmp_path: Path
) -> None:
    output_md = tmp_path / "drift.md"
    output_status = tmp_path / "drift.status.json"
    drifted_handoffs = json.loads(
        (FIXTURE_DIR / "drift_handoffs_required_inputs.json").read_text(encoding="utf-8")
    )
    monkeypatch.setattr(generate_quality_gate_decision, "DOWNSTREAM_HANDOFFS", drifted_handoffs)

    code, stdout, stderr = run_main(
        [
            "--output-md",
            str(output_md),
            "--output-status",
            str(output_status),
            "--generated-at",
            "2026-02-24T00:00:00Z",
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "quality-gate-generator: DRIFT" in stderr
    assert "required_inputs drift" in stderr
    assert not output_md.exists()
    assert not output_status.exists()


def test_contract_hard_fail_returns_exit_2_and_writes_nothing(
    monkeypatch: object, tmp_path: Path
) -> None:
    output_md = tmp_path / "hard_fail.md"
    output_status = tmp_path / "hard_fail.status.json"
    malformed_evidence = json.loads(
        (FIXTURE_DIR / "hard_fail_evidence_missing_summary.json").read_text(encoding="utf-8")
    )
    monkeypatch.setattr(generate_quality_gate_decision, "EVIDENCE_ITEMS", malformed_evidence)

    code, stdout, stderr = run_main(
        [
            "--output-md",
            str(output_md),
            "--output-status",
            str(output_status),
            "--generated-at",
            "2026-02-24T00:00:00Z",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "quality-gate-generator: HARD-FAIL" in stderr
    assert "schema mismatch" in stderr
    assert "missing keys ['summary']" in stderr
    assert not output_md.exists()
    assert not output_status.exists()


def test_contract_drift_gate_status_returns_exit_1_and_writes_nothing(
    monkeypatch: object, tmp_path: Path
) -> None:
    output_md = tmp_path / "gate_drift.md"
    output_status = tmp_path / "gate_drift.status.json"
    drifted_gates = [
        {
            "gate_id": "QG-01",
            "status": "fail",
            "rationale": "CT-04 failed: unresolved high/critical blocker count is 3 (threshold requires 0).",
        },
        {
            "gate_id": "QG-02",
            "status": "blocked",
            "rationale": "Diagnostics stability evidence is present; no active diagnostics exception is required.",
        },
        {
            "gate_id": "QG-03",
            "status": "pass",
            "rationale": "Reproducibility rerun digest indicates stable replay outcomes for the locked snapshot.",
        },
    ]
    monkeypatch.setattr(generate_quality_gate_decision, "BASE_GATE_RESULTS", drifted_gates)

    code, stdout, stderr = run_main(
        [
            "--output-md",
            str(output_md),
            "--output-status",
            str(output_status),
            "--generated-at",
            "2026-02-24T00:00:00Z",
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "quality-gate-generator: DRIFT" in stderr
    assert "QG-02 status drift" in stderr
    assert not output_md.exists()
    assert not output_status.exists()


def test_contract_hard_fail_rejects_unexpected_evidence_keys(
    monkeypatch: object, tmp_path: Path
) -> None:
    output_md = tmp_path / "schema_hard_fail.md"
    output_status = tmp_path / "schema_hard_fail.status.json"
    malformed_evidence = [
        {
            "evidence_id": "EV-06",
            "status": "pass",
            "summary": "Exception ledger is published with required D-05 fields and valid statuses.",
            "blocking_refs": [],
            "extra_field": "not-allowed",
        },
        {
            "evidence_id": "EV-07",
            "status": "pass",
            "summary": "Human-readable gate decision record is published with QG-04 and handoff notes.",
            "blocking_refs": [],
        },
        {
            "evidence_id": "EV-08",
            "status": "fail",
            "summary": "Machine-readable gate decision remains no-go while BLK-189 blockers are open.",
            "blocking_refs": ["BLK-189-01", "BLK-189-02", "BLK-189-03"],
        },
    ]
    monkeypatch.setattr(generate_quality_gate_decision, "EVIDENCE_ITEMS", malformed_evidence)

    code, stdout, stderr = run_main(
        [
            "--output-md",
            str(output_md),
            "--output-status",
            str(output_status),
            "--generated-at",
            "2026-02-24T00:00:00Z",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "quality-gate-generator: HARD-FAIL" in stderr
    assert "schema mismatch" in stderr
    assert "unexpected keys ['extra_field']" in stderr
    assert not output_md.exists()
    assert not output_status.exists()


def test_subprocess_output_is_deterministic_across_hash_seeds(tmp_path: Path) -> None:
    command_seed_1 = [
        sys.executable,
        str(SCRIPT_PATH),
        "--output-md",
        str(tmp_path / "seed_1.md"),
        "--output-status",
        str(tmp_path / "seed_1.status.json"),
        "--generated-at",
        "2026-02-24T00:00:00Z",
    ]
    command_seed_2 = [
        sys.executable,
        str(SCRIPT_PATH),
        "--output-md",
        str(tmp_path / "seed_2.md"),
        "--output-status",
        str(tmp_path / "seed_2.status.json"),
        "--generated-at",
        "2026-02-24T00:00:00Z",
    ]

    def run_with_seed(command: list[str], seed: str) -> subprocess.CompletedProcess[bytes]:
        env = os.environ.copy()
        env["PYTHONHASHSEED"] = seed
        return subprocess.run(command, capture_output=True, check=False, env=env)

    first = run_with_seed(command_seed_1, "1")
    second = run_with_seed(command_seed_2, "2")

    assert first.returncode == 0, first.stderr.decode("utf-8", errors="replace")
    assert second.returncode == 0, second.stderr.decode("utf-8", errors="replace")
    assert first.stderr == b""
    assert second.stderr == b""
    assert first.stdout == second.stdout

    md_seed_1 = (tmp_path / "seed_1.md").read_bytes()
    md_seed_2 = (tmp_path / "seed_2.md").read_bytes()
    status_seed_1 = (tmp_path / "seed_1.status.json").read_bytes()
    status_seed_2 = (tmp_path / "seed_2.status.json").read_bytes()

    assert md_seed_1 == md_seed_2
    assert status_seed_1 == status_seed_2
    assert b"\r" not in md_seed_1
    assert b"\r" not in status_seed_1
