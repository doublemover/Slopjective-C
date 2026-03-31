#!/usr/bin/env python3
from __future__ import annotations

import json
import re
import subprocess
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/source_hygiene/archive_boundary_contract.json"
PRODUCT_REPORT_PATH = ROOT / "tmp/reports/source-hygiene/product-decontamination/product_decontamination_report.json"
OUT_DIR = ROOT / "tmp/reports/source-hygiene/archive-boundary-compatibility"
JSON_OUT = OUT_DIR / "archive_boundary_compatibility_summary.json"
MD_OUT = OUT_DIR / "archive_boundary_compatibility_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def normalize(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def tracked_files_under(prefix: str) -> list[str]:
    result = subprocess.run(
        ["git", "ls-files", prefix],
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )
    return [line.strip() for line in result.stdout.splitlines() if line.strip()]


def count_pattern_hits(paths: list[str], patterns: list[str]) -> tuple[int, list[str]]:
    compiled = [re.compile(pattern) for pattern in patterns]
    sample: list[str] = []
    count = 0
    for relative_path in paths:
        text = (ROOT / relative_path).read_text(encoding="utf-8", errors="ignore")
        if any(pattern.search(text) for pattern in compiled):
            count += 1
            if len(sample) < 10:
                sample.append(relative_path)
    return count, sample


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    product_report = read_json(PRODUCT_REPORT_PATH)

    archive_counts: dict[str, int] = {}
    archive_samples: dict[str, list[str]] = {}
    archive_total = 0
    pattern_total = 0
    pattern_sample: list[str] = []
    for root in contract["archive_roots"]:
        files = tracked_files_under(root)
        archive_counts[root] = len(files)
        archive_samples[root] = files[:10]
        archive_total += len(files)
        hits, sample = count_pattern_hits(files, contract["historical_reference_patterns"])
        pattern_total += hits
        for path in sample:
            if len(pattern_sample) < 10 and path not in pattern_sample:
                pattern_sample.append(path)

    product_roots = contract["product_surface_roots"]
    archive_roots = contract["archive_roots"]
    checks = {
        "archive_roots_have_tracked_files": archive_total > 0,
        "archive_roots_do_not_overlap_product_roots": all(
            not any(product.startswith(archive) or archive.startswith(product) for product in product_roots)
            for archive in archive_roots
        ),
        "generated_truth_files_stay_outside_archive_roots": all(
            not any(path.startswith(root) for root in archive_roots)
            for path in contract["generated_truth_files"]
        ),
        "archive_roots_preserve_historical_references": pattern_total > 0,
        "product_surface_report_stays_clean": product_report.get("ok") is True,
        "product_surface_report_has_zero_residue_hits": product_report.get("product_milestone_residue_hit_count") == 0
        and product_report.get("generated_truth_milestone_residue_hit_count") == 0,
    }

    summary = {
        "issue": "source-hygiene-archive-boundary-compatibility",
        "contract_id": contract["contract_id"],
        "archive_root_file_counts": archive_counts,
        "archive_root_samples": archive_samples,
        "historical_reference_hit_count": pattern_total,
        "historical_reference_sample": pattern_sample,
        "product_report_path": normalize(PRODUCT_REPORT_PATH),
        "checks": checks,
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Archive Boundary Compatibility Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Archive-tracked file count: `{archive_total}`\n"
        f"- Historical-reference hit count: `{summary['historical_reference_hit_count']}`\n"
        f"- Product cleanliness report: `{summary['product_report_path']}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
