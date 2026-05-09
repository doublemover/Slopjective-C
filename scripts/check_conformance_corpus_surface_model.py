"""Structured model and report writer for the conformance corpus checker."""

from __future__ import annotations

import json
import sys
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Mapping

from objc3c_tooling.json_io import load_json_any
from objc3c_tooling.paths import repo_rel


CHECKER_NAME = "conformance-corpus-surface"
SURFACE_CONTRACT_ID = "objc3c.conformance.corpus.surface.v1"
LONGITUDINAL_CONTRACT_ID = "objc3c.conformance.longitudinal_suites.v1"
SUMMARY_CONTRACT_ID = "objc3c.conformance.corpus.surface.summary.v1"
EXPECTED_PRIMARY_BUCKETS = [
    "parser",
    "semantic",
    "lowering_abi",
    "module_roundtrip",
    "diagnostics",
]
EXPECTED_SUPPLEMENTAL_BUCKETS = [
    "examples",
    "spec_open_issues",
    "workpacks",
]
EXPECTED_WORKFLOW_SURFACE = {
    "report_root": "tmp/reports/conformance",
    "artifact_root": "tmp/artifacts/conformance",
    "package_stage_root": "tmp/pkg/objc3c-native-runnable-toolchain",
    "surface_check_script": "scripts/check_conformance_corpus_surface.py",
    "coverage_index_script": "scripts/generate_conformance_corpus_index.py",
    "legacy_suite_gate_script": "scripts/check_conformance_suite.ps1",
    "coverage_map": "tests/conformance/COVERAGE_MAP.md",
    "longitudinal_suite_manifest": "tests/conformance/longitudinal_suites.json",
}


class SurfaceValidationError(RuntimeError):
    def __init__(self, message: str) -> None:
        super().__init__(message)
        self.message = message


@dataclass(frozen=True)
class SurfaceField:
    name: str
    expected: Any
    drift_message: str


@dataclass(frozen=True)
class ConformanceCorpusPaths:
    root: Path

    @property
    def corpus_surface_path(self) -> Path:
        return self.root / "tests" / "conformance" / "corpus_surface.json"

    @property
    def longitudinal_suites_path(self) -> Path:
        return self.root / "tests" / "conformance" / "longitudinal_suites.json"

    @property
    def summary_path(self) -> Path:
        return self.root / "tmp" / "reports" / "conformance" / "corpus-surface-summary.json"

    def require_path(self, relative_path: str, *, kind: str) -> Path:
        path = self.root / relative_path
        if not path.exists():
            raise RuntimeError(f"missing {kind}: {relative_path}")
        return path


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


SURFACE_FIELDS = (
    SurfaceField("contract_id", SURFACE_CONTRACT_ID, "corpus surface contract_id drifted"),
    SurfaceField("schema_version", 1, "corpus surface schema_version drifted"),
    SurfaceField("corpus_root", "tests/conformance", "corpus_root drifted"),
    SurfaceField("suite_readme", "tests/conformance/README.md", "suite_readme drifted"),
    SurfaceField("coverage_map", "tests/conformance/COVERAGE_MAP.md", "coverage_map drifted"),
    SurfaceField("runbook", "docs/runbooks/objc3c_conformance_corpus.md", "runbook drifted"),
    SurfaceField("primary_buckets", EXPECTED_PRIMARY_BUCKETS, "primary_buckets drifted"),
    SurfaceField(
        "supplemental_buckets",
        EXPECTED_SUPPLEMENTAL_BUCKETS,
        "supplemental_buckets drifted",
    ),
)


