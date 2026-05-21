"""Contract models for application and stdlib materialization outputs."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

CANONICAL_WORKSPACE_CONTRACT_ID = (
    "objc3c.application.architecture.testing.canonical_workspace.v1"
)
CANONICAL_WORKSPACE_SUMMARY_CONTRACT_ID = (
    "objc3c.application.architecture.testing.canonical_workspace.summary.v1"
)
PROJECT_TEMPLATE_CONTRACT_ID = "objc3c.project.template.surface.v1"
PROJECT_TEMPLATE_HARNESS_CONTRACT_ID = "objc3c.project.template.demo.harness.v1"
STDLIB_WORKSPACE_SUMMARY_CONTRACT_ID = "objc3c.stdlib.materialized.workspace.v1"


@dataclass(frozen=True)
class CanonicalWorkspaceMaterialization:
    output_root: Path
    workspace_manifest: Path
    readme: Path
    copied_paths: list[str]
    included_examples: list[dict[str, object]]


@dataclass(frozen=True)
class StdlibModuleMaterialization:
    canonical_module: object
    implementation_module: object
    workspace_root: str
    manifest: str
    source: str
    smoke_source: str

    def to_payload(self) -> dict[str, object]:
        return {
            "canonical_module": self.canonical_module,
            "implementation_module": self.implementation_module,
            "workspace_root": self.workspace_root,
            "manifest": self.manifest,
            "source": self.source,
            "smoke_source": self.smoke_source,
        }


@dataclass(frozen=True)
class StdlibWorkspaceMaterialization:
    output_root: Path
    summary_path: Path
    copied_paths: list[str]
    modules: list[StdlibModuleMaterialization]


@dataclass(frozen=True)
class ProjectTemplatePaths:
    template_root: Path
    report_root: Path
    template_source: Path
    template_workspace_manifest: Path
    template_readme: Path
    template_manifest: Path
    compile_artifact_root: Path
    harness_path: Path
