"""Structured stdlib surface inputs and summary report model."""

from __future__ import annotations

from dataclasses import dataclass
import re
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_any as load_json
from objc3c_tooling.json_io import write_report_json
from objc3c_tooling.paths import repo_rel


JsonObject = dict[str, Any]
SUMMARY_CONTRACT_ID = "objc3c.stdlib.surface.summary.v1"
TABLE_ROW_RE = re.compile(
    r"^\|\s*`(?P<module>[^`]+)`\s*\|\s*`(?P<capability>[^`]+)`\s*\|\s*(?P<profile>[^|]+?)\s*\|$"
)


@dataclass(frozen=True)
class SurfaceInput:
    path: Path
    missing_message: str


@dataclass(frozen=True)
class StdlibSurfacePaths:
    root: Path
    workspace: Path
    module_inventory: Path
    stability_policy: Path
    package_surface: Path
    core_architecture: Path
    advanced_architecture: Path
    semantic_policy: Path
    compatibility_gates: Path
    lowering_import_surface: Path
    advanced_helper_package_surface: Path
    program_surface: Path
    spec_contract: Path
    summary: Path

    @classmethod
    def from_script(cls, script_path: Path) -> "StdlibSurfacePaths":
        root = script_path.resolve().parents[1]
        return cls(
            root=root,
            workspace=root / "stdlib" / "workspace.json",
            module_inventory=root / "stdlib" / "module_inventory.json",
            stability_policy=root / "stdlib" / "stability_policy.json",
            package_surface=root / "stdlib" / "package_surface.json",
            core_architecture=root / "stdlib" / "core_architecture.json",
            advanced_architecture=root / "stdlib" / "advanced_architecture.json",
            semantic_policy=root / "stdlib" / "semantic_policy.json",
            compatibility_gates=root / "stdlib" / "compatibility_gates.json",
            lowering_import_surface=root / "stdlib" / "lowering_import_surface.json",
            advanced_helper_package_surface=root / "stdlib" / "advanced_helper_package_surface.json",
            program_surface=root / "stdlib" / "program_surface.json",
            spec_contract=root / "spec" / "STANDARD_LIBRARY_CONTRACT.md",
            summary=root / "tmp" / "reports" / "stdlib" / "surface-summary.json",
        )

    def required_inputs(self) -> tuple[SurfaceInput, ...]:
        return (
            SurfaceInput(self.workspace, "missing workspace contract"),
            SurfaceInput(self.module_inventory, "missing module inventory"),
            SurfaceInput(self.stability_policy, "missing stability policy"),
            SurfaceInput(self.package_surface, "missing package surface"),
            SurfaceInput(self.core_architecture, "missing core architecture contract"),
            SurfaceInput(self.advanced_architecture, "missing advanced architecture contract"),
            SurfaceInput(self.semantic_policy, "missing semantic policy contract"),
            SurfaceInput(self.compatibility_gates, "missing compatibility gate contract"),
            SurfaceInput(self.lowering_import_surface, "missing lowering/import surface contract"),
            SurfaceInput(
                self.advanced_helper_package_surface,
                "missing advanced helper package surface contract",
            ),
            SurfaceInput(self.program_surface, "missing program surface contract"),
            SurfaceInput(self.spec_contract, "missing spec contract"),
        )


@dataclass(frozen=True)
class StdlibSurfaceDocuments:
    workspace: JsonObject
    inventory: JsonObject
    stability_policy: JsonObject
    package_surface: JsonObject
    core_architecture: JsonObject
    advanced_architecture: JsonObject
    semantic_policy: JsonObject
    compatibility_gates: JsonObject
    lowering_import_surface: JsonObject
    advanced_helper_package_surface: JsonObject
    program_surface: JsonObject
    spec_text: str

    @classmethod
    def load(cls, paths: StdlibSurfacePaths) -> "StdlibSurfaceDocuments":
        return cls(
            workspace=load_json(paths.workspace),
            inventory=load_json(paths.module_inventory),
            stability_policy=load_json(paths.stability_policy),
            package_surface=load_json(paths.package_surface),
            core_architecture=load_json(paths.core_architecture),
            advanced_architecture=load_json(paths.advanced_architecture),
            semantic_policy=load_json(paths.semantic_policy),
            compatibility_gates=load_json(paths.compatibility_gates),
            lowering_import_surface=load_json(paths.lowering_import_surface),
            advanced_helper_package_surface=load_json(paths.advanced_helper_package_surface),
            program_surface=load_json(paths.program_surface),
            spec_text=paths.spec_contract.read_text(encoding="utf-8"),
        )


