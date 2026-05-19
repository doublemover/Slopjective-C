from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.subprocesses import run_timed

from objc3c_adoption_legibility_evidence.paths import AdoptionLegibilityEvidencePaths
from objc3c_adoption_legibility_evidence.validation import require_passing_report, require_report_file


@dataclass(frozen=True)
class AdoptionLegibilityEvidenceInputs:
    steps: list[dict[str, object]]
    reports: dict[str, dict[str, Any]]
    boundary: dict[str, Any]
    public_claim_policy: dict[str, Any]
    comparison_semantics: dict[str, Any]
    adoption_replay_semantics: dict[str, Any]
    artifact_contract: dict[str, Any]
    failures: list[str]


def run_step(
    name: str,
    command: list[str],
    paths: AdoptionLegibilityEvidencePaths | None = None,
) -> dict[str, object]:
    resolved_paths = paths or AdoptionLegibilityEvidencePaths.for_root()
    result = run_timed(command, cwd=resolved_paths.root, echo=True)
    return {
        "name": name,
        "command": command,
        "exit_code": result.returncode,
        "duration_ms": result.duration_ms,
        "stdout": result.stdout,
        "stderr": result.stderr,
    }


def run_generation_steps(paths: AdoptionLegibilityEvidencePaths, failures: list[str]) -> list[dict[str, object]]:
    steps: list[dict[str, object]] = []
    for step_spec in paths.steps():
        step = run_step(step_spec.name, step_spec.command, paths)
        steps.append(step)
        if step["exit_code"] != 0:
            failures.append(f"{step_spec.name} failed")
            break
    return steps


def load_required_reports(paths: AdoptionLegibilityEvidencePaths, failures: list[str]) -> dict[str, dict[str, Any]]:
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


def load_contracts(paths: AdoptionLegibilityEvidencePaths) -> dict[str, dict[str, Any]]:
    return {
        "boundary": load_json(paths.boundary_contract),
        "public_claim_policy": load_json(paths.public_claim_policy),
        "comparison_semantics": load_json(paths.comparison_semantics),
        "adoption_replay_semantics": load_json(paths.adoption_replay_semantics),
        "artifact_contract": load_json(paths.artifact_contract),
    }


def load_adoption_legibility_inputs(paths: AdoptionLegibilityEvidencePaths) -> AdoptionLegibilityEvidenceInputs:
    failures: list[str] = []
    steps = run_generation_steps(paths, failures)
    reports = load_required_reports(paths, failures)
    contracts = load_contracts(paths)

    return AdoptionLegibilityEvidenceInputs(
        steps=steps,
        reports=reports,
        boundary=contracts["boundary"],
        public_claim_policy=contracts["public_claim_policy"],
        comparison_semantics=contracts["comparison_semantics"],
        adoption_replay_semantics=contracts["adoption_replay_semantics"],
        artifact_contract=contracts["artifact_contract"],
        failures=failures,
    )
