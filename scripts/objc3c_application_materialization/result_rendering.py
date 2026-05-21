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
    *,
    example_id: str,
    source_origin: object,
    template_source: Path,
    template_workspace_manifest: Path,
    compile_artifact_root: Path,
    root: Path,
) -> list[str]:
    from objc3c_tooling.paths import display_path

    template_source_path = display_path(template_source, root=root)
    template_workspace_path = display_path(template_workspace_manifest, root=root)
    compile_root_path = display_path(compile_artifact_root, root=root)
    return [
        f"# {example_id} Template",
        "",
        "This generated starter workspace is derived from checked-in showcase sources.",
        "Treat this directory as a reproducible output; the checked-in showcase source, workspace manifest, and tutorial contract remain the source truth.",
        "",
        f"- source example: `{source_origin}`",
        f"- generated source: `{template_source_path}`",
        f"- generated workspace manifest: `{template_workspace_path}`",
        f"- generated compile outputs: `{compile_root_path}`",
        "",
        "Normal developer loop:",
        f"  - `npm run objc3c -- materialize-project-template --example {example_id}`",
        (
            "  - `npm run objc3c -- compile-objc3c "
            f"{template_source_path} --out-dir "
            f"{compile_root_path} --emit-prefix module`"
        ),
        f"  - `npm run objc3c -- inspect-compile-observability {template_source_path}`",
        "  - `npm run objc3c -- validate-getting-started`",
        "",
        "If compilation fails:",
        f"  - open `{compile_root_path}/module.diagnostics.json`",
        f"  - rerun `npm run objc3c -- inspect-compile-observability {template_source_path}`",
        "",
        "Additional public tooling checks:",
        f"  - `npm run objc3c -- materialize-playground-workspace {template_source_path}`",
        f"  - `npm run objc3c -- benchmark-runtime-inspector {template_source_path}`",
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
