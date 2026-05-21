"""Summary payloads and output rendering for the conformance corpus surface checker."""

from __future__ import annotations

import json
import sys
from dataclasses import dataclass
from datetime import datetime, timezone
from typing import Any, Mapping

from objc3c_tooling.paths import repo_rel

from .contracts import (
    CHECKER_NAME,
    EXPECTED_PRIMARY_BUCKETS,
    EXPECTED_SUPPLEMENTAL_BUCKETS,
    SUMMARY_CONTRACT_ID,
)
from .loading import ConformanceCorpusPaths


@dataclass(frozen=True)
class ManifestBucketSummary:
    bucket: str
    manifest_path: str
    suite: Any
    group_count: int
    fixture_count: int
    issue_refs: tuple[int, ...]

    def to_payload(self) -> dict[str, Any]:
        return {
            "bucket": self.bucket,
            "manifest_path": self.manifest_path,
            "suite": self.suite,
            "group_count": self.group_count,
            "fixture_count": self.fixture_count,
            "issue_count": len(self.issue_refs),
            "issue_refs": list(self.issue_refs),
        }


@dataclass(frozen=True)
class RetainedSuiteSummary:
    suite_id: str
    suite_class: str
    bucket: str
    manifest: str
    traceability_targets: tuple[Any, ...]

    def to_payload(self) -> dict[str, Any]:
        return {
            "suite_id": self.suite_id,
            "suite_class": self.suite_class,
            "bucket": self.bucket,
            "manifest": self.manifest,
            "traceability_targets": list(self.traceability_targets),
        }


@dataclass(frozen=True)
class ConformanceCorpusSurfaceSummary:
    paths: ConformanceCorpusPaths
    surface: Mapping[str, Any]
    bucket_summaries: tuple[ManifestBucketSummary, ...]
    retained_suite_summary: tuple[RetainedSuiteSummary, ...]
    workflow_surface: Mapping[str, Any]

    def to_payload(self) -> dict[str, Any]:
        return {
            "contract_id": SUMMARY_CONTRACT_ID,
            "generated_at_utc": datetime.now(timezone.utc).isoformat(),
            "status": "PASS",
            "corpus_surface_contract": repo_rel(self.paths.corpus_surface_path),
            "coverage_map": self.surface["coverage_map"],
            "runbook": self.surface["runbook"],
            "support_claim_runnable_evidence_catalog": self.surface[
                "support_claim_runnable_evidence_catalog"
            ],
            "public_suite_manifest": self.surface["public_suite_manifest"],
            "primary_buckets": EXPECTED_PRIMARY_BUCKETS,
            "supplemental_buckets": EXPECTED_SUPPLEMENTAL_BUCKETS,
            "bucket_summaries": [summary.to_payload() for summary in self.bucket_summaries],
            "claim_policy": self.surface.get("claim_policy"),
            "gap_priority_model": self.surface.get("gap_priority_model"),
            "suite_partitions": self.surface.get("suite_partitions"),
            "longitudinal_policy": self.surface.get("longitudinal_policy"),
            "workflow_surface": self.workflow_surface,
            "retained_suite_summary": [
                summary.to_payload() for summary in self.retained_suite_summary
            ],
        }


@dataclass(frozen=True)
class ConformanceCorpusSurfaceReportWriter:
    paths: ConformanceCorpusPaths

    def write_failure(self, message: str) -> int:
        print(f"{CHECKER_NAME}: FAIL\n- {message}", file=sys.stderr)
        return 1

    def write_success(self, summary: ConformanceCorpusSurfaceSummary) -> int:
        self.paths.summary_path.parent.mkdir(parents=True, exist_ok=True)
        self.paths.summary_path.write_text(
            json.dumps(summary.to_payload(), indent=2) + "\n",
            encoding="utf-8",
        )
        print(f"summary_path: {repo_rel(self.paths.summary_path)}")
        print(f"{CHECKER_NAME}: OK")
        return 0


__all__ = (
    "ConformanceCorpusSurfaceReportWriter",
    "ConformanceCorpusSurfaceSummary",
    "ManifestBucketSummary",
    "RetainedSuiteSummary",
)
