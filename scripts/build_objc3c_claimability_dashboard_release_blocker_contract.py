#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.reports import expected_json_report
from objc3c_tooling.reports import write_report_outputs
from objc3c_tooling.paths import display_path as repo_rel
try:
    from build_full_envelope_claimability_contracts import (
        DASHBOARD_DECISION_FIELDS,
        DASHBOARD_SUMMARY,
        NON_PRODUCTION_PUBLIC_CLAIM_CLASSES,
        PUBLIC_SUMMARY,
        PUBLIC_SUMMARY_DECISION_FIELDS,
        ROLLOUT_CLASS_CANDIDATE,
        ROLLOUT_CLASS_PREVIEW,
    )
except ModuleNotFoundError:
    from scripts.build_full_envelope_claimability_contracts import (
        DASHBOARD_DECISION_FIELDS,
        DASHBOARD_SUMMARY,
        NON_PRODUCTION_PUBLIC_CLAIM_CLASSES,
        PUBLIC_SUMMARY,
        PUBLIC_SUMMARY_DECISION_FIELDS,
        ROLLOUT_CLASS_CANDIDATE,
        ROLLOUT_CLASS_PREVIEW,
    )

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_POLICY = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "full_envelope_claimability"
    / "release_blocker_rollout_policy.json"
)
DEFAULT_JSON_OUT = (
    ROOT
    / "reports"
    / "claimability"
    / "dashboard-release-blockers"
    / "dashboard_release_blocker_contract_summary.json"
)
DEFAULT_MD_OUT = (
    ROOT
    / "reports"
    / "claimability"
    / "dashboard-release-blockers"
    / "dashboard_release_blocker_contract_summary.md"
)
RUNBOOK = ROOT / "docs" / "runbooks" / "objc3c_full_envelope_claimability.md"
RELEASE_BLOCKER_SCRIPT = (
    ROOT / "scripts" / "build_full_envelope_claimability_release_blocker_summary.py"
)
DASHBOARD_SCRIPT = ROOT / "scripts" / "build_full_envelope_claimability_dashboard.py"
SUMMARY_CONTRACT_ID = "objc3c.claimability.dashboard.release_blocker.contract.summary.v1"


class ContractError(RuntimeError):
    pass



def read_json(path: Path) -> dict[str, Any]:
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ContractError(f"missing JSON file `{repo_rel(path)}`") from exc
    except json.JSONDecodeError as exc:
        raise ContractError(f"invalid JSON at `{repo_rel(path)}`: {exc}") from exc
    if not isinstance(payload, dict):
        raise ContractError(f"JSON object expected at `{repo_rel(path)}`")
    return payload


def require_string(payload: dict[str, Any], key: str) -> str:
    value = payload.get(key)
    if not isinstance(value, str) or not value:
        raise ContractError(f"`{key}` must be a non-empty string")
    return value


def require_string_list(payload: dict[str, Any], key: str) -> list[str]:
    value = payload.get(key)
    if not isinstance(value, list) or not value:
        raise ContractError(f"`{key}` must be a non-empty list")
    out: list[str] = []
    for index, item in enumerate(value):
        if not isinstance(item, str) or not item:
            raise ContractError(f"`{key}[{index}]` must be a non-empty string")
        out.append(item)
    return out


