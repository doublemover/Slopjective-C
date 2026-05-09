"""File and console rendering for materialization results."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel

from .contract_models import ProjectTemplatePaths


def canonical_workspace_readme_lines() -> list[str]:
    return [
        "# Canonical Application Workspace",
        "",
        "This machine-owned workspace packages the checked-in showcase and stdlib program surfaces into one copyable application architecture root.",
        "",
        "Replay commands:",
        "- `python scripts/materialize_objc3c_canonical_application_workspace.py`",
        "- `python scripts/check_application_architecture_template_harness.py`",
        "- `npm run objc3c -- validate-showcase`",
        "- `npm run objc3c -- validate-runnable-showcase`",
        "- `npm run objc3c -- validate-stdlib-program`",
        "- `npm run objc3c -- validate-runnable-stdlib-program`",
        "- `npm run objc3c -- package-runnable-toolchain`",
    ]


def project_template_readme_lines(
    *, example_id: str, source_origin: object, template_source: Path, root: Path
) -> list[str]:
    from objc3c_tooling.paths import display_path

    return [
        f"# {example_id} Template",
        "",
        "This machine-owned template is derived from the checked-in showcase portfolio.",
        "",
        f"- source example: `{source_origin}`",
        f"- generated source: `{display_path(template_source, root=root)}`",
        "- live commands:",
        f"  - `npm run objc3c -- materialize-project-template --example {example_id}`",
        f"  - `npm run objc3c -- materialize-playground-workspace {display_path(template_source, root=root)}`",
        f"  - `npm run objc3c -- benchmark-runtime-inspector {display_path(template_source, root=root)}`",
        "  - `npm run objc3c -- inspect-bonus-tool-integration`",
    ]


def write_lines(path: Path, lines: list[str]) -> None:
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def write_payload(path: Path, payload: dict[str, object]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(path, payload)


def print_canonical_workspace_result(
    *, root: Path, output_root: Path, workspace_manifest: Path, summary_path: Path
) -> None:
    print(f"workspace_root: {repo_rel(output_root, root=root)}")
    print(f"workspace_path: {repo_rel(workspace_manifest, root=root)}")
    print(f"summary_path: {repo_rel(summary_path, root=root)}")


def print_project_template_result(*, root: Path, paths: ProjectTemplatePaths) -> None:
    from objc3c_tooling.paths import display_path

    print(f"template_path: {display_path(paths.template_manifest, root=root)}")
    print(f"harness_path: {display_path(paths.harness_path, root=root)}")


def print_stdlib_workspace_result(
    *, root: Path, output_root: Path, summary_path: Path
) -> None:
    print(f"workspace_root: {repo_rel(output_root, root=root)}")
    print(f"summary_path: {repo_rel(summary_path, root=root)}")
