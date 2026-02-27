from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "run_open_blocker_audit.py"
SPEC = importlib.util.spec_from_file_location("run_open_blocker_audit", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/run_open_blocker_audit.py for tests.")
run_open_blocker_audit = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = run_open_blocker_audit
SPEC.loader.exec_module(run_open_blocker_audit)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "open_blocker_audit_runner"


def read_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def run_scenario(tmp_path: Path, name: str) -> tuple[int, Path]:
    scenario_root = FIXTURE_ROOT / name
    output_dir = tmp_path / f"{name}_artifacts"
    code = run_open_blocker_audit.main(
        [
            "--audit-root",
            str(scenario_root),
            "--generated-at-utc",
            "2026-02-25T13:30:00Z",
            "--source",
            f"fixture:open-blocker-audit-{name}",
            "--output-dir",
            str(output_dir),
        ]
    )
    return code, output_dir


def test_runner_zero_open_persists_artifacts_and_returns_zero(tmp_path: Path) -> None:
    code, output_dir = run_scenario(tmp_path, "zero_open")

    assert code == 0

    snapshot_path = output_dir / "inputs" / "open_blockers.snapshot.json"
    extract_log_path = output_dir / "extract_open_blockers.log"
    summary_path = output_dir / "open_blocker_audit_summary.json"
    report_path = output_dir / "open_blocker_audit_report.md"
    contract_check_transcript_path = (
        output_dir / run_open_blocker_audit.CONTRACT_CHECK_TRANSCRIPT_FILENAME
    )
    contract_check_stderr_path = (
        output_dir / run_open_blocker_audit.CONTRACT_CHECK_STDERR_FILENAME
    )

    for artifact in (
        snapshot_path,
        extract_log_path,
        summary_path,
        report_path,
        contract_check_transcript_path,
        contract_check_stderr_path,
    ):
        assert artifact.exists()
        assert artifact.is_file()
        assert b"\r" not in artifact.read_bytes()

    snapshot = read_json(snapshot_path)
    assert list(snapshot.keys()) == [
        "contract_id",
        "contract_version",
        "generated_at_utc",
        "source",
        "open_blocker_count",
        "open_blockers",
    ]
    assert snapshot["contract_id"] == run_open_blocker_audit.RUNNER_CONTRACT_ID
    assert snapshot["contract_version"] == run_open_blocker_audit.RUNNER_CONTRACT_VERSION
    assert snapshot["generated_at_utc"] == "2026-02-25T13:30:00Z"
    assert snapshot["source"] == "fixture:open-blocker-audit-zero_open"
    assert snapshot["open_blocker_count"] == 0
    assert snapshot["open_blockers"] == []

    summary = read_json(summary_path)
    assert list(summary.keys()) == [
        "runner",
        "contract_id",
        "contract_version",
        "inputs",
        "scope",
        "artifacts",
        "audit",
        "commands",
        "errors",
        "final_status",
        "final_exit_code",
    ]
    assert summary["runner"] == run_open_blocker_audit.RUNNER_ID
    assert summary["contract_id"] == run_open_blocker_audit.RUNNER_CONTRACT_ID
    assert summary["contract_version"] == run_open_blocker_audit.RUNNER_CONTRACT_VERSION
    assert summary["final_status"] == "ok"
    assert summary["final_exit_code"] == 0

    inputs = summary["inputs"]
    assert isinstance(inputs, dict)
    assert inputs["exclude_paths"] == list(run_open_blocker_audit.DEFAULT_EXCLUDE_PATHS)
    assert inputs["generated_at_utc"] == "2026-02-25T13:30:00Z"
    assert inputs["source"] == "fixture:open-blocker-audit-zero_open"

    scope = summary["scope"]
    assert isinstance(scope, dict)
    assert scope["included_markdown_count"] == 1
    assert scope["excluded_markdown_count"] == 2

    audit = summary["audit"]
    assert isinstance(audit, dict)
    assert audit["extract_attempted"] is True
    assert audit["extract_exit_code"] == 0
    assert audit["open_blocker_count"] == 0

    commands = summary["commands"]
    assert isinstance(commands, dict)
    assert list(commands.keys()) == ["extract_open_blockers_snapshot_json"]
    assert commands["extract_open_blockers_snapshot_json"]["exit_code"] == 0

    extract_log = extract_log_path.read_text(encoding="utf-8")
    assert "# extract_open_blockers snapshot-json command output" in extract_log
    assert "## stderr" in extract_log
    assert "_empty_" in extract_log
    contract_check_transcript = contract_check_transcript_path.read_text(encoding="utf-8")
    assert "# open_blocker_audit contract check transcript" in contract_check_transcript
    assert "python scripts/check_open_blocker_audit_contract.py" in contract_check_transcript
    assert "## stdout" in contract_check_transcript
    assert "## stderr" in contract_check_transcript
    assert "Exit code: 0" in contract_check_transcript
    assert contract_check_stderr_path.read_text(encoding="utf-8") == ""

    report_text = report_path.read_text(encoding="utf-8")
    assert f"- Contract ID: `{run_open_blocker_audit.RUNNER_CONTRACT_ID}`" in report_text
    assert (
        f"- Contract version: `{run_open_blocker_audit.RUNNER_CONTRACT_VERSION}`"
        in report_text
    )
    assert "- Final status: `ok`" in report_text
    assert "- Open blocker count: `0`" in report_text


def test_runner_open_blockers_returns_one_with_canonical_snapshot(tmp_path: Path) -> None:
    code, output_dir = run_scenario(tmp_path, "open_blockers")

    assert code == 1

    snapshot = read_json(output_dir / "inputs" / "open_blockers.snapshot.json")
    assert snapshot["contract_id"] == run_open_blocker_audit.RUNNER_CONTRACT_ID
    assert snapshot["contract_version"] == run_open_blocker_audit.RUNNER_CONTRACT_VERSION
    assert snapshot["open_blocker_count"] == 1
    rows = snapshot["open_blockers"]
    assert isinstance(rows, list)
    assert rows == [
        {
            "blocker_id": "BLK-REAL-01",
            "source_path": "tests/tooling/fixtures/open_blocker_audit_runner/open_blockers/spec/planning/open_blocker.md",
            "line_number": 5,
            "line": 5,
        }
    ]

    summary = read_json(output_dir / "open_blocker_audit_summary.json")
    assert summary["contract_id"] == run_open_blocker_audit.RUNNER_CONTRACT_ID
    assert summary["contract_version"] == run_open_blocker_audit.RUNNER_CONTRACT_VERSION
    assert summary["final_status"] == "open-blockers"
    assert summary["final_exit_code"] == 1

    audit = summary["audit"]
    assert isinstance(audit, dict)
    assert audit["open_blocker_count"] == 1
    assert audit["extract_exit_code"] == 0

    report_text = (output_dir / "open_blocker_audit_report.md").read_text(encoding="utf-8")
    assert "- Final status: `open-blockers`" in report_text
    assert "- Open blocker count: `1`" in report_text
    contract_check_transcript = (
        output_dir / run_open_blocker_audit.CONTRACT_CHECK_TRANSCRIPT_FILENAME
    ).read_text(encoding="utf-8")
    assert "Exit code: 0" in contract_check_transcript
    assert (
        output_dir / run_open_blocker_audit.CONTRACT_CHECK_STDERR_FILENAME
    ).read_text(encoding="utf-8") == ""


def test_include_glob_scopes_effective_root_for_extractor_command(tmp_path: Path) -> None:
    scenario_root = FIXTURE_ROOT / "open_blockers"
    output_dir = tmp_path / "include_glob_artifacts"

    code = run_open_blocker_audit.main(
        [
            "--audit-root",
            str(scenario_root),
            "--include-glob",
            "spec/planning/**/*.md",
            "--generated-at-utc",
            "2026-02-25T14:00:00Z",
            "--source",
            "fixture:include-glob-scope",
            "--output-dir",
            str(output_dir),
        ]
    )

    assert code == 1
    summary = read_json(output_dir / "open_blocker_audit_summary.json")
    inputs = summary["inputs"]
    assert isinstance(inputs, dict)
    assert inputs["include_globs"] == ["spec/planning/**/*.md"]
    assert inputs["effective_audit_root"] == (
        "tests/tooling/fixtures/open_blocker_audit_runner/open_blockers/spec/planning"
    )

    commands = summary["commands"]
    assert isinstance(commands, dict)
    argv = commands["extract_open_blockers_snapshot_json"]["argv"]
    assert isinstance(argv, list)
    assert "--root" in argv
    assert (
        "tests/tooling/fixtures/open_blocker_audit_runner/open_blockers/spec/planning"
        in argv
    )


def test_runner_fails_closed_when_extract_command_fails(tmp_path: Path, monkeypatch) -> None:
    audit_root = tmp_path / "scope"
    (audit_root / "spec" / "planning").mkdir(parents=True, exist_ok=True)
    (audit_root / "spec" / "planning" / "clean.md").write_text(
        "# Empty\n",
        encoding="utf-8",
    )
    output_dir = tmp_path / "artifacts"

    command_specs: list[run_open_blocker_audit.CommandSpec] = []

    def fake_run_command(
        spec: run_open_blocker_audit.CommandSpec,
    ) -> run_open_blocker_audit.CommandResult:
        command_specs.append(spec)
        return run_open_blocker_audit.CommandResult(
            spec=spec,
            exit_code=2,
            stdout="",
            stderr="error: markdown file is not valid UTF-8: scope/spec/planning/clean.md\n",
        )

    monkeypatch.setattr(run_open_blocker_audit, "run_command", fake_run_command)

    code = run_open_blocker_audit.main(
        [
            "--audit-root",
            str(audit_root),
            "--generated-at-utc",
            "2026-02-25T13:30:00Z",
            "--source",
            "fixture:extract-fail",
            "--output-dir",
            str(output_dir),
        ]
    )

    assert code == 2
    assert [spec.name for spec in command_specs] == [
        "extract_open_blockers_snapshot_json",
        "check_open_blocker_audit_contract",
    ]

    summary = read_json(output_dir / "open_blocker_audit_summary.json")
    assert summary["contract_id"] == run_open_blocker_audit.RUNNER_CONTRACT_ID
    assert summary["contract_version"] == run_open_blocker_audit.RUNNER_CONTRACT_VERSION
    assert summary["final_status"] == "runner-error"
    assert summary["final_exit_code"] == 2

    commands = summary["commands"]
    assert isinstance(commands, dict)
    assert commands["extract_open_blockers_snapshot_json"]["exit_code"] == 2

    errors = summary["errors"]
    assert isinstance(errors, list)
    assert any("returned unexpected exit code 2" in entry for entry in errors)

    assert (output_dir / "extract_open_blockers.log").exists()
    assert (
        output_dir / run_open_blocker_audit.CONTRACT_CHECK_TRANSCRIPT_FILENAME
    ).exists()
    assert (
        output_dir / run_open_blocker_audit.CONTRACT_CHECK_STDERR_FILENAME
    ).exists()
    assert not (output_dir / "inputs" / "open_blockers.snapshot.json").exists()


def test_runner_fails_closed_when_contract_check_fails(
    tmp_path: Path,
    monkeypatch,
) -> None:
    audit_root = tmp_path / "scope"
    (audit_root / "spec" / "planning").mkdir(parents=True, exist_ok=True)
    (audit_root / "spec" / "planning" / "clean.md").write_text(
        "# Empty\n",
        encoding="utf-8",
    )
    output_dir = tmp_path / "artifacts"

    command_specs: list[run_open_blocker_audit.CommandSpec] = []

    extract_stdout = json.dumps(
        {
            "generated_at_utc": "2026-02-25T13:30:00Z",
            "source": "fixture:contract-check-fail",
            "open_blocker_count": 0,
            "open_blockers": [],
        },
        indent=2,
    ) + "\n"

    def fake_run_command(
        spec: run_open_blocker_audit.CommandSpec,
    ) -> run_open_blocker_audit.CommandResult:
        command_specs.append(spec)
        if spec.name == "extract_open_blockers_snapshot_json":
            return run_open_blocker_audit.CommandResult(
                spec=spec,
                exit_code=0,
                stdout=extract_stdout,
                stderr="",
            )
        if spec.name == "check_open_blocker_audit_contract":
            return run_open_blocker_audit.CommandResult(
                spec=spec,
                exit_code=2,
                stdout="",
                stderr="open-blocker-audit-contract: contract drift detected (1 finding(s)).\n",
            )
        raise AssertionError(f"unexpected command spec name: {spec.name}")

    monkeypatch.setattr(run_open_blocker_audit, "run_command", fake_run_command)

    code = run_open_blocker_audit.main(
        [
            "--audit-root",
            str(audit_root),
            "--generated-at-utc",
            "2026-02-25T13:30:00Z",
            "--source",
            "fixture:contract-check-fail",
            "--output-dir",
            str(output_dir),
        ]
    )

    assert code == 2
    assert [spec.name for spec in command_specs] == [
        "extract_open_blockers_snapshot_json",
        "check_open_blocker_audit_contract",
    ]

    summary = read_json(output_dir / "open_blocker_audit_summary.json")
    assert summary["final_status"] == "runner-error"
    assert summary["final_exit_code"] == 2

    errors = summary["errors"]
    assert isinstance(errors, list)
    assert any(
        "check_open_blocker_audit_contract returned unexpected exit code 2" in entry
        for entry in errors
    )

    transcript = (
        output_dir / run_open_blocker_audit.CONTRACT_CHECK_TRANSCRIPT_FILENAME
    ).read_text(encoding="utf-8")
    assert "python scripts/check_open_blocker_audit_contract.py" in transcript
    assert "Exit code: 2" in transcript

    stderr_text = (
        output_dir / run_open_blocker_audit.CONTRACT_CHECK_STDERR_FILENAME
    ).read_text(encoding="utf-8")
    assert (
        stderr_text
        == "open-blocker-audit-contract: contract drift detected (1 finding(s)).\n"
    )


def test_default_excludes_block_utf8_failure_from_tests_tree(tmp_path: Path) -> None:
    audit_root = tmp_path / "repo_root"
    (audit_root / "spec" / "planning").mkdir(parents=True, exist_ok=True)
    (audit_root / "tests" / "tooling" / "fixtures" / "excluded").mkdir(
        parents=True, exist_ok=True
    )
    output_dir = tmp_path / "artifacts"

    (audit_root / "spec" / "planning" / "clean.md").write_text(
        "# Clean fixture\n",
        encoding="utf-8",
    )
    (audit_root / "tests" / "tooling" / "fixtures" / "excluded" / "report.md").write_bytes(
        b"\xff\xfe\xfd\x00"
    )

    code = run_open_blocker_audit.main(
        [
            "--audit-root",
            str(audit_root),
            "--generated-at-utc",
            "2026-02-25T13:30:00Z",
            "--source",
            "fixture:repo-root-open-blocker-audit",
            "--output-dir",
            str(output_dir),
        ]
    )

    assert code == 0

    summary = read_json(output_dir / "open_blocker_audit_summary.json")
    assert summary["final_status"] == "ok"
    assert summary["final_exit_code"] == 0
    assert summary["errors"] == []

    scope = summary["scope"]
    assert isinstance(scope, dict)
    assert scope["included_markdown_count"] == 1
    assert scope["excluded_markdown_count"] == 1

    extract_log = (output_dir / "extract_open_blockers.log").read_text(encoding="utf-8")
    assert "markdown file is not valid UTF-8" not in extract_log
