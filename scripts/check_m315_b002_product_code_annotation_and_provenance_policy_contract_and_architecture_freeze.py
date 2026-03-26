#!/usr/bin/env python3
"""Checker for M315-B002 product-code annotation/provenance policy."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m315" / "M315-B002" / "product_code_annotation_provenance_policy_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m315_product_code_annotation_and_provenance_policy_contract_and_architecture_freeze_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_b002_product_code_annotation_and_provenance_policy_contract_and_architecture_freeze_packet.md"
POLICY_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_b002_product_code_annotation_and_provenance_policy_contract_and_architecture_freeze_policy.json"


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    policy = json.loads(read_text(POLICY_JSON))

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-product-code-annotation-provenance-policy/m315-b002-v1`" in expectations, str(EXPECTATIONS_DOC), "M315-B002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("prohibit milestone, lane, and issue tokens" in expectations, str(EXPECTATIONS_DOC), "M315-B002-EXP-02", "expectations missing prohibition summary", failures)
    checks_passed += require("Allowed product/generated annotation families" in packet, str(PACKET_DOC), "M315-B002-PKT-01", "packet missing annotation family summary", failures)
    checks_passed += require("Next issue: `M315-B003`." in packet, str(PACKET_DOC), "M315-B002-PKT-02", "packet missing next issue", failures)

    checks_total += 5
    checks_passed += require(policy.get("mode") == "m315-b002-product-code-annotation-provenance-policy-v1", str(POLICY_JSON), "M315-B002-POL-01", "mode drifted", failures)
    checks_passed += require(policy.get("contract_id") == "objc3c-cleanup-product-code-annotation-provenance-policy/m315-b002-v1", str(POLICY_JSON), "M315-B002-POL-02", "contract id drifted", failures)
    checks_passed += require(policy.get("next_issue") == "M315-B003", str(POLICY_JSON), "M315-B002-POL-03", "next issue drifted", failures)
    checks_passed += require(policy.get("generated_truth_requirements", {}).get("generated_replay_requires_followup_owner_issue") == "M315-C003", str(POLICY_JSON), "M315-B002-POL-04", "generated replay owner drifted", failures)
    checks_passed += require(policy.get("generated_truth_requirements", {}).get("authenticity_schema_owner_issue") == "M315-C002", str(POLICY_JSON), "M315-B002-POL-05", "authenticity schema owner drifted", failures)

    checks_total += 7
    checks_passed += require([entry["family_id"] for entry in policy.get("allowed_annotation_families", [])] == ["surface_id", "artifact_family_id", "provenance_class", "provenance_mode"], str(POLICY_JSON), "M315-B002-ANN-01", "allowed annotation families drifted", failures)
    checks_passed += require(policy.get("archival_only_family", {}).get("family_id") == "historical_ref", str(POLICY_JSON), "M315-B002-ANN-02", "archival-only family drifted", failures)
    checks_passed += require("native/objc3c/src/** durable identifiers" in policy.get("archival_only_family", {}).get("prohibited_locations", []), str(POLICY_JSON), "M315-B002-ANN-03", "historical_ref prohibition missing", failures)
    checks_passed += require(policy.get("native_source_comment_policy", {}).get("allow_planning_narrative") is False, str(POLICY_JSON), "M315-B002-ANN-04", "planning narrative rule drifted", failures)
    checks_passed += require(policy.get("native_source_comment_policy", {}).get("allow_milestone_strings") is False, str(POLICY_JSON), "M315-B002-ANN-05", "milestone string rule drifted", failures)
    checks_passed += require(policy.get("native_source_comment_policy", {}).get("allow_issue_ids") is False, str(POLICY_JSON), "M315-B002-ANN-06", "issue id rule drifted", failures)
    checks_passed += require(policy.get("native_source_comment_policy", {}).get("allow_brief_surface_marker_comments") is True, str(POLICY_JSON), "M315-B002-ANN-07", "brief surface marker rule drifted", failures)

    expected_classes = [
        "synthetic_fixture",
        "sample_or_example",
        "generated_report",
        "generated_replay",
        "schema_policy_contract",
        "historical_archive",
    ]
    checks_total += 3
    checks_passed += require(policy.get("provenance_classes") == expected_classes, str(POLICY_JSON), "M315-B002-PROV-01", "provenance class list drifted", failures)
    checks_passed += require("mNNN" in policy.get("prohibited_annotation_tokens", []), str(POLICY_JSON), "M315-B002-PROV-02", "mNNN prohibition missing", failures)
    checks_passed += require("issueNNNN" in policy.get("prohibited_annotation_tokens", []), str(POLICY_JSON), "M315-B002-PROV-03", "issueNNNN prohibition missing", failures)

    checks_total += 3
    checks_passed += require(policy.get("downstream_ownership", {}).get("native_source_marker_removal") == "M315-B003", str(POLICY_JSON), "M315-B002-OWN-01", "native source removal owner drifted", failures)
    checks_passed += require(policy.get("downstream_ownership", {}).get("authenticity_schema") == "M315-C002", str(POLICY_JSON), "M315-B002-OWN-02", "authenticity schema owner drifted", failures)
    checks_passed += require(policy.get("downstream_ownership", {}).get("replay_regeneration") == "M315-C003", str(POLICY_JSON), "M315-B002-OWN-03", "replay regeneration owner drifted", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": policy["mode"],
        "contract_id": policy["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "allowed_annotation_families": [entry["family_id"] for entry in policy["allowed_annotation_families"]],
        "provenance_classes": policy["provenance_classes"],
        "downstream_ownership": policy["downstream_ownership"],
        "next_issue": "M315-B003",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M315-B002 product-code annotation/provenance policy checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
