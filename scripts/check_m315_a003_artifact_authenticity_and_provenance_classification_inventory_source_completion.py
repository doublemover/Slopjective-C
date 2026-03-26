#!/usr/bin/env python3
"""Checker for M315-A003 artifact authenticity inventory."""

from __future__ import annotations

import argparse
import json
import sys
from collections import Counter, defaultdict
from dataclasses import dataclass
from pathlib import Path
from subprocess import check_output
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m315" / "M315-A003" / "artifact_authenticity_inventory_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m315_artifact_authenticity_and_provenance_classification_inventory_source_completion_a003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_a003_artifact_authenticity_and_provenance_classification_inventory_source_completion_packet.md"
INVENTORY_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_a003_artifact_authenticity_and_provenance_classification_inventory_source_completion_inventory.json"
EXCLUDED_JSON = {".prettierrc.json", "package-lock.json", "package.json"}


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


def tracked_artifacts() -> list[str]:
    files = check_output(["git", "ls-files"], cwd=ROOT, text=True).splitlines()
    candidates: list[str] = []
    for rel in files:
        rel = rel.replace("\\", "/")
        suffix = Path(rel).suffix.lower()
        if suffix == ".ll":
            candidates.append(rel)
        elif suffix == ".json" and rel not in EXCLUDED_JSON:
            candidates.append(rel)
    return candidates


def classify(rel: str) -> str:
    if rel.startswith(("schemas/", "registries/", "spec/planning/")) or rel == "site/src/index.contract.json":
        return "schema_policy_contract"
    if rel.startswith("reports/"):
        lower = rel.lower()
        if ".sample." in lower or ".example." in lower:
            return "sample_or_example_artifact"
        return "generated_or_report_artifact"
    if rel.startswith("tests/conformance/examples/"):
        return "sample_or_example_artifact"
    if rel.startswith("tests/conformance/"):
        return "synthetic_fixture"
    if rel.startswith("tests/tooling/fixtures/objc3c/") and "/replay_run_" in rel and (rel.endswith(".ll") or rel.endswith("module.manifest.json")):
        return "replay_candidate_missing_provenance"
    if rel.startswith("tests/tooling/fixtures/native/library_cli_parity/"):
        return "synthetic_fixture"
    if rel.startswith("tests/tooling/fixtures/"):
        return "synthetic_fixture"
    return "unclassified"


def provenance_signal(rel: str) -> str:
    text = (ROOT / rel).read_text(encoding="utf-8")
    if rel.endswith(".ll"):
        header = "\n".join(text.splitlines()[:5]).lower()
        if "fixture parity ir" in header:
            return "explicit_fixture_label"
        if "objc3c native frontend ir" in header:
            return "artifact_header_only"
        return "none"
    try:
        payload = json.loads(text)
    except json.JSONDecodeError:
        return "non_json_or_invalid"
    if isinstance(payload, dict) and isinstance(payload.get("artifact_authenticity"), dict):
        return "artifact_authenticity_envelope"
    if any(key in payload for key in ("generated_at", "published_at_utc", "generated_at_utc")):
        return "embedded_generation_metadata"
    if any(key in payload for key in ("producer", "source_revision", "ci_run_id", "release_id", "artifact_id")):
        return "embedded_generation_metadata"
    if rel.endswith((".meta.json", ".status.json", ".snapshot.json", ".diagnostics.json")):
        return "fixture_or_status_label"
    if rel.endswith((".schema.json", ".contract.json")):
        return "schema_or_contract_label"
    if rel.endswith("module.manifest.json"):
        return "artifact_header_only"
    return "none"


