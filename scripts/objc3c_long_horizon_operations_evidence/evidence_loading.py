from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.subprocesses import run_timed

from objc3c_long_horizon_operations_evidence.paths import LongHorizonEvidencePaths
from objc3c_long_horizon_operations_evidence.validation import (
    require_passing_report,
    require_report_file,
    require_support_artifact,
)


@dataclass(frozen=True)
class LongHorizonEvidenceInputs:
    steps: list[dict[str, object]]
    reports: dict[str, dict[str, Any]]
    update_manifest: dict[str, Any]
    upgrade_support_report: dict[str, Any]
    failures: list[str]


def run_step(name: str, command: list[str], paths: LongHorizonEvidencePaths) -> dict[str, object]:
    result = run_timed(command, cwd=paths.root, echo=True)
    return {
        "name": name,
        "command": command,
        "exit_code": result.returncode,
        "duration_ms": result.duration_ms,
        "stdout": result.stdout,
        "stderr": result.stderr,
    }


def run_generation_steps(paths: LongHorizonEvidencePaths, failures: list[str]) -> list[dict[str, object]]:
    steps: list[dict[str, object]] = []
    for step_spec in paths.steps():
        step = run_step(step_spec.name, step_spec.command, paths)
        steps.append(step)
        if step["exit_code"] != 0:
            failures.append(f"{step_spec.name} failed")
            break
    return steps


def load_required_reports(paths: LongHorizonEvidencePaths, failures: list[str]) -> dict[str, dict[str, Any]]:
    for report in paths.required_reports():
        require_report_file(report.path, report.name, failures, root=paths.root)

    reports = {
        report.name: load_json(report.path)
        for report in paths.required_reports()
        if report.path.is_file()
    }
    for name, payload in reports.items():
        require_passing_report(name, payload, failures)
    return reports


def load_optional_json(path) -> dict[str, Any]:
    return load_json(path) if path.is_file() else {}


def load_long_horizon_inputs(paths: LongHorizonEvidencePaths) -> LongHorizonEvidenceInputs:
    failures: list[str] = []
    steps = run_generation_steps(paths, failures)
    reports = load_required_reports(paths, failures)

    update_manifest = load_optional_json(paths.update_manifest)
    upgrade_support_report = load_optional_json(paths.upgrade_support_report)
    require_support_artifact(paths.update_manifest, update_manifest, "update manifest", failures, root=paths.root)
    require_support_artifact(
        paths.upgrade_support_report,
        upgrade_support_report,
        "upgrade support report",
        failures,
        root=paths.root,
    )

    return LongHorizonEvidenceInputs(
        steps=steps,
        reports=reports,
        update_manifest=update_manifest,
        upgrade_support_report=upgrade_support_report,
        failures=failures,
    )
