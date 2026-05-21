"""Copy and directory materialization owners for application workspaces."""

from __future__ import annotations

import shutil
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel

from .contract_models import (
    CanonicalWorkspaceMaterialization,
    ProjectTemplatePaths,
    StdlibModuleMaterialization,
    StdlibWorkspaceMaterialization,
)


STDLIB_ROOT_FILES = [
    Path("stdlib/README.md"),
    Path("stdlib/workspace.json"),
    Path("stdlib/module_inventory.json"),
    Path("stdlib/stability_policy.json"),
    Path("stdlib/package_surface.json"),
    Path("stdlib/core_architecture.json"),
    Path("stdlib/advanced_architecture.json"),
    Path("stdlib/semantic_policy.json"),
    Path("stdlib/compatibility_gates.json"),
    Path("stdlib/lowering_import_surface.json"),
    Path("stdlib/advanced_helper_package_surface.json"),
    Path("docs/runbooks/objc3c_stdlib_foundation.md"),
    Path("docs/runbooks/objc3c_stdlib_core.md"),
    Path("docs/runbooks/objc3c_stdlib_advanced.md"),
    Path("spec/STANDARD_LIBRARY_CONTRACT.md"),
]


def resolve_output_dir(*, root: Path, raw_out_dir: str, default_output_root: Path) -> Path:
    if raw_out_dir:
        candidate = Path(raw_out_dir)
        if not candidate.is_absolute():
            candidate = root / candidate
        return candidate.resolve()
    return default_output_root.resolve()


def resolve_stdlib_output_dir(
    *, root: Path, raw_out_dir: str, default_output_root: Path
) -> Path:
    if raw_out_dir:
        return resolve_output_dir(
            root=root, raw_out_dir=raw_out_dir, default_output_root=default_output_root
        )
    run_id = datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S_%f")
    return (default_output_root / run_id).resolve()


def project_template_paths(
    *, artifact_root: Path, report_root: Path, example_id: str
) -> ProjectTemplatePaths:
    template_root = artifact_root / example_id
    example_report_root = report_root / example_id
    return ProjectTemplatePaths(
        template_root=template_root,
        report_root=example_report_root,
        template_source=template_root / "src" / "main.objc3",
        template_readme=template_root / "README.md",
        template_manifest=template_root / "template.json",
        compile_artifact_root=template_root / "build",
        harness_path=example_report_root / "demo-harness.json",
    )


def materialize_project_template_source(
    *, example_source: Path, paths: ProjectTemplatePaths
) -> None:
    paths.template_source.parent.mkdir(parents=True, exist_ok=True)
    paths.report_root.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(example_source, paths.template_source)


def materialize_canonical_workspace(
    *,
    root: Path,
    output_root: Path,
    portfolio: dict[str, object],
    stdlib_program_surface_path: Path,
) -> CanonicalWorkspaceMaterialization:
    if output_root.exists():
        shutil.rmtree(output_root)
    output_root.mkdir(parents=True, exist_ok=True)

    copied_paths: list[str] = []
    included_examples = _copy_canonical_examples(
        root=root,
        output_root=output_root,
        examples=portfolio.get("examples", []),
        copied_paths=copied_paths,
    )
    _copy_canonical_stdlib_surface(
        root=root,
        output_root=output_root,
        stdlib_program_surface_path=stdlib_program_surface_path,
        copied_paths=copied_paths,
    )
    return CanonicalWorkspaceMaterialization(
        output_root=output_root,
        workspace_manifest=output_root / "workspace.json",
        readme=output_root / "README.md",
        copied_paths=copied_paths,
        included_examples=included_examples,
    )


def materialize_stdlib_workspace(
    *,
    root: Path,
    output_root: Path,
    inventory: dict[str, Any],
) -> StdlibWorkspaceMaterialization:
    output_root.mkdir(parents=True, exist_ok=True)
    copied_paths: list[str] = []
    for relative in STDLIB_ROOT_FILES:
        destination = output_root / relative
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(root / relative, destination)
        copied_paths.append(relative.as_posix())

    modules: list[StdlibModuleMaterialization] = []
    for entry in inventory["canonical_modules"]:
        module_entry = dict(entry)
        workspace_root = root / str(module_entry["workspace_root"])
        destination_root = output_root / str(module_entry["workspace_root"])
        destination_root.parent.mkdir(parents=True, exist_ok=True)
        if destination_root.exists():
            shutil.rmtree(destination_root)
        shutil.copytree(workspace_root, destination_root)
        copied_paths.extend(
            [
                str(module_entry["manifest"]),
                str(module_entry["source"]),
                str(module_entry["smoke_source"]),
            ]
        )
        modules.append(
            StdlibModuleMaterialization(
                canonical_module=module_entry["module"],
                implementation_module=module_entry["implementation_module"],
                workspace_root=str(module_entry["workspace_root"]),
                manifest=str(module_entry["manifest"]),
                source=str(module_entry["source"]),
                smoke_source=str(module_entry["smoke_source"]),
            )
        )

    return StdlibWorkspaceMaterialization(
        output_root=output_root,
        summary_path=output_root / "stdlib.materialized.workspace.json",
        copied_paths=copied_paths,
        modules=modules,
    )


def _copy_canonical_examples(
    *,
    root: Path,
    output_root: Path,
    examples: object,
    copied_paths: list[str],
) -> list[dict[str, object]]:
    included_examples: list[dict[str, object]] = []
    if not isinstance(examples, list):
        return included_examples

    for entry in examples:
        if not isinstance(entry, dict):
            continue
        example_id = str(entry["id"])
        source_path = root / str(entry["source"])
        workspace_manifest = root / str(entry["workspace_manifest"])
        destination_root = output_root / "examples" / example_id
        destination_root.mkdir(parents=True, exist_ok=True)
        copied_source = destination_root / "main.objc3"
        copied_workspace = destination_root / "workspace.json"
        shutil.copy2(source_path, copied_source)
        shutil.copy2(workspace_manifest, copied_workspace)
        copied_paths.extend(
            [
                repo_rel(source_path, root=root),
                repo_rel(workspace_manifest, root=root),
            ]
        )
        included_examples.append(
            {
                "id": example_id,
                "source": repo_rel(copied_source, root=root),
                "workspace_manifest": repo_rel(copied_workspace, root=root),
                "story_capabilities": entry.get("story_capabilities", []),
                "stdlib_followup_modules": entry.get("stdlib_followup_modules", []),
            }
        )
    return included_examples


def _copy_canonical_stdlib_surface(
    *,
    root: Path,
    output_root: Path,
    stdlib_program_surface_path: Path,
    copied_paths: list[str],
) -> None:
    copied_stdlib_workspace = output_root / "stdlib" / "workspace.json"
    copied_stdlib_program_surface = output_root / "stdlib" / "program_surface.json"
    copied_stdlib_workspace.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(root / "stdlib" / "workspace.json", copied_stdlib_workspace)
    shutil.copy2(stdlib_program_surface_path, copied_stdlib_program_surface)
    copied_paths.extend(["stdlib/workspace.json", "stdlib/program_surface.json"])
