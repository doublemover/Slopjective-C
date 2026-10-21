"""Validation timing changed-path classification."""

from __future__ import annotations

import subprocess
import os
from collections.abc import Sequence

from ..environment import ROOT, WORKFLOW_COMMAND_TEXT
from .validation_timing_profile_rules import VALIDATION_PROFILE_RULES


def _git(*args: str) -> subprocess.CompletedProcess[str]:
    return subprocess.run(["git", *args], cwd=ROOT, check=False, text=True, capture_output=True)


def _integration_base() -> str | None:
    # The integration point may be explicitly pinned by a caller/CI job.
    configured = os.environ.get("OBJC3C_INTEGRATION_BASE", "").strip()
    if configured:
        if _git("rev-parse", "--verify", f"{configured}^{{commit}}").returncode == 0:
            return configured
        return None
    for candidate in ("origin/main", "main"):
        if _git("rev-parse", "--verify", f"{candidate}^{{commit}}").returncode == 0:
            merge_base = _git("merge-base", "HEAD", candidate)
            if merge_base.returncode == 0:
                return merge_base.stdout.strip()
    return None


def git_changed_paths() -> list[str]:
    paths: set[str] = set()
    base = _integration_base()
    if base:
        result = _git("diff", "--no-renames", "--name-only", "--diff-filter=ACDMRTUXB", "-z", f"{base}...HEAD", "--")
        if result.returncode == 0:
            paths.update(p.replace("\\", "/") for p in result.stdout.split("\0") if p)
        else:
            paths.add("*")
    else:
        # An unknown baseline means classify the repository conservatively.
        paths.add("*")

    staged = _git("diff", "--cached", "--no-renames", "--name-only", "--diff-filter=ACDMRTUXB", "-z", "--")
    worktree = _git("diff", "HEAD", "--no-renames", "--name-only", "--diff-filter=ACDMRTUXB", "-z", "--")
    untracked = _git("ls-files", "--others", "--exclude-standard", "-z")
    for result in (staged, worktree, untracked):
        if result.returncode != 0:
            paths.add("*")
            continue
        paths.update(p.replace("\\", "/") for p in result.stdout.split("\0") if p)
    return sorted(paths)


def select_validation_profiles(paths: Sequence[str]) -> dict[str, object]:
    matched_profiles: list[dict[str, object]] = []
    for profile_name, rule in VALIDATION_PROFILE_RULES.items():
        prefixes = rule.get("path_prefixes", ())
        matched_paths = [path for path in paths if path == "*" or any(path.startswith(str(prefix)) for prefix in prefixes)]
        if not matched_paths:
            continue
        matched_profiles.append({
            "profile": profile_name,
            "matched_paths": matched_paths[:20],
            "recommended_actions": list(rule.get("recommended_actions", ())),
            "exhaustive_actions": list(rule.get("exhaustive_actions", ())),
            "deferred_actions": list(rule.get("deferred_actions", ())),
            "profile_owner": rule.get("profile_owner"),
            "source_owner": rule.get("source_owner"),
            "hard_blocking_decision_owner": rule.get("hard_blocking_decision_owner"),
        })
    known_prefixes = [str(prefix) for rule in VALIDATION_PROFILE_RULES.values()
                      for prefix in rule.get("path_prefixes", ())]
    unknown_paths = [path for path in paths
                     if path == "*" or not any(path.startswith(prefix) for prefix in known_prefixes)]
    if unknown_paths:
        matched_profiles.append({
            "profile": "repo",
            "matched_paths": unknown_paths[:20],
            "recommended_actions": ("test-smoke", "check-task-hygiene"),
            "exhaustive_actions": ("test-full", "test-nightly"),
            "deferred_actions": ("nightly release and stress fan-out",),
            "profile_owner": "validation_timing_changed_paths",
            "source_owner": "validation_timing_changed_paths",
            "hard_blocking_decision_owner": "validation_timing_budgets",
        })
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