@dataclass(frozen=True)
class CanonicalModuleSurface:
    module: str
    implementation_module: str
    capability_id: str
    required_profile: str
    workspace_root: str
    source: str
    smoke_source: str
    manifest: str

    def path_for(self, key: str) -> str:
        if key == "workspace_root":
            return self.workspace_root
        if key == "source":
            return self.source
        if key == "smoke_source":
            return self.smoke_source
        if key == "manifest":
            return self.manifest
        raise KeyError(key)

    def expected_source_declaration(self) -> str:
        return f"module {self.implementation_module};"

    def to_spec_row(self) -> dict[str, str]:
        return {
            "module": self.module,
            "capability_id": self.capability_id,
            "required_profile": self.required_profile,
        }

    def to_report_row(self) -> dict[str, str]:
        return {
            "module": self.module,
            "implementation_module": self.implementation_module,
            "capability_id": self.capability_id,
            "required_profile": self.required_profile,
            "workspace_root": self.workspace_root,
            "source": self.source,
            "smoke_source": self.smoke_source,
            "manifest": self.manifest,
        }


@dataclass(frozen=True)
class PackageImportSurface:
    canonical_module: str
    implementation_module: str
    source_declaration: str


@dataclass(frozen=True)
class StdlibSurfaceReport:
    paths: StdlibSurfacePaths
    canonical_modules: list[CanonicalModuleSurface]
    layers: Any
    module_imports: Any
    api_families: Any
    advanced_api_families: Any
    required_exports: Any
    advanced_required_exports: Any
    module_semver: Any
    compatibility_gates: Any
    artifact_filenames: Any
    advanced_helper_modules: Any
    capability_demo_examples: Any

    def to_json(self) -> JsonObject:
        return {
            "contract_id": SUMMARY_CONTRACT_ID,
            "schema_version": 1,
            "status": "PASS",
            "workspace_contract": repo_rel(self.paths.workspace),
            "module_inventory": repo_rel(self.paths.module_inventory),
            "stability_policy": repo_rel(self.paths.stability_policy),
            "package_surface": repo_rel(self.paths.package_surface),
            "core_architecture": repo_rel(self.paths.core_architecture),
            "advanced_architecture": repo_rel(self.paths.advanced_architecture),
            "semantic_policy": repo_rel(self.paths.semantic_policy),
            "compatibility_gates": repo_rel(self.paths.compatibility_gates),
            "lowering_import_surface": repo_rel(self.paths.lowering_import_surface),
            "advanced_helper_package_surface": repo_rel(self.paths.advanced_helper_package_surface),
            "program_surface": repo_rel(self.paths.program_surface),
            "spec_contract": repo_rel(self.paths.spec_contract),
            "canonical_modules": [
                module_surface.to_report_row()
                for module_surface in self.canonical_modules
            ],
            "layers": self.layers,
            "module_imports": self.module_imports,
            "api_families": self.api_families,
            "advanced_api_families": self.advanced_api_families,
            "required_exports": self.required_exports,
            "advanced_required_exports": self.advanced_required_exports,
            "module_semver": self.module_semver,
            "compatibility_gates_summary": self.compatibility_gates,
            "artifact_filenames": self.artifact_filenames,
            "advanced_helper_modules": self.advanced_helper_modules,
            "capability_demo_examples": self.capability_demo_examples,
        }


def parse_spec_canonical_modules(spec_text: str) -> list[dict[str, str]]:
    rows: list[dict[str, str]] = []
    for raw_line in spec_text.splitlines():
        match = TABLE_ROW_RE.match(raw_line.strip())
        if not match:
            continue
        module = match.group("module")
        capability = match.group("capability")
        if not module.startswith("objc3."):
            continue
        rows.append(
            {
                "module": module,
                "capability_id": capability,
                "required_profile": match.group("profile").strip(),
            }
        )
    return rows


def write_stdlib_surface_report(report: StdlibSurfaceReport) -> None:
    write_report_json(report.paths.summary, report.to_json(), sort_keys=False)
