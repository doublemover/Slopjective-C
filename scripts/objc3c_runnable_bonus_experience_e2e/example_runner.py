from __future__ import annotations

import shutil
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.paths import normalize_rel_path, repo_rel

from .assertions import expect
from .example_commands import (
    compile_template_source,
    link_template_executable,
    run_template_executable,
)
from .example_template import write_template_readme
from .models import BonusPackageSurface, ExampleRunResult


def materialize_and_run_example(
    *,
    package_root: Path,
    artifacts_root: Path,
    surface: BonusPackageSurface,
    example: dict[str, Any],
    portfolio_examples: dict[str, Any],
    clangxx: str,
) -> ExampleRunResult:
    expect(isinstance(example, dict), "package manifest published a malformed showcase example entry")
    example_id = str(example["example_id"])
    source_path = package_root / normalize_rel_path(str(example["source"]))
    workspace_manifest = package_root / normalize_rel_path(str(example["workspace_manifest"]))
    expect(source_path.is_file(), f"packaged runnable toolchain missing showcase source for {example_id}")
    expect(workspace_manifest.is_file(), f"packaged runnable toolchain missing workspace manifest for {example_id}")
    expect(example_id in portfolio_examples, f"packaged showcase portfolio missing example {example_id}")

    template_root = artifacts_root / example_id / "template"
    template_source = template_root / "src" / "main.objc3"
    template_readme = template_root / "README.md"
    compile_dir = artifacts_root / example_id / "compile"
    runtime_dir = artifacts_root / example_id / "runtime"
    exe_path = runtime_dir / "template.exe"
    link_log = runtime_dir / "link.log"
    run_log = runtime_dir / "run.log"

    template_source.parent.mkdir(parents=True, exist_ok=True)
    runtime_dir.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(source_path, template_source)
    write_template_readme(
        package_root=package_root,
        template_readme=template_readme,
        example_id=example_id,
        source_path=source_path,
        template_source=template_source,
        guided_walkthrough_manifest=surface.guided_walkthrough_manifest,
    )

    compile_template_source(
        package_root=package_root,
        compile_wrapper=surface.compile_wrapper,
        template_source=template_source,
        compile_dir=compile_dir,
        example_id=example_id,
    )

    object_path = compile_dir / "module.obj"
    registration_manifest_path = compile_dir / "module.runtime-registration-manifest.json"
    compile_manifest_path = compile_dir / "module.manifest.json"
    expect(object_path.is_file(), f"packaged bonus template compile did not publish {object_path}")
    expect(
        registration_manifest_path.is_file(),
        f"packaged bonus template compile did not publish {registration_manifest_path}",
    )
    expect(
        compile_manifest_path.is_file(),
        f"packaged bonus template compile did not publish {compile_manifest_path}",
    )

    registration_manifest = load_json(registration_manifest_path)
    expected_runtime_library = str(
        registration_manifest.get("runtime_support_library_archive_relative_path", "")
    )
    expect(
        expected_runtime_library == normalize_rel_path(str(surface.manifest["runtime_library"])),
        (
            "packaged bonus template registration manifest drifted from the "
            f"packaged runtime archive for {example_id}"
        ),
    )

    expected_exit = int(example["expected_exit_code"])
    link_template_executable(
        package_root=package_root,
        clangxx=clangxx,
        object_path=object_path,
        runtime_library=surface.runtime_library,
        registration_manifest=registration_manifest,
        exe_path=exe_path,
        link_log=link_log,
        example_id=example_id,
    )
    actual_exit = run_template_executable(
        package_root=package_root,
        exe_path=exe_path,
        run_log=run_log,
        expected_exit=expected_exit,
        example_id=example_id,
    )

    return ExampleRunResult(
        example_id=example_id,
        packaged_source=repo_rel(source_path),
        template_root=repo_rel(template_root),
        template_source=repo_rel(template_source),
        template_readme=repo_rel(template_readme),
        workspace_manifest=repo_rel(workspace_manifest),
        compile_dir=repo_rel(compile_dir),
        executable=repo_rel(exe_path),
        link_log=repo_rel(link_log),
        run_log=repo_rel(run_log),
        expected_exit_code=expected_exit,
        actual_exit_code=actual_exit,
    )


__all__ = ("materialize_and_run_example",)
