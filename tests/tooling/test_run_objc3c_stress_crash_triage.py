from __future__ import annotations

import importlib.util
import json
import shutil
import sys
from pathlib import Path

import pytest

from scripts.objc3c_tooling.artifact_identity import current_host_artifact_identity


ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))
SCRIPT_PATH = ROOT / "scripts" / "run_objc3c_stress_crash_triage.py"
SIGNATURE_SHA256 = "a" * 64
ARTIFACT_IDENTITY = current_host_artifact_identity()


def _load_runner():
    spec = importlib.util.spec_from_file_location(
        "run_objc3c_stress_crash_triage",
        SCRIPT_PATH,
    )
    if spec is None or spec.loader is None:
        raise RuntimeError("Unable to load scripts/run_objc3c_stress_crash_triage.py")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def _write_json(path: Path, payload: dict[str, object]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def _repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def _write_valid_fixture(run_root: Path) -> tuple[Path, Path, Path]:
    fixture_id = run_root.name
    failure_dir = (
        ROOT
        / "tmp"
        / "artifacts"
        / "stress"
        / "failures"
        / f"crash-triage-test-{fixture_id}"
        / "parser-stability"
    )
    minimized_dir = (
        ROOT
        / "tmp"
        / "artifacts"
        / "stress"
        / "minimized"
        / f"crash-triage-test-{fixture_id}"
        / "parser-stability"
    )
    failure_dir.mkdir(parents=True, exist_ok=True)
    minimized_dir.mkdir(parents=True, exist_ok=True)

    (failure_dir / "source.objc3").write_text("@interface Broken\n", encoding="utf-8")
    _write_json(
        failure_dir / "invocation.json",
        {"compiler": ARTIFACT_IDENTITY.native_executable_relative_path},
    )
    _write_json(
        failure_dir / "failure-summary.json",
        {
            "case_id": "parser-stability",
            "subsystem": "parser",
            "returncode": 1,
            "diagnostic_lines": ["error: expected @end"],
            "signature_sha256": SIGNATURE_SHA256,
        },
    )
    _write_json(
        failure_dir / "stable-signature.json",
        {
            "returncode": 1,
            "diagnostic_lines": ["error: expected @end"],
        },
    )

    (minimized_dir / "candidate.objc3").write_text("@interface Broken\n", encoding="utf-8")
    _write_json(minimized_dir / "reducer-plan.json", {"case_id": "parser-stability"})
    _write_json(
        minimized_dir / "reduced-summary.json",
        {
            "case_id": "parser-stability",
            "original_bytes": 32,
            "reduced_bytes": 18,
            "signature_sha256": SIGNATURE_SHA256,
        },
    )

    artifact_surface = run_root / "artifact_surface.json"
    _write_json(
        artifact_surface,
        {
            "contract_id": "objc3c.stress.artifact.surface.v1",
            "schema_version": 1,
            "machine_owned_artifact_roots": [
                "tmp/artifacts/stress/failures",
                "tmp/artifacts/stress/minimized",
                "tmp/artifacts/stress/replays",
                "tmp/artifacts/stress/triage",
            ],
            "summary_reports": {
                "minimization": "tmp/reports/stress/minimization-summary.json",
                "crash_triage": "tmp/reports/stress/crash-triage-summary.json",
            },
            "failure_capsule_required_artifacts": [
                "source.objc3",
                "invocation.json",
                "failure-summary.json",
                "stable-signature.json",
            ],
            "reducer_session_required_artifacts": [
                "candidate.objc3",
                "reducer-plan.json",
                "reduced-summary.json",
            ],
            "triage_required_artifacts": [
                "signature-index.json",
                "case-index.json",
                "triage-summary.json",
            ],
        },
    )
    minimization_summary = run_root / "minimization-summary.json"
    _write_json(
        minimization_summary,
        {
            "contract_id": "objc3c.stress.minimization.summary.v1",
            "status": "PASS",
            "case_summaries": [
                {
                    "case_id": "parser-stability",
                    "failure_dir": _repo_rel(failure_dir),
                    "minimized_dir": _repo_rel(minimized_dir),
                    "signature_sha256": SIGNATURE_SHA256,
                }
            ],
        },
    )
    return artifact_surface, minimization_summary, run_root / "crash-triage-summary.json"


def _remove_fixture_artifact_roots(run_root: Path) -> None:
    for root_name in ("failures", "minimized"):
        shutil.rmtree(
            ROOT
            / "tmp"
            / "artifacts"
            / "stress"
            / root_name
            / f"crash-triage-test-{run_root.name}",
            ignore_errors=True,
        )


def _remove_generated_summary_roots(summary: dict[str, object]) -> None:
    for key in ("triage_root", "replay_root"):
        value = summary.get(key)
        if isinstance(value, str) and value:
            shutil.rmtree(ROOT / value, ignore_errors=True)


def test_stress_crash_triage_writes_replayable_signature_indexes() -> None:
    runner = _load_runner()
    run_root = ROOT / "tmp" / "tests" / "stress-crash-triage" / "valid"
    shutil.rmtree(run_root, ignore_errors=True)
    artifact_surface, minimization_summary, summary_out = _write_valid_fixture(run_root)

    try:
        exit_code = runner.main(
            [
                "--artifact-surface",
                str(artifact_surface),
                "--minimization-summary",
                str(minimization_summary),
                "--summary-out",
                str(summary_out),
            ]
        )

        assert exit_code == 0
        summary = json.loads(summary_out.read_text(encoding="utf-8"))
        assert summary["contract_id"] == "objc3c.stress.crash.triage.summary.v1"
        assert summary["status"] == "PASS"
        assert summary["fixture_manifest_contract_id"] == (
            "objc3c.stress.crash.triage.fixture.manifest.v1"
        )
        assert summary["signature_count"] == 1
        assert summary["case_count"] == 1
        assert summary["parser_case_count"] == 1
        assert summary["semantic_case_count"] == 0
        assert summary["runtime_case_count"] == 0
        assert summary["execution_case_count"] == 0
        assert summary["runtime_execution_case_count"] == 0
        assert summary["replay_request_count"] == 1

        case_index_path = ROOT / summary["case_index_path"]
        case_index = json.loads(case_index_path.read_text(encoding="utf-8"))
        assert case_index[0]["case_id"] == "parser-stability"
        assert case_index[0]["subsystem"] == "parser"
        assert case_index[0]["signature_sha256"] == SIGNATURE_SHA256
        replay_request = json.loads(
            (ROOT / case_index[0]["replay_request_path"]).read_text(encoding="utf-8")
        )
        assert replay_request["subsystem"] == "parser"
    finally:
        if summary_out.exists():
            _remove_generated_summary_roots(json.loads(summary_out.read_text(encoding="utf-8")))
        _remove_fixture_artifact_roots(run_root)
        shutil.rmtree(run_root, ignore_errors=True)


def test_stress_crash_triage_fixture_manifest_validates_checked_in_cases() -> None:
    runner = _load_runner()
    manifest = runner.load_json(runner.FIXTURE_MANIFEST_PATH)

    summary = runner.validate_fixture_manifest(manifest)

    assert summary == {
        "contract_id": "objc3c.stress.crash.triage.fixture.manifest.v1",
        "positive_case_count": 2,
        "negative_case_count": 2,
        "positive_case_subsystem_counts": {
            "parser": 1,
            "semantic": 0,
            "runtime": 0,
            "execution": 1,
            "runtime_execution": 1,
        },
        "positive_runtime_execution_case_count": 1,
    }


def test_stress_crash_triage_rejects_missing_case_signature() -> None:
    runner = _load_runner()
    run_root = ROOT / "tmp" / "tests" / "stress-crash-triage" / "missing-signature"
    shutil.rmtree(run_root, ignore_errors=True)
    artifact_surface, minimization_summary, summary_out = _write_valid_fixture(run_root)
    payload = json.loads(minimization_summary.read_text(encoding="utf-8"))
    del payload["case_summaries"][0]["signature_sha256"]
    _write_json(minimization_summary, payload)

    try:
        with pytest.raises(RuntimeError, match="missing signature_sha256"):
            runner.main(
                [
                    "--artifact-surface",
                    str(artifact_surface),
                    "--minimization-summary",
                    str(minimization_summary),
                    "--summary-out",
                    str(summary_out),
                ]
            )
        assert not summary_out.exists()
    finally:
        _remove_fixture_artifact_roots(run_root)
        shutil.rmtree(run_root, ignore_errors=True)


def test_stress_crash_triage_rejects_invalid_case_signature() -> None:
    runner = _load_runner()
    run_root = ROOT / "tmp" / "tests" / "stress-crash-triage" / "invalid-signature"
    shutil.rmtree(run_root, ignore_errors=True)
    artifact_surface, minimization_summary, summary_out = _write_valid_fixture(run_root)
    payload = json.loads(minimization_summary.read_text(encoding="utf-8"))
    payload["case_summaries"][0]["signature_sha256"] = "not-a-sha"
    _write_json(minimization_summary, payload)

    try:
        with pytest.raises(RuntimeError, match="invalid signature_sha256"):
            runner.main(
                [
                    "--artifact-surface",
                    str(artifact_surface),
                    "--minimization-summary",
                    str(minimization_summary),
                    "--summary-out",
                    str(summary_out),
                ]
            )
        assert not summary_out.exists()
    finally:
        _remove_fixture_artifact_roots(run_root)
        shutil.rmtree(run_root, ignore_errors=True)


def test_stress_crash_triage_rejects_artifacts_outside_machine_owned_roots() -> None:
    runner = _load_runner()
    run_root = ROOT / "tmp" / "tests" / "stress-crash-triage" / "outside-machine-root"
    shutil.rmtree(run_root, ignore_errors=True)
    artifact_surface, minimization_summary, summary_out = _write_valid_fixture(run_root)
    payload = json.loads(minimization_summary.read_text(encoding="utf-8"))
    outside_dir = run_root / "checked-in-looking-failure"
    outside_dir.mkdir(parents=True, exist_ok=True)
    payload["case_summaries"][0]["failure_dir"] = _repo_rel(outside_dir)
    _write_json(minimization_summary, payload)

    try:
        with pytest.raises(RuntimeError, match="outside machine-owned artifact roots"):
            runner.main(
                [
                    "--artifact-surface",
                    str(artifact_surface),
                    "--minimization-summary",
                    str(minimization_summary),
                    "--summary-out",
                    str(summary_out),
                ]
            )
        assert not summary_out.exists()
    finally:
        _remove_fixture_artifact_roots(run_root)
        shutil.rmtree(run_root, ignore_errors=True)


def test_stress_crash_triage_rejects_reducer_growth() -> None:
    runner = _load_runner()
    run_root = ROOT / "tmp" / "tests" / "stress-crash-triage" / "reducer-growth"
    shutil.rmtree(run_root, ignore_errors=True)
    artifact_surface, minimization_summary, summary_out = _write_valid_fixture(run_root)
    payload = json.loads(minimization_summary.read_text(encoding="utf-8"))
    minimized_dir = ROOT / payload["case_summaries"][0]["minimized_dir"]
    reduced_summary = json.loads((minimized_dir / "reduced-summary.json").read_text(encoding="utf-8"))
    reduced_summary["reduced_bytes"] = reduced_summary["original_bytes"] + 1
    _write_json(minimized_dir / "reduced-summary.json", reduced_summary)

    try:
        with pytest.raises(RuntimeError, match="grew beyond original bytes"):
            runner.main(
                [
                    "--artifact-surface",
                    str(artifact_surface),
                    "--minimization-summary",
                    str(minimization_summary),
                    "--summary-out",
                    str(summary_out),
                ]
            )
        assert not summary_out.exists()
    finally:
        _remove_fixture_artifact_roots(run_root)
        shutil.rmtree(run_root, ignore_errors=True)
