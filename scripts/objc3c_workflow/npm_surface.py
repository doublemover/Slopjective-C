"""Canonical npm bridge metadata for the objc3c workflow."""

from __future__ import annotations

from .environment import WORKFLOW_RUNNER_MODE, WORKFLOW_RUNNER_SURFACE


def describe_package_script_payload(script_name: str) -> dict[str, object]:
    if script_name == "objc3c":
        return {
            "package_script": "objc3c",
            "action": "<action>",
            "summary": "canonical npm bridge for the objc3c workflow action registry",
            "audience": "operator",
            "category": "bridge",
            "backend": "python -m scripts.objc3c_workflow",
            "validation_tier": "repo",
            "guarantee_owner": "GitHub Actions and local npm users route workflow actions through one package bridge",
            "pass_through_args": True,
            "mode": WORKFLOW_RUNNER_MODE,
            "runner_path": WORKFLOW_RUNNER_SURFACE,
        }
    raise KeyError(script_name)