def build_summary(policy_path: Path) -> dict[str, Any]:
    policy = read_json(policy_path)
    projection = policy.get("dashboard_release_blocker_projection")
    if not isinstance(projection, dict):
        raise ContractError("policy missing dashboard_release_blocker_projection")

    blocker = require_string(projection, "blocker")
    dashboard_summary_path = require_string(projection, "dashboard_summary_path")
    public_summary_path = require_string(projection, "public_summary_path")
    blocking_public_claim_classes = require_string_list(
        projection, "blocking_public_claim_classes"
    )
    blocking_rollout_classes = require_string_list(projection, "blocking_rollout_classes")
    required_dashboard_fields = require_string_list(projection, "required_dashboard_fields")
    required_public_summary_fields = require_string_list(
        projection, "required_public_summary_fields"
    )
    source_owned_decision_fields = require_string_list(
        projection, "source_owned_decision_fields"
    )

    release_blocker_text = RELEASE_BLOCKER_SCRIPT.read_text(encoding="utf-8")
    dashboard_text = DASHBOARD_SCRIPT.read_text(encoding="utf-8")
    runbook_text = RUNBOOK.read_text(encoding="utf-8")
    source_truth_paths = [
        repo_rel(policy_path),
        repo_rel(RELEASE_BLOCKER_SCRIPT),
        repo_rel(DASHBOARD_SCRIPT),
        repo_rel(RUNBOOK),
    ]
    tmp_source_truth_paths = [
        path for path in source_truth_paths if path.startswith("tmp/")
    ]

    checks = {
        "projection_has_blocker_id": blocker
        == "claimability-dashboard-not-production-strength",
        "projection_names_dashboard_outputs": dashboard_summary_path
        == DASHBOARD_SUMMARY
        and public_summary_path
        == PUBLIC_SUMMARY,
        "projection_blocks_non_production_claim_classes": set(
            blocking_public_claim_classes
        )
        == set(NON_PRODUCTION_PUBLIC_CLAIM_CLASSES),
        "projection_blocks_non_stable_rollouts": set(blocking_rollout_classes)
        == {ROLLOUT_CLASS_PREVIEW, ROLLOUT_CLASS_CANDIDATE},
        "projection_requires_dashboard_decision_fields": set(DASHBOARD_DECISION_FIELDS).issubset(required_dashboard_fields),
        "projection_requires_public_summary_decision_fields": set(PUBLIC_SUMMARY_DECISION_FIELDS).issubset(required_public_summary_fields),
        "projection_has_source_owned_decision_fields": set(DASHBOARD_DECISION_FIELDS).issubset(
            source_owned_decision_fields
        )
        and set(PUBLIC_SUMMARY_DECISION_FIELDS).issubset(source_owned_decision_fields),
        "release_blocker_script_emits_projection": all(
            marker in release_blocker_text
            for marker in (
                "build_dashboard_release_blocker_projection",
                "dashboard_release_blocker_projection",
                "blocks_production_strength_claim",
            )
        ),
        "dashboard_script_consumes_projection": all(
            marker in dashboard_text
            for marker in (
                "dashboard_release_blocker_projection",
                "release blocker summary missing dashboard_release_blocker_projection",
                "dashboard release-blocker projection public claim class drifted",
            )
        ),
        "runbook_documents_dashboard_blocker_projection": all(
            marker in runbook_text
            for marker in (
                "dashboard release-blocker projection",
                "`dashboard_release_blocker_projection`",
                "blocks production-strength release claims",
            )
        ),
        "source_truth_excludes_tmp": not tmp_source_truth_paths,
    }
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "source_policy_contract_id": policy.get("contract_id"),
        "source_policy_path": repo_rel(policy_path),
        "release_blocker_script": repo_rel(RELEASE_BLOCKER_SCRIPT),
        "dashboard_script": repo_rel(DASHBOARD_SCRIPT),
        "runbook": repo_rel(RUNBOOK),
        "dashboard_release_blocker_projection": {
            "blocker": blocker,
            "dashboard_summary_path": dashboard_summary_path,
            "public_summary_path": public_summary_path,
            "blocking_public_claim_classes": blocking_public_claim_classes,
            "blocking_rollout_classes": blocking_rollout_classes,
            "required_dashboard_fields": required_dashboard_fields,
            "required_public_summary_fields": required_public_summary_fields,
            "source_owned_decision_fields": source_owned_decision_fields,
        },
        "source_truth_paths": source_truth_paths,
        "tmp_source_truth_paths": tmp_source_truth_paths,
        "checks": checks,
    }


def render_markdown(summary: dict[str, Any]) -> str:
    projection = summary["dashboard_release_blocker_projection"]
    return (
        "# Claimability Dashboard Release-Blocker Contract Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Status: `{summary['status']}`\n"
        f"- Policy: `{summary['source_policy_path']}`\n"
        f"- Dashboard blocker: `{projection['blocker']}`\n"
        f"- Blocking public claim classes: `{', '.join(projection['blocking_public_claim_classes'])}`\n"
        f"- Blocking rollout classes: `{', '.join(projection['blocking_rollout_classes'])}`\n"
        f"- Source truth excludes tmp: `{summary['checks']['source_truth_excludes_tmp']}`\n"
    )


def write_outputs(summary: dict[str, Any], json_out: Path, md_out: Path) -> None:
    write_report_outputs(
        summary=summary,
        json_path=json_out,
        markdown_path=md_out,
        markdown=render_markdown(summary),
        sort_keys=False,
    )


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--policy", type=Path, default=DEFAULT_POLICY)
    parser.add_argument("--summary-json", type=Path, default=DEFAULT_JSON_OUT)
    parser.add_argument("--summary-md", type=Path, default=DEFAULT_MD_OUT)
    parser.add_argument(
        "--check",
        action="store_true",
        help="fail if checked-in reports differ from generated output",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_arg_parser().parse_args(argv)
    policy_path = args.policy if args.policy.is_absolute() else ROOT / args.policy
    json_out = args.summary_json if args.summary_json.is_absolute() else ROOT / args.summary_json
    md_out = args.summary_md if args.summary_md.is_absolute() else ROOT / args.summary_md
    try:
        summary = build_summary(policy_path)
    except ContractError as exc:
        print(f"dashboard release-blocker contract error: {exc}", file=sys.stderr)
        return 1

    next_json = expected_json_report(summary, sort_keys=False)
    next_md = render_markdown(summary)
    if args.check:
        mismatches = []
        if not json_out.is_file() or json_out.read_text(encoding="utf-8") != next_json:
            mismatches.append(repo_rel(json_out))
        if not md_out.is_file() or md_out.read_text(encoding="utf-8") != next_md:
            mismatches.append(repo_rel(md_out))
        if mismatches:
            print(
                "dashboard release-blocker contract output drift: "
                + ", ".join(mismatches),
                file=sys.stderr,
            )
            return 1
    else:
        write_outputs(summary, json_out, md_out)

    print(
        "dashboard release-blocker contract: {status} ({path})".format(
            status=summary["status"],
            path=repo_rel(json_out),
        )
    )
    return 0 if summary["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
