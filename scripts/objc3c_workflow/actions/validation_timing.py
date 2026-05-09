"""Validation timing report helpers and actions."""

from __future__ import annotations

import json
import subprocess
from datetime import datetime, timezone
from pathlib import Path
from typing import Sequence

from ..environment import ROOT, WORKFLOW_COMMAND_TEXT, WORKFLOW_RUNNER_SURFACE
from .validation_timing_budgets import (
    validation_budget_violations,
    validation_speed_budget_mode,
    validation_speed_budgets,
)
from .validation_timing_reports import (
    dashboard_section_from_report,
    latest_json_file,
    load_child_reports,
    load_latest_report_payload,
    load_surface_from_report,
    relative_path_or_none,
    safe_float,
    summarize_execution_replay_report,
    summarize_execution_smoke_report,
    summarize_runtime_acceptance_report,
)

PUBLIC_WORKFLOW_REPORT_ROOT = ROOT / "tmp" / "reports" / "objc3c-public-workflow"


def collect_child_timing(
    steps: Sequence[dict[str, object]]
) -> dict[str, object]:
    child_reports = load_child_reports(steps)
    runtime_acceptance: dict[str, object] | None = None
    execution_smoke: dict[str, object] | None = None
    execution_replay: dict[str, object] | None = None
    for report in child_reports:
        payload = report["payload"]
        if not isinstance(payload, dict):
            continue
        path = str(report["path"])
        if "default_compile_backend" in payload or "case_count" in payload:
            runtime_acceptance = summarize_runtime_acceptance_report(
                path,
                payload,
                report_reused=bool(report.get("report_reused", False)),
            )
        elif "proof_run_id" in payload:
            execution_replay = summarize_execution_replay_report(path, payload)
        elif "compile_command" in payload and "results" in payload:
            execution_smoke = summarize_execution_smoke_report(path, payload)
    total_step_seconds = round(
        sum(safe_float(step.get("duration_seconds", 0.0)) for step in steps),
        6,
    )
    estimated_no_skip_seconds = total_step_seconds
    if runtime_acceptance is not None and runtime_acceptance.get("report_reused"):
        for step in steps:
            if step.get("action") == "test-runtime-acceptance":
                estimated_no_skip_seconds -= safe_float(step.get("duration_seconds", 0.0))
                estimated_no_skip_seconds += safe_float(
                    runtime_acceptance.get("elapsed_seconds", 0.0)
                )
                break
    estimated_no_skip_seconds = round(estimated_no_skip_seconds, 6)
    return {
        "child_report_paths": [str(report["path"]) for report in child_reports],
        "runtime_acceptance": runtime_acceptance,
        "execution_smoke": execution_smoke,
        "execution_replay": execution_replay,
        "estimated_no_skip_seconds": estimated_no_skip_seconds,
        "budgets": validation_speed_budgets(
            runtime_acceptance,
            execution_smoke,
            execution_replay,
            estimated_no_skip_seconds,
        ),
    }


