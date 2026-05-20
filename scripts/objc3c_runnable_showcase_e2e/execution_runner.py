from __future__ import annotations

from pathlib import Path

from .assertions import expect
from .execution_compile import (
    assert_compile_artifacts,
    compile_showcase_example,
    load_registration_manifest,
)
from .execution_runtime import link_showcase_executable, run_showcase_executable
from .models import ShowcaseExampleResult, ShowcasePackageSurface
from .tooling import load_json, normalize_rel_path, repo_rel


def run_showcase_example(
    *,
    package_root: Path,
    artifacts_root: Path,
    surface: ShowcasePackageSurface,
    example: object,
    clangxx: str,
) -> ShowcaseExampleResult:
    expect(isinstance(example, dict), "package manifest published a malformed showcase example entry")
    assert isinstance(example, dict)
    example_id = str(example["example_id"])
    source_path = package_root / normalize_rel_path(str(example["source"]))
    workspace_manifest = package_root / normalize_rel_path(str(example["workspace_manifest"]))
    expect(source_path.is_file(), f"packaged runnable toolchain missing showcase source for {example_id}")
    expect(
        workspace_manifest.is_file(),
        f"packaged runnable toolchain missing workspace manifest for {example_id}",
    )

    workspace_payload = load_json(workspace_manifest)
    expected_exit = int(example["expected_exit_code"])
    demo_packages_by_example = {
        str(package.get("example_id")): package
        for package in surface.showcase_demo_packages
        if isinstance(package, dict)
    }
    demo_package = demo_packages_by_example.get(example_id)
    expect(demo_package is not None, f"packaged demo package missing for {example_id}")
    assert isinstance(demo_package, dict)
    expect(
        int(demo_package["expected_exit_code"]) == expected_exit,
        f"packaged demo package expected exit drifted for {example_id}",
    )
    expect(
        int(workspace_payload["runtime_surface"]["expected_exit_code"]) == expected_exit,
        f"workspace runtime surface drifted for {example_id}",
    )

    compile_dir = artifacts_root / example_id / "compile"
    compile_showcase_example(
        package_root=package_root,
        compile_wrapper=surface.compile_wrapper,
        source_path=source_path,
        compile_dir=compile_dir,
        example_id=example_id,
    )
    compile_artifacts = assert_compile_artifacts(
        compile_dir=compile_dir,
        example_id=example_id,
    )
    registration_manifest = load_registration_manifest(
        registration_manifest_path=compile_artifacts["registration_manifest"],
        expected_runtime_library=normalize_rel_path(str(surface.manifest["runtime_library"])),
        example_id=example_id,
    )

    runtime_dir = artifacts_root / example_id / "runtime"
    runtime_dir.mkdir(parents=True, exist_ok=True)
    exe_path = runtime_dir / "module.exe"
    link_log = runtime_dir / "link.log"
    run_log = runtime_dir / "run.log"

    link_showcase_executable(
        package_root=package_root,
        clangxx=clangxx,
        object_path=compile_artifacts["object"],
        runtime_library=surface.runtime_library,
        registration_manifest=registration_manifest,
        exe_path=exe_path,
        link_log=link_log,
        example_id=example_id,
    )
    actual_exit = run_showcase_executable(
        package_root=package_root,
        exe_path=exe_path,
        run_log=run_log,
        expected_exit=expected_exit,
        example_id=example_id,
    )

    return ShowcaseExampleResult(
        example_id=example_id,
        source=repo_rel(source_path),
        workspace_manifest=repo_rel(workspace_manifest),
        compile_dir=repo_rel(compile_dir),
        executable=repo_rel(exe_path),
        link_log=repo_rel(link_log),
        run_log=repo_rel(run_log),
        expected_exit_code=expected_exit,
        actual_exit_code=actual_exit,
        demo_package_id=str(demo_package["package_id"]),
        coverage_domain=str(demo_package["coverage_domain"]),
    )


__all__ = ("run_showcase_example",)
