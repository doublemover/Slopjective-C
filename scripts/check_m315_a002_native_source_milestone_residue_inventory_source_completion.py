#!/usr/bin/env python3
"""Checker for M315-A002 native-source milestone residue inventory."""

from __future__ import annotations

import argparse
import json
import re
import sys
from collections import Counter
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SRC_ROOT = ROOT / "native" / "objc3c" / "src"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m315" / "M315-A002" / "native_source_milestone_residue_inventory_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m315_native_source_milestone_residue_inventory_source_completion_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_a002_native_source_milestone_residue_inventory_source_completion_packet.md"
INVENTORY_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_a002_native_source_milestone_residue_inventory_source_completion_inventory.json"
PATTERN = re.compile(r"m[0-9]{3}-[a-z][0-9]{3}", re.IGNORECASE)


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


def kind_for(path: Path) -> str:
    suffix = path.suffix.lower()
    if suffix == ".md":
        return "embedded_docs"
    if suffix in {".h", ".hpp"}:
        return "interface_headers"
    if suffix in {".cpp", ".cc", ".cxx"}:
        return "implementation_sources"
    return "other_native_text"


def measure() -> dict[str, object]:
    subdir_counts: Counter[str] = Counter()
    kind_counts: Counter[str] = Counter()
    hotspot_files: list[tuple[str, int, str]] = []
    code_hotspots: list[tuple[str, int]] = []
    total = 0
    file_count = 0

    for path in SRC_ROOT.rglob("*"):
        if not path.is_file():
            continue
        try:
            text = path.read_text(encoding="utf-8")
        except Exception:
            continue
        count = len(PATTERN.findall(text))
        if not count:
            continue
        rel = path.relative_to(SRC_ROOT).as_posix()
        bucket = rel.split("/", 1)[0] if "/" in rel else "."
        kind = kind_for(path)
        total += count
        file_count += 1
        subdir_counts[bucket] += count
        kind_counts[kind] += count
        hotspot_files.append((rel, count, kind))
        if kind in {"interface_headers", "implementation_sources"}:
            code_hotspots.append((rel, count))

    return {
        "total": total,
        "file_count": file_count,
        "subdir_counts": dict(subdir_counts),
        "kind_counts": dict(kind_counts),
        "top_hotspots": sorted(hotspot_files, key=lambda item: (-item[1], item[0]))[:10],
        "top_code_hotspots": sorted(code_hotspots, key=lambda item: (-item[1], item[0]))[:10],
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
    checks_passed += require("Contract ID: `objc3c-cleanup-native-source-milestone-residue-inventory/m315-a002-v1`" in expectations, str(EXPECTATIONS_DOC), "M315-A002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("embedded docs inside" in expectations, str(EXPECTATIONS_DOC), "M315-A002-EXP-02", "expectations missing embedded-doc note", failures)
    checks_passed += require("native-source slice" in packet, str(PACKET_DOC), "M315-A002-PKT-01", "packet missing native-source summary", failures)
    checks_passed += require("Next issue: `M315-A003`." in packet, str(PACKET_DOC), "M315-A002-PKT-02", "packet missing next issue", failures)

    checks_total += 5
    checks_passed += require(inventory.get("mode") == "m315-a002-native-source-milestone-residue-inventory-v1", str(INVENTORY_JSON), "M315-A002-INV-01", "mode drifted", failures)
    checks_passed += require(inventory.get("contract_id") == "objc3c-cleanup-native-source-milestone-residue-inventory/m315-a002-v1", str(INVENTORY_JSON), "M315-A002-INV-02", "contract id drifted", failures)
    checks_passed += require(inventory.get("totals", {}).get("match_count") == measured["total"] == 3182, str(INVENTORY_JSON), "M315-A002-INV-03", "native match count drifted", failures)
    checks_passed += require(inventory.get("totals", {}).get("file_count_with_matches") == measured["file_count"] == 32, str(INVENTORY_JSON), "M315-A002-INV-04", "native file count drifted", failures)
    checks_passed += require(inventory.get("next_issue") == "M315-A003", str(INVENTORY_JSON), "M315-A002-INV-05", "next issue drifted", failures)

    for key, expected in {
        "embedded_docs": 1744,
        "interface_headers": 542,
        "implementation_sources": 896,
    }.items():
        checks_total += 1
        checks_passed += require(inventory.get("kind_counts", {}).get(key) == measured["kind_counts"].get(key) == expected, str(INVENTORY_JSON), f"M315-A002-KIND-{key}", f"kind count drifted for {key}", failures)

    for key, expected in {
        ".": 1691,
        "lower": 383,
        "sema": 214,
        "runtime": 189,
        "ir": 153,
        "pipeline": 136,
        "io": 105,
    }.items():
        checks_total += 1
        checks_passed += require(inventory.get("subdirectory_counts", {}).get(key) == measured["subdir_counts"].get(key) == expected, str(INVENTORY_JSON), f"M315-A002-SUBDIR-{key}", f"subdir count drifted for {key}", failures)

    checks_total += 5
    checks_passed += require(inventory.get("top_hotspot_files", [])[0]["path"] == "ARCHITECTURE.md" and inventory.get("top_hotspot_files", [])[0]["match_count"] == 1691, str(INVENTORY_JSON), "M315-A002-HOT-01", "top native hotspot drifted", failures)
    checks_passed += require(inventory.get("top_hotspot_files", [])[1]["path"] == "lower/objc3_lowering_contract.h", str(INVENTORY_JSON), "M315-A002-HOT-02", "second native hotspot drifted", failures)
    checks_passed += require(inventory.get("top_code_hotspot_files", [])[0]["path"] == "lower/objc3_lowering_contract.h" and inventory.get("top_code_hotspot_files", [])[0]["match_count"] == 234, str(INVENTORY_JSON), "M315-A002-HOT-03", "top code hotspot drifted", failures)
    checks_passed += require(inventory.get("downstream_ownership", {}).get("native_source_marker_removal") == "M315-B003", str(INVENTORY_JSON), "M315-A002-HOT-04", "downstream owner drifted", failures)
    checks_passed += require(inventory.get("downstream_ownership", {}).get("native_source_decontamination_sweep") == "M315-B005", str(INVENTORY_JSON), "M315-A002-HOT-05", "downstream sweep owner drifted", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": inventory["mode"],
        "contract_id": inventory["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "native_match_count": measured["total"],
        "kind_counts": measured["kind_counts"],
        "subdirectory_counts": measured["subdir_counts"],
        "top_hotspots": measured["top_hotspots"],
        "top_code_hotspots": measured["top_code_hotspots"],
        "next_issue": "M315-A003",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M315-A002 native-source residue inventory checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