VALIDATION_PROFILE_RULES: dict[str, dict[str, object]] = {
    "docs": {
        "path_prefixes": ("docs/", "site/", "README", "CHANGELOG", "package.json"),
        "recommended_actions": (
            "check-documentation-surface",
            "check-markdown",
            "check-public-command-surface",
        ),
        "exhaustive_actions": ("validate-documentation-surface",),
        "skipped_by_default": (
            "runtime acceptance",
            "execution smoke",
            "execution replay",
        ),
    },
    "lowering": {
        "path_prefixes": (
            "native/objc3c/src/ir/",
            "native/objc3c/src/sema/",
            "native/objc3c/src/parser/",
            "tests/native/",
            "tests/tooling/fixtures/native/",
        ),
        "recommended_actions": (
            "test-behavior-matrix",
            "test-runtime-acceptance-fast",
            "test-runtime-acceptance-diagnostics",
            "test-execution-replay-focused",
        ),
        "exhaustive_actions": ("test-full", "test-nightly"),
        "skipped_by_default": ("full smoke matrix", "nightly recovery fan-out"),
    },
    "runtime": {
        "path_prefixes": (
            "native/objc3c/src/runtime/",
            "tests/tooling/runtime/",
            "native/objc3c/runtime/",
        ),
        "recommended_actions": (
            "test-runtime-acceptance-fast",
            "test-runtime-acceptance-block-arc",
            "test-runtime-acceptance-concurrency",
            "test-execution-smoke",
            "test-execution-replay-focused",
        ),
        "exhaustive_actions": ("test-runtime-acceptance", "test-nightly"),
        "skipped_by_default": ("release packaging validations",),
    },
    "diagnostics": {
        "path_prefixes": (
            "tests/tooling/fixtures/native/negative/",
            "tests/tooling/fixtures/native/diagnostics/",
            "native/objc3c/src/diagnostics/",
        ),
        "recommended_actions": (
            "test-negative-expectations",
            "test-runtime-acceptance-diagnostics",
        ),
        "exhaustive_actions": ("test-runtime-acceptance", "test-nightly"),
        "skipped_by_default": ("runtime-only smoke cases not touching diagnostics"),
    },
    "conformance": {
        "path_prefixes": (
            "tests/conformance/",
            "docs/objc3c-native/src/",
            "scripts/check_objc3c_runnable_",
        ),
        "recommended_actions": (
            "validate-conformance-corpus",
            "validate-runtime-architecture",
        ),
        "exhaustive_actions": ("test-nightly",),
        "skipped_by_default": ("stress and fuzz validation"),
    },
    "stress": {
        "path_prefixes": (
            "tests/tooling/fixtures/stress/",
            "scripts/run_objc3c_fuzz",
            "scripts/run_objc3c_lowering_runtime_stress",
            "scripts/run_objc3c_mixed_module_differential",
            "scripts/run_objc3c_stress",
        ),
        "recommended_actions": (
            "validate-stress",
            "test-fuzz-safety",
            "test-lowering-runtime-stress",
        ),
        "exhaustive_actions": ("test-nightly",),
        "skipped_by_default": ("docs-only validation"),
    },
    "release-claim": {
        "path_prefixes": (
            "docs/runbooks/",
            "schemas/",
            "release/",
            "scripts/publish_",
            "scripts/build_objc3c_release",
        ),
        "recommended_actions": (
            "check-release-evidence",
            "validate-release-foundation",
            "validate-public-conformance-reporting",
        ),
        "exhaustive_actions": ("test-nightly",),
        "skipped_by_default": ("local-only playground inspections"),
    },
}


def latest_json_file(root: Path) -> Path | None:
    if root.is_file():
        return root
    if not root.exists():
        return None
    candidates = sorted(
        (candidate for candidate in root.rglob("*.json") if candidate.is_file()),
        key=lambda candidate: candidate.stat().st_mtime,
        reverse=True,
    )
    return candidates[0] if candidates else None


