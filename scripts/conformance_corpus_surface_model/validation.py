"""Validation checks for the conformance corpus surface checker."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import PurePosixPath
from typing import Any, Mapping

from objc3c_tooling.paths import repo_rel

from .contracts import (
    EXPECTED_PRIMARY_BUCKETS,
    EXPECTED_WORKFLOW_SURFACE,
    LONGITUDINAL_CONTRACT_ID,
    SURFACE_FIELDS,
    SurfaceValidationError,
)
from .loading import (
    ConformanceCorpusPaths,
    load_longitudinal_suites,
    load_manifest_payload,
    load_surface,
)
from .reports import (
    ConformanceCorpusSurfaceSummary,
    ManifestBucketSummary,
    RetainedSuiteSummary,
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
            manifest_inventory,
        )
        return ConformanceCorpusSurfaceSummary(
            self.paths,
            surface,
            bucket_summaries,
            retained_suite_summary,
            workflow_surface,
        )

    def load_surface(self) -> Mapping[str, Any]:
        return load_surface(self.paths)

    def load_longitudinal_suites(self) -> Mapping[str, Any]:
        return load_longitudinal_suites(self.paths)

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
        payload = load_manifest_payload(manifest_path)
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
        manifest_inventory: Mapping[str, Any],
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
        known_buckets = set(manifest_inventory.keys())
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
            if manifest != manifest_inventory[bucket]:
                raise SurfaceValidationError(
                    f"longitudinal_suites entry {suite_id} manifest drifted from "
                    f"taxonomy.manifest_inventory for {bucket}"
                )
            manifest_path = self.paths.require_path(
                manifest,
                kind=f"longitudinal manifest reference for {suite_id}",
            )
            manifest_payload = load_manifest_payload(manifest_path)
            known_targets = self.manifest_traceability_targets(manifest_payload)
            resolved_targets = self.validate_traceability_targets(
                suite_id,
                traceability_targets,
                known_targets,
            )
            retained_summary.append(
                RetainedSuiteSummary(
                    suite_id=suite_id,
                    suite_class=suite_class,
                    bucket=bucket,
                    manifest=manifest,
                    traceability_targets=resolved_targets,
                )
            )

        return tuple(retained_summary)

    def manifest_traceability_targets(self, manifest: Mapping[str, Any]) -> set[str]:
        groups = manifest.get("groups")
        if not isinstance(groups, list) or not groups:
            raise SurfaceValidationError(
                "longitudinal_suites manifest reference is missing non-empty groups"
            )

        targets: set[str] = set()
        for group in groups:
            if not isinstance(group, dict):
                raise SurfaceValidationError(
                    "longitudinal_suites manifest reference contains a non-object group"
                )

            name = group.get("name")
            if isinstance(name, str) and name:
                targets.add(name)

            issue = group.get("issue")
            if isinstance(issue, int):
                targets.add(str(issue))
                targets.add(f"#{issue}")

            issues = group.get("issues")
            if isinstance(issues, list):
                for value in issues:
                    if isinstance(value, int):
                        targets.add(str(value))
                        targets.add(f"#{value}")

            files = group.get("files")
            if isinstance(files, list):
                for file_name in files:
                    if isinstance(file_name, str) and file_name:
                        targets.add(PurePosixPath(file_name).stem)

        return targets

    def validate_traceability_targets(
        self,
        suite_id: str,
        traceability_targets: list[Any],
        known_targets: set[str],
    ) -> tuple[str, ...]:
        resolved_targets: list[str] = []
        duplicate_targets: set[str] = set()
        seen_targets: set[str] = set()
        for target in traceability_targets:
            if not isinstance(target, str) or not target:
                raise SurfaceValidationError(
                    f"longitudinal_suites entry {suite_id} has a non-string traceability target"
                )
            if target in seen_targets:
                duplicate_targets.add(target)
            seen_targets.add(target)
            resolved_targets.append(target)

        if duplicate_targets:
            duplicate_list = ", ".join(sorted(duplicate_targets))
            raise SurfaceValidationError(
                f"longitudinal_suites entry {suite_id} has duplicate traceability targets: "
                f"{duplicate_list}"
            )

        missing_targets = [
            target for target in resolved_targets if target not in known_targets
        ]
        if missing_targets:
            missing_list = ", ".join(missing_targets)
            raise SurfaceValidationError(
                f"longitudinal_suites entry {suite_id} traceability_targets lack "
                f"manifest evidence: {missing_list}"
            )

        return tuple(resolved_targets)


__all__ = ("ConformanceCorpusSurfaceModel",)
