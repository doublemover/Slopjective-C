"""Validation timing changed-path classification."""

from __future__ import annotations

import subprocess
from collections.abc import Sequence

from ..environment import ROOT, WORKFLOW_COMMAND_TEXT
from .validation_timing_profile_rules import VALIDATION_PROFILE_RULES


def git_changed_paths() -> list[str]:
    commands = (
        ["git", "diff", "--name-only", "HEAD", "--"],
        ["git", "ls-files", "--others", "--exclude-standard"],
    )
    changed: list[str] = []
    for command in commands:
        result = subprocess.run(
            command,
            cwd=ROOT,
            check=False,
            text=True,
            capture_output=True,
        )
        if result.returncode != 0:
            continue
        changed.extend(
            line.strip().replace("\\", "/")
            for line in result.stdout.splitlines()
            if line.strip()
        )
    return sorted(dict.fromkeys(changed))


def select_validation_profiles(paths: Sequence[str]) -> dict[str, object]:
    matched_profiles: list[dict[str, object]] = []
    for profile_name, rule in VALIDATION_PROFILE_RULES.items():
        prefixes = rule.get("path_prefixes", ())
        matched_paths = [
            path
            for path in paths
            if any(path.startswith(str(prefix)) for prefix in prefixes)
        ]
        if not matched_paths:
            continue
        matched_profiles.append(
            {
                "profile": profile_name,
                "matched_paths": matched_paths[:20],
                "recommended_actions": list(rule.get("recommended_actions", ())),
                "exhaustive_actions": list(rule.get("exhaustive_actions", ())),
                "deferred_actions": list(rule.get("deferred_actions", ())),
                "profile_owner": rule.get("profile_owner"),
                "source_owner": rule.get("source_owner"),
                "hard_blocking_decision_owner": rule.get(
                    "hard_blocking_decision_owner"
                ),
            }
        )
    if not matched_profiles and paths:
        matched_profiles.append(
            {
                "profile": "repo",
                "matched_paths": list(paths)[:20],
                "recommended_actions": ("test-smoke", "check-task-hygiene"),
                "exhaustive_actions": ("test-full", "test-nightly"),
                "deferred_actions": ("nightly release and stress fan-out",),
                "profile_owner": "validation_timing_changed_paths",
                "source_owner": "validation_timing_changed_paths",
                "hard_blocking_decision_owner": "validation_timing_budgets",
            }
        )
    return {
        "changed_paths": list(paths),
        "profiles": matched_profiles,
        "manual_override": {
            "smoke": f"{WORKFLOW_COMMAND_TEXT} test-smoke",
            "full": f"{WORKFLOW_COMMAND_TEXT} test-full",
            "nightly": f"{WORKFLOW_COMMAND_TEXT} test-nightly",
        },
        "owners": {
            "source_owner": "validation_timing_changed_paths",
            "profile_owner": "validation_timing_profile_rules",
            "command_owner": "test_orchestration_commands",
            "hard_blocking_decision_owner": "validation_timing_budgets",
        },
    }
