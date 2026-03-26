#!/usr/bin/env python3
"""Checker for M315-B001 stable feature-surface identifier policy."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m315" / "M315-B001" / "stable_identifier_policy_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m315_stable_feature_surface_identifier_and_annotation_policy_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_b001_stable_feature_surface_identifier_and_annotation_policy_contract_and_architecture_freeze_packet.md"
POLICY_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_b001_stable_feature_surface_identifier_and_annotation_policy_contract_and_architecture_freeze_policy.json"


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

    regex_text = policy["durable_identifier_grammar"]["regex"]
    regex = re.compile(regex_text)
    prohibited_patterns = [re.compile(pattern) for pattern in policy["prohibited_segment_patterns"]]

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-stable-feature-surface-identifier-policy/m315-b001-v1`" in expectations, str(EXPECTATIONS_DOC), "M315-B001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("`objc3c.<domain>.<subsystem>.<surface>.vN`" in expectations, str(EXPECTATIONS_DOC), "M315-B001-EXP-02", "expectations missing stable identifier grammar", failures)
    checks_passed += require("Durable product and generated source-of-truth identifiers must use" in packet, str(PACKET_DOC), "M315-B001-PKT-01", "packet missing durable identifier summary", failures)
    checks_passed += require("Next issue: `M315-B002`." in packet, str(PACKET_DOC), "M315-B001-PKT-02", "packet missing next issue", failures)

    checks_total += 6
    checks_passed += require(policy.get("mode") == "m315-b001-stable-feature-surface-identifier-policy-v1", str(POLICY_JSON), "M315-B001-POL-01", "mode drifted", failures)
    checks_passed += require(policy.get("contract_id") == "objc3c-cleanup-stable-feature-surface-identifier-policy/m315-b001-v1", str(POLICY_JSON), "M315-B001-POL-02", "contract id drifted", failures)
    checks_passed += require(policy.get("durable_identifier_grammar", {}).get("required_prefix") == "objc3c.", str(POLICY_JSON), "M315-B001-POL-03", "required prefix drifted", failures)
    checks_passed += require(policy.get("durable_identifier_grammar", {}).get("terminal_version_segment_required") is True, str(POLICY_JSON), "M315-B001-POL-04", "terminal version requirement drifted", failures)
    checks_passed += require(policy.get("durable_identifier_grammar", {}).get("lowercase_ascii_only") is True, str(POLICY_JSON), "M315-B001-POL-05", "lowercase-ascii requirement drifted", failures)
    checks_passed += require(policy.get("next_issue") == "M315-B002", str(POLICY_JSON), "M315-B001-POL-06", "next issue drifted", failures)

    checks_total += 5
    checks_passed += require(len(policy.get("annotation_families", [])) == 5, str(POLICY_JSON), "M315-B001-ANN-01", "annotation family count drifted", failures)
    checks_passed += require(policy["annotation_families"][0]["family_id"] == "surface_id", str(POLICY_JSON), "M315-B001-ANN-02", "surface_id family missing or reordered", failures)
    checks_passed += require(policy["annotation_families"][4]["family_id"] == "historical_ref", str(POLICY_JSON), "M315-B001-ANN-03", "historical_ref family missing or reordered", failures)
    checks_passed += require("native/objc3c/src/** durable identifiers" in policy["annotation_families"][4]["prohibited_in"], str(POLICY_JSON), "M315-B001-ANN-04", "historical_ref prohibition drifted", failures)
    checks_passed += require(policy.get("downstream_ownership", {}).get("product_code_annotation_policy") == "M315-B002", str(POLICY_JSON), "M315-B001-ANN-05", "product-code annotation owner drifted", failures)

    example_map = policy["replacement_examples"]
    checks_total += len(example_map)
    for key, value in example_map.items():
        checks_passed += require(bool(regex.fullmatch(value)), str(POLICY_JSON), f"M315-B001-EXAMPLE-{key}", f"replacement example does not match stable grammar: {value}", failures)

    prohibited_tokens = ["m315", "m228", "lanea", "laned", "issue7796", "a001", "d002"]
    checks_total += len(example_map)
    for key, value in example_map.items():
        parts = value.split(".")
        ok = all(not pattern.fullmatch(part) for part in parts for pattern in prohibited_patterns) and not any(token in value for token in prohibited_tokens)
        checks_passed += require(ok, str(POLICY_JSON), f"M315-B001-PROHIBITED-{key}", f"replacement example includes prohibited milestone token: {value}", failures)

    checks_total += 3
    checks_passed += require(any(path == "docs/contracts/**" for path in policy.get("archival_only_zones", [])), str(POLICY_JSON), "M315-B001-ZONE-01", "docs/contracts archival zone missing", failures)
    checks_passed += require(any(path == "spec/planning/**" for path in policy.get("archival_only_zones", [])), str(POLICY_JSON), "M315-B001-ZONE-02", "spec/planning archival zone missing", failures)
    checks_passed += require(any(path == "native/objc3c/src/**" for path in policy.get("durable_product_zones", [])), str(POLICY_JSON), "M315-B001-ZONE-03", "native source durable zone missing", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": policy["mode"],
        "contract_id": policy["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "durable_identifier_regex": regex_text,
        "annotation_families": [item["family_id"] for item in policy["annotation_families"]],
        "replacement_examples": example_map,
        "next_issue": "M315-B002",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M315-B001 stable identifier policy checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