@dataclass(frozen=True)
class ConformanceCorpusSurfaceModel:
    paths: ConformanceCorpusPaths

    def build_summary(self) -> ConformanceCorpusSurfaceSummary:
        surface = self.load_surface()
        self.validate_surface_contract(surface)

        manifest_inventory = self.manifest_inventory(surface)
        required_manifest_keys = EXPECTED_PRIMARY_BUCKETS + ["examples", "spec_open_issues"]
        if sorted(manifest_inventory.keys()) != sorted(required_manifest_keys):
            raise SurfaceValidationError("manifest inventory keys drifted")

        bucket_summaries = tuple(
            self.summarize_manifest(bucket, manifest_inventory)
            for bucket in required_manifest_keys
        )
        workflow_surface = self.validate_artifact_and_workflow_surface(surface)
        retained_suite_summary = self.validate_longitudinal_suites(
            surface,
            required_manifest_keys,
        )
        return ConformanceCorpusSurfaceSummary(
            self.paths,
            surface,
            bucket_summaries,
            retained_suite_summary,
            workflow_surface,
        )

    def load_surface(self) -> Mapping[str, Any]:
        if not self.paths.corpus_surface_path.is_file():
            raise SurfaceValidationError(
                f"missing corpus surface contract: {repo_rel(self.paths.corpus_surface_path)}"
            )
        surface = load_json_any(self.paths.corpus_surface_path)
        if not isinstance(surface, dict):
            raise SurfaceValidationError("corpus surface contract is not an object")
        return surface

    def load_longitudinal_suites(self) -> Mapping[str, Any]:
        longitudinal_suites = load_json_any(self.paths.longitudinal_suites_path)
        if not isinstance(longitudinal_suites, dict):
            raise SurfaceValidationError("longitudinal_suites contract is not an object")
        return longitudinal_suites

    def validate_surface_contract(self, surface: Mapping[str, Any]) -> None:
        for field in SURFACE_FIELDS:
            if surface.get(field.name) != field.expected:
                raise SurfaceValidationError(field.drift_message)

        self.paths.require_path("tests/conformance/README.md", kind="suite readme")
        self.paths.require_path("tests/conformance/COVERAGE_MAP.md", kind="coverage map")
        self.paths.require_path("docs/runbooks/objc3c_conformance_corpus.md", kind="runbook")
        self.paths.require_path(
            "tests/conformance/longitudinal_suites.json",
            kind="longitudinal suite manifest",
        )

    def manifest_inventory(self, surface: Mapping[str, Any]) -> Mapping[str, Any]:
        taxonomy = surface.get("taxonomy")
        if not isinstance(taxonomy, dict):
            raise SurfaceValidationError("taxonomy is missing")
        manifest_inventory = taxonomy.get("manifest_inventory")
        if not isinstance(manifest_inventory, dict):
            raise SurfaceValidationError("taxonomy.manifest_inventory is missing")
        return manifest_inventory

    def summarize_manifest(
        self,
        bucket: str,
        manifest_inventory: Mapping[str, Any],
    ) -> ManifestBucketSummary:
        manifest_relative = manifest_inventory[bucket]
        if not isinstance(manifest_relative, str) or not manifest_relative:
            raise SurfaceValidationError(f"manifest inventory entry for {bucket} is missing")
        manifest_path = self.paths.require_path(manifest_relative, kind=f"{bucket} manifest")
        payload = load_json_any(manifest_path)
        if not isinstance(payload, dict):
            raise RuntimeError(f"{repo_rel(manifest_path)} is not a manifest object")
        groups = payload.get("groups")
        if not isinstance(groups, list) or not groups:
            raise RuntimeError(f"{repo_rel(manifest_path)} is missing non-empty groups")

        referenced_files: list[str] = []
        issue_refs: set[int] = set()
        for group in groups:
            if not isinstance(group, dict):
                raise RuntimeError(f"{repo_rel(manifest_path)} contains a non-object group")
            files = group.get("files")
            if not isinstance(files, list) or not files:
                raise RuntimeError(f"{repo_rel(manifest_path)} contains a group without files")
            for file_name in files:
                if not isinstance(file_name, str) or not file_name:
                    raise RuntimeError(f"{repo_rel(manifest_path)} contains a non-string file entry")
                fixture_path = manifest_path.parent / file_name
                if not fixture_path.is_file():
                    raise RuntimeError(
                        f"{repo_rel(manifest_path)} references missing fixture "
                        f"{repo_rel(fixture_path)}"
                    )
                referenced_files.append(file_name)
            issue = group.get("issue")
            if isinstance(issue, int):
                issue_refs.add(issue)
            issues = group.get("issues")
            if isinstance(issues, list):
                for value in issues:
                    if isinstance(value, int):
                        issue_refs.add(value)

        return ManifestBucketSummary(
            bucket=bucket,
            manifest_path=repo_rel(manifest_path),
            suite=payload.get("suite"),
            group_count=len(groups),
            fixture_count=len(referenced_files),
            issue_refs=tuple(sorted(issue_refs)),
        )

    def validate_artifact_and_workflow_surface(
        self,
        surface: Mapping[str, Any],
    ) -> Mapping[str, Any]:
        artifact_surface = surface.get("artifact_surface")
        if not isinstance(artifact_surface, dict):
            raise SurfaceValidationError("artifact_surface is missing")
        if (
            artifact_surface.get("surface_summary")
            != "tmp/reports/conformance/corpus-surface-summary.json"
        ):
            raise SurfaceValidationError("artifact_surface.surface_summary drifted")
        if (
            artifact_surface.get("coverage_index")
            != "tmp/reports/conformance/corpus-index.json"
        ):
            raise SurfaceValidationError("artifact_surface.coverage_index drifted")

        workflow_surface = surface.get("workflow_surface")
        if workflow_surface != EXPECTED_WORKFLOW_SURFACE:
            raise SurfaceValidationError("workflow_surface drifted")
        return workflow_surface

    def validate_longitudinal_suites(
        self,
        surface: Mapping[str, Any],
        required_manifest_keys: list[str],
    ) -> tuple[RetainedSuiteSummary, ...]:
        longitudinal_policy = surface.get("longitudinal_policy")
        if not isinstance(longitudinal_policy, dict):
            raise SurfaceValidationError("longitudinal_policy is missing")
        if (
            longitudinal_policy.get("suite_manifest")
            != "tests/conformance/longitudinal_suites.json"
        ):
            raise SurfaceValidationError("longitudinal_policy.suite_manifest drifted")

        longitudinal_payload = self.load_longitudinal_suites()
        if longitudinal_payload.get("contract_id") != LONGITUDINAL_CONTRACT_ID:
            raise SurfaceValidationError("longitudinal_suites contract_id drifted")
        if longitudinal_payload.get("schema_version") != 1:
            raise SurfaceValidationError("longitudinal_suites schema_version drifted")
        retained_suites = longitudinal_payload.get("retained_suites")
        if not isinstance(retained_suites, list) or not retained_suites:
            raise SurfaceValidationError("longitudinal_suites retained_suites is missing")

        retained_summary: list[RetainedSuiteSummary] = []
        known_buckets = set(required_manifest_keys)
        known_suite_classes = set(longitudinal_policy.get("retained_suite_classes", []))
        for entry in retained_suites:
            if not isinstance(entry, dict):
                raise SurfaceValidationError("longitudinal_suites contains a non-object entry")
            suite_id = entry.get("suite_id")
            suite_class = entry.get("suite_class")
            bucket = entry.get("bucket")
            manifest = entry.get("manifest")
            traceability_targets = entry.get("traceability_targets")
            if not all(
                isinstance(value, str) and value
                for value in (suite_id, suite_class, bucket, manifest)
            ):
                raise SurfaceValidationError(
                    "longitudinal_suites entry is missing suite_id/suite_class/bucket/manifest"
                )
            if bucket not in known_buckets:
                raise SurfaceValidationError(
                    f"longitudinal_suites references unknown bucket {bucket}"
                )
            if suite_class not in known_suite_classes:
                raise SurfaceValidationError(
                    f"longitudinal_suites references unknown suite_class {suite_class}"
                )
            if not isinstance(traceability_targets, list) or not traceability_targets:
                raise SurfaceValidationError(
                    f"longitudinal_suites entry {suite_id} is missing traceability_targets"
                )
            self.paths.require_path(
                manifest,
                kind=f"longitudinal manifest reference for {suite_id}",
            )
            retained_summary.append(
                RetainedSuiteSummary(
                    suite_id=suite_id,
                    suite_class=suite_class,
                    bucket=bucket,
                    manifest=manifest,
                    traceability_targets=tuple(traceability_targets),
                )
            )

        return tuple(retained_summary)