def relative_path_or_none(path: Path | None) -> str | None:
    if path is None:
        return None
    try:
        return path.relative_to(ROOT).as_posix()
    except ValueError:
        return str(path)


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
                "skipped_by_default": list(rule.get("skipped_by_default", ())),
            }
        )
    if not matched_profiles and paths:
        matched_profiles.append(
            {
                "profile": "repo",
                "matched_paths": list(paths)[:20],
                "recommended_actions": ("test-smoke", "check-task-hygiene"),
                "exhaustive_actions": ("test-full", "test-nightly"),
                "skipped_by_default": ("nightly release and stress fan-out",),
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
    }


def load_latest_report_payload(path: Path | None) -> dict[str, object] | None:
    if path is None:
        return None
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        return None
    return payload if isinstance(payload, dict) else None


def dashboard_section_from_report(
    label: str, path: Path | None, payload: dict[str, object] | None
) -> dict[str, object]:
    if payload is None:
        return {
            "label": label,
            "report_path": relative_path_or_none(path),
            "status": "MISSING",
        }
    timing = payload.get("timing", {})
    timing_payload = timing if isinstance(timing, dict) else {}
    section: dict[str, object] = {
        "label": label,
        "report_path": relative_path_or_none(path),
        "status": payload.get("status", "UNKNOWN"),
        "elapsed_seconds": safe_float(timing_payload.get("elapsed_seconds", 0.0)),
    }
    if "case_count" in payload or "default_compile_backend" in payload:
        section.update(
            summarize_runtime_acceptance_report(
                relative_path_or_none(path) or "",
                payload,
                report_reused=False,
            )
        )
    elif "proof_run_id" in payload:
        section.update(
            summarize_execution_replay_report(relative_path_or_none(path) or "", payload)
        )
    elif "compile_command" in payload and "results" in payload:
        section.update(
            summarize_execution_smoke_report(relative_path_or_none(path) or "", payload)
        )
    elif "child_timing" in payload:
        section["child_timing"] = payload.get("child_timing")
        section["slowest_steps"] = timing_payload.get("slowest_steps", [])
        section["estimated_no_skip_seconds"] = timing_payload.get(
            "estimated_no_skip_seconds"
        )
    return section


def write_validation_timing_markdown(payload: dict[str, object], path: Path) -> None:
    lines = [
        "# ObjC3 Validation Timing Dashboard",
        "",
        f"- generated: `{payload['generated_at_utc']}`",
        "- contract: `objc3c.validation.speed.dashboard.v1`",
        "",
        "## Reports",
    ]
    reports = payload.get("reports", {})
    if isinstance(reports, dict):
        for name, report in reports.items():
            if not isinstance(report, dict):
                continue
            lines.append(
                f"- `{name}`: status=`{report.get('status')}` "
                f"elapsed=`{report.get('elapsed_seconds', 0.0)}` "
                f"path=`{report.get('report_path')}`"
            )
    lines.extend(["", "## Budget Status"])
    budgets = payload.get("budgets", [])
    if isinstance(budgets, list):
        for budget in budgets:
            if isinstance(budget, dict):
                lines.append(
                    f"- `{budget.get('name')}`: `{budget.get('status')}` "
                    f"actual=`{budget.get('actual_seconds', budget.get('actual_count'))}`"
                )
    lines.extend(["", "## Validation Profiles"])
    profiles = payload.get("validation_profiles", {})
    if isinstance(profiles, dict):
        for profile in profiles.get("profiles", []):
            if isinstance(profile, dict):
                actions = ", ".join(
                    str(action) for action in profile.get("recommended_actions", [])
                )
                lines.append(f"- `{profile.get('profile')}`: {actions}")
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def action_inspect_validation_timing(_: list[str]) -> int:
    runtime_path = latest_json_file(
        ROOT / "tmp" / "reports" / "runtime" / "acceptance" / "summary.json"
    )
    smoke_path = latest_json_file(
        ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-smoke"
    )
    replay_path = latest_json_file(
        ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-replay-proof"
    )
    test_full_path = latest_json_file(PUBLIC_WORKFLOW_REPORT_ROOT / "test-full.json")
    test_smoke_path = latest_json_file(PUBLIC_WORKFLOW_REPORT_ROOT / "test-smoke.json")
    report_payloads = {
        "test_full": load_latest_report_payload(test_full_path),
        "test_smoke": load_latest_report_payload(test_smoke_path),
        "runtime_acceptance": load_latest_report_payload(runtime_path),
        "execution_smoke": load_latest_report_payload(smoke_path),
        "execution_replay": load_latest_report_payload(replay_path),
    }
    reports = {
        "test_full": dashboard_section_from_report(
            "test_full", test_full_path, report_payloads["test_full"]
        ),
        "test_smoke": dashboard_section_from_report(
            "test_smoke", test_smoke_path, report_payloads["test_smoke"]
        ),
        "runtime_acceptance": dashboard_section_from_report(
            "runtime_acceptance",
            runtime_path,
            report_payloads["runtime_acceptance"],
        ),
        "execution_smoke": dashboard_section_from_report(
            "execution_smoke", smoke_path, report_payloads["execution_smoke"]
        ),
        "execution_replay": dashboard_section_from_report(
            "execution_replay", replay_path, report_payloads["execution_replay"]
        ),
    }
    runtime_report = reports["runtime_acceptance"]
    smoke_report = reports["execution_smoke"]
    replay_report = reports["execution_replay"]
    total_seconds = safe_float(
        reports["test_full"].get("estimated_no_skip_seconds")
        or reports["test_full"].get("elapsed_seconds")
        or reports["test_smoke"].get("elapsed_seconds")
    )
    payload = {
        "contract_id": "objc3c.validation.speed.dashboard.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "runner_path": WORKFLOW_RUNNER_SURFACE,
        "reports": reports,
        "budgets": validation_speed_budgets(
            runtime_report if runtime_report.get("status") != "MISSING" else None,
            smoke_report if smoke_report.get("status") != "MISSING" else None,
            replay_report if replay_report.get("status") != "MISSING" else None,
            total_seconds,
        ),
        "validation_profiles": select_validation_profiles(git_changed_paths()),
        "profile_catalog": VALIDATION_PROFILE_RULES,
        "notes": [
            "This dashboard is generated from the latest local timing reports under tmp.",
            "Budget results are warning-only until stable post-optimization baselines are established.",
            "Generated reports are observability outputs; checked-in scripts remain the source of truth.",
        ],
    }
    PUBLIC_WORKFLOW_REPORT_ROOT.mkdir(parents=True, exist_ok=True)
    dashboard_path = PUBLIC_WORKFLOW_REPORT_ROOT / "validation-timing-dashboard.json"
    markdown_path = PUBLIC_WORKFLOW_REPORT_ROOT / "validation-timing-dashboard.md"
    dashboard_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    write_validation_timing_markdown(payload, markdown_path)
    print(f"summary_path: {dashboard_path.relative_to(ROOT).as_posix()}")
    print(f"dashboard_path: {markdown_path.relative_to(ROOT).as_posix()}")
    return 0
