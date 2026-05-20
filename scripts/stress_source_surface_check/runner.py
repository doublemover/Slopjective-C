"""Stress source-surface validation entrypoint."""

from __future__ import annotations

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_report_json
from objc3c_tooling.paths import repo_rel

from .constants import CLAIM_GATE, SUMMARY_PATH, SURFACE_PATH, WORKFLOW_SURFACE
from .failures import StressSourceSurfaceError, fail
from .metadata import validate_surface_metadata
from .paths import require_path
from .summary import build_summary
from .surface_validators import (
    collect_family_summaries,
    validate_artifact_surface,
    validate_checked_in_roots,
    validate_claim_gate,
    validate_safety_policy,
    validate_workflow_surface,
)


def main() -> int:
    try:
        if not SURFACE_PATH.is_file():
            return fail(f"missing stress source surface contract: {repo_rel(SURFACE_PATH)}")

        surface = load_json(SURFACE_PATH)
        validate_surface_metadata(surface)

        require_path("docs/runbooks/objc3c_stress_validation.md", kind="runbook")
        require_path("tests/tooling/fixtures/stress/README.md", kind="stress README")
        safety_policy_path = require_path("tests/tooling/fixtures/stress/safety_policy.json", kind="stress safety policy")
        artifact_surface_path = require_path("tests/tooling/fixtures/stress/artifact_surface.json", kind="stress artifact surface")
        workflow_surface_path = require_path(WORKFLOW_SURFACE, kind="stress workflow surface")
        claim_gate_path = require_path(CLAIM_GATE, kind="stress claim gate")

        safety_policy = load_json(safety_policy_path)
        artifact_surface = load_json(artifact_surface_path)
        workflow_surface = load_json(workflow_surface_path)
        claim_gate = load_json(claim_gate_path)
        validate_safety_policy(safety_policy)
        validate_artifact_surface(artifact_surface)
        validate_workflow_surface(workflow_surface, surface)
        claim_summaries = validate_claim_gate(
            claim_gate,
            surface=surface,
            artifact_surface=artifact_surface,
            workflow_surface=workflow_surface,
        )

        checked_in_roots = validate_checked_in_roots(surface)
        family_summaries = collect_family_summaries(surface)
        required_actions = workflow_surface.get("required_actions", [])
        summary = build_summary(
            surface=surface,
            safety_policy=safety_policy,
            artifact_surface=artifact_surface,
            safety_policy_path=safety_policy_path,
            artifact_surface_path=artifact_surface_path,
            workflow_surface_path=workflow_surface_path,
            claim_gate_path=claim_gate_path,
            checked_in_roots=checked_in_roots,
            required_actions=required_actions,
            family_summaries=family_summaries,
            claim_summaries=claim_summaries,
        )
        write_report_json(SUMMARY_PATH, summary, sort_keys=False)
        print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
        print("stress-source-surface: OK")
        return 0
    except (RuntimeError, StressSourceSurfaceError) as exc:
        return fail(str(exc))