def measure() -> dict[str, object]:
    artifacts = tracked_artifacts()
    class_counts: Counter[str] = Counter()
    provenance_counts: Counter[str] = Counter()
    root_counts: Counter[str] = Counter()
    synthetic_breakdown: Counter[str] = Counter()
    sample_breakdown: Counter[str] = Counter()
    generated_breakdown: Counter[str] = Counter()
    replay_breakdown: Counter[str] = Counter()
    class_by_provenance: dict[str, Counter[str]] = defaultdict(Counter)
    flagged = {
        "explicit_synthetic_stub_ir": [],
        "replay_without_frontend_header": [],
        "generated_reports_without_embedded_metadata": [],
        "sample_artifacts_without_embedded_metadata": [],
    }

    for rel in artifacts:
        artifact_class = classify(rel)
        provenance = provenance_signal(rel)
        class_counts[artifact_class] += 1
        provenance_counts[provenance] += 1
        class_by_provenance[artifact_class][provenance] += 1

        if rel.startswith("tests/conformance/examples/"):
            root_counts["tests/conformance/examples"] += 1
        elif rel.startswith("tests/conformance/"):
            root_counts["tests/conformance"] += 1
        elif rel.startswith("tests/tooling/fixtures/"):
            root_counts["tests/tooling/fixtures"] += 1
        elif rel.startswith("spec/planning/"):
            root_counts["spec/planning"] += 1
        elif rel.startswith("reports/"):
            root_counts["reports"] += 1
        elif rel.startswith("schemas/"):
            root_counts["schemas"] += 1
        elif rel.startswith("registries/"):
            root_counts["registries"] += 1
        elif rel.startswith("site/src/"):
            root_counts["site/src"] += 1

        if artifact_class == "synthetic_fixture":
            if rel.startswith("tests/conformance/"):
                synthetic_breakdown["tests/conformance"] += 1
            elif rel.startswith("tests/tooling/fixtures/"):
                synthetic_breakdown["tests/tooling/fixtures"] += 1
        elif artifact_class == "sample_or_example_artifact":
            if rel.startswith("reports/"):
                sample_breakdown["reports"] += 1
            elif rel.startswith("tests/conformance/examples/"):
                sample_breakdown["tests/conformance/examples"] += 1
        elif artifact_class == "generated_or_report_artifact":
            generated_breakdown["reports"] += 1
        elif artifact_class == "replay_candidate_missing_provenance":
            replay_breakdown[Path(rel).suffix.lower()] += 1
            if rel.endswith(".ll"):
                if provenance == "artifact_header_only":
                    replay_breakdown["ll_with_frontend_header"] += 1
                else:
                    replay_breakdown["ll_without_frontend_header"] += 1

        if provenance == "explicit_fixture_label":
            flagged["explicit_synthetic_stub_ir"].append(rel)
        if artifact_class == "replay_candidate_missing_provenance" and provenance == "none":
            flagged["replay_without_frontend_header"].append(rel)
        if artifact_class == "generated_or_report_artifact" and provenance == "none":
            flagged["generated_reports_without_embedded_metadata"].append(rel)
        if artifact_class == "sample_or_example_artifact" and provenance == "none":
            flagged["sample_artifacts_without_embedded_metadata"].append(rel)

    return {
        "tracked_artifact_candidates": len(artifacts),
        "ll_files": sum(1 for rel in artifacts if rel.endswith(".ll")),
        "json_files": sum(1 for rel in artifacts if rel.endswith(".json")),
        "class_counts": dict(class_counts),
        "provenance_counts": dict(provenance_counts),
        "root_counts": dict(root_counts),
        "synthetic_breakdown": dict(synthetic_breakdown),
        "sample_breakdown": dict(sample_breakdown),
        "generated_breakdown": dict(generated_breakdown),
        "replay_breakdown": {
            "ll_files": replay_breakdown[".ll"],
            "manifest_json_files": replay_breakdown[".json"],
            "ll_with_frontend_header": replay_breakdown["ll_with_frontend_header"],
            "ll_without_frontend_header": replay_breakdown["ll_without_frontend_header"],
        },
        "class_by_provenance": {key: dict(value) for key, value in class_by_provenance.items()},
        "flagged_examples": {key: value[:10] for key, value in flagged.items()},
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    inventory = json.loads(read_text(INVENTORY_JSON))
    measured = measure()

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-artifact-authenticity-inventory/m315-a003-v1`" in expectations, str(EXPECTATIONS_DOC), "M315-A003-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("`.ll` plus authenticity-sensitive tracked `.json` surfaces" in expectations, str(EXPECTATIONS_DOC), "M315-A003-EXP-02", "expectations missing scope note", failures)
    checks_passed += require("artifact-authenticity slice" in packet, str(PACKET_DOC), "M315-A003-PKT-01", "packet missing artifact-authenticity summary", failures)
    checks_passed += require("Next issue: `M315-B001`." in packet, str(PACKET_DOC), "M315-A003-PKT-02", "packet missing next issue", failures)

    checks_total += 5
    checks_passed += require(inventory.get("mode") == "m315-a003-artifact-authenticity-inventory-v1", str(INVENTORY_JSON), "M315-A003-INV-01", "mode drifted", failures)
    checks_passed += require(inventory.get("contract_id") == "objc3c-cleanup-artifact-authenticity-inventory/m315-a003-v1", str(INVENTORY_JSON), "M315-A003-INV-02", "contract id drifted", failures)
    checks_passed += require(inventory.get("totals", {}).get("tracked_artifact_candidates") == measured["tracked_artifact_candidates"] == 2514, str(INVENTORY_JSON), "M315-A003-INV-03", "artifact candidate total drifted", failures)
    checks_passed += require(inventory.get("totals", {}).get("ll_files") == measured["ll_files"] == 78, str(INVENTORY_JSON), "M315-A003-INV-04", "ll file count drifted", failures)
    checks_passed += require(inventory.get("totals", {}).get("json_files") == measured["json_files"] == 2436, str(INVENTORY_JSON), "M315-A003-INV-05", "json file count drifted", failures)

    for key, expected in {
        "schema_policy_contract": 66,
        "generated_or_report_artifact": 25,
        "sample_or_example_artifact": 62,
        "synthetic_fixture": 2209,
        "replay_candidate_missing_provenance": 152,
    }.items():
        checks_total += 1
        checks_passed += require(inventory.get("authenticity_class_counts", {}).get(key) == measured["class_counts"].get(key) == expected, str(INVENTORY_JSON), f"M315-A003-CLASS-{key}", f"class count drifted for {key}", failures)

    for key, expected in {
        "none": 2162,
        "artifact_authenticity_envelope": 3,
        "embedded_generation_metadata": 49,
        "artifact_header_only": 106,
        "fixture_or_status_label": 183,
        "explicit_fixture_label": 2,
    }.items():
        checks_total += 1
        checks_passed += require(inventory.get("provenance_signal_counts", {}).get(key) == measured["provenance_counts"].get(key) == expected, str(INVENTORY_JSON), f"M315-A003-PROV-{key}", f"provenance count drifted for {key}", failures)

    checks_total += 9
    checks_passed += require(inventory.get("root_counts", {}).get("tests/tooling/fixtures") == measured["root_counts"].get("tests/tooling/fixtures") == 760, str(INVENTORY_JSON), "M315-A003-ROOT-01", "tests/tooling/fixtures root count drifted", failures)
    checks_passed += require(inventory.get("root_counts", {}).get("tests/conformance") == measured["root_counts"].get("tests/conformance") == 1601, str(INVENTORY_JSON), "M315-A003-ROOT-02", "tests/conformance root count drifted", failures)
    checks_passed += require(inventory.get("root_counts", {}).get("spec/planning") == measured["root_counts"].get("spec/planning") == 60, str(INVENTORY_JSON), "M315-A003-ROOT-03", "spec/planning root count drifted", failures)
    checks_passed += require(inventory.get("root_counts", {}).get("tests/conformance/examples") == measured["root_counts"].get("tests/conformance/examples") == 59, str(INVENTORY_JSON), "M315-A003-ROOT-04", "tests/conformance/examples root count drifted", failures)
    checks_passed += require(inventory.get("synthetic_fixture_breakdown", {}).get("tests/conformance") == measured["synthetic_breakdown"].get("tests/conformance") == 1601, str(INVENTORY_JSON), "M315-A003-SYN-01", "synthetic tests/conformance count drifted", failures)
    checks_passed += require(inventory.get("synthetic_fixture_breakdown", {}).get("tests/tooling/fixtures") == measured["synthetic_breakdown"].get("tests/tooling/fixtures") == 608, str(INVENTORY_JSON), "M315-A003-SYN-02", "synthetic tests/tooling/fixtures count drifted", failures)
    checks_passed += require(inventory.get("sample_or_example_breakdown", {}).get("reports") == measured["sample_breakdown"].get("reports") == 3, str(INVENTORY_JSON), "M315-A003-SAMPLE-01", "sample reports count drifted", failures)
    checks_passed += require(inventory.get("sample_or_example_breakdown", {}).get("tests/conformance/examples") == measured["sample_breakdown"].get("tests/conformance/examples") == 59, str(INVENTORY_JSON), "M315-A003-SAMPLE-02", "sample tests/conformance/examples count drifted", failures)
    checks_passed += require(inventory.get("generated_report_breakdown", {}).get("reports") == measured["generated_breakdown"].get("reports") == 25, str(INVENTORY_JSON), "M315-A003-GEN-01", "generated reports count drifted", failures)

    checks_total += 8
    replay = inventory.get("replay_candidate_breakdown", {})
    checks_passed += require(replay.get("ll_files") == measured["replay_breakdown"]["ll_files"] == 76, str(INVENTORY_JSON), "M315-A003-REPLAY-01", "replay ll count drifted", failures)
    checks_passed += require(replay.get("manifest_json_files") == measured["replay_breakdown"]["manifest_json_files"] == 76, str(INVENTORY_JSON), "M315-A003-REPLAY-02", "replay manifest count drifted", failures)
    checks_passed += require(replay.get("ll_with_frontend_header") == measured["replay_breakdown"]["ll_with_frontend_header"] == 30, str(INVENTORY_JSON), "M315-A003-REPLAY-03", "replay ll-with-header count drifted", failures)
    checks_passed += require(replay.get("ll_without_frontend_header") == measured["replay_breakdown"]["ll_without_frontend_header"] == 46, str(INVENTORY_JSON), "M315-A003-REPLAY-04", "replay ll-without-header count drifted", failures)
    checks_passed += require(inventory.get("flagged_examples", {}).get("explicit_synthetic_stub_ir", [None])[0] == "tests/tooling/fixtures/native/library_cli_parity/cli/module.ll", str(INVENTORY_JSON), "M315-A003-FLAG-01", "explicit synthetic stub example drifted", failures)
    checks_passed += require(len(inventory.get("flagged_examples", {}).get("explicit_synthetic_stub_ir", [])) == 2, str(INVENTORY_JSON), "M315-A003-FLAG-02", "explicit synthetic stub example count drifted", failures)
    checks_passed += require(inventory.get("flagged_examples", {}).get("replay_without_frontend_header", [None])[0] == "tests/tooling/fixtures/objc3c/m170_validation_block_determinism_perf_baseline_contract/replay_run_1/module.ll", str(INVENTORY_JSON), "M315-A003-FLAG-03", "replay missing-header example drifted", failures)
    checks_passed += require(inventory.get("next_issue") == "M315-B001", str(INVENTORY_JSON), "M315-A003-INV-06", "next issue drifted", failures)

    checks_total += 3
    checks_passed += require(inventory.get("downstream_ownership", {}).get("stable_identifier_and_annotation_policy") == "M315-B001", str(INVENTORY_JSON), "M315-A003-OWN-01", "stable identifier owner drifted", failures)
    checks_passed += require(inventory.get("downstream_ownership", {}).get("replay_regeneration_and_provenance_capture") == "M315-C003", str(INVENTORY_JSON), "M315-A003-OWN-02", "replay regeneration owner drifted", failures)
    checks_passed += require(inventory.get("downstream_ownership", {}).get("synthetic_fixture_relocation_and_labeling") == "M315-C004", str(INVENTORY_JSON), "M315-A003-OWN-03", "synthetic fixture relocation owner drifted", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": inventory["mode"],
        "contract_id": inventory["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "tracked_artifact_candidates": measured["tracked_artifact_candidates"],
        "authenticity_class_counts": measured["class_counts"],
        "provenance_signal_counts": measured["provenance_counts"],
        "replay_candidate_breakdown": measured["replay_breakdown"],
        "flagged_examples": measured["flagged_examples"],
        "next_issue": "M315-B001",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M315-A003 artifact authenticity inventory checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
