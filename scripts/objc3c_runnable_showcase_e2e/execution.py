from __future__ import annotations

import subprocess
from pathlib import Path
from typing import Any

from .assertions import expect
from .constants import PWSH
from .models import (
    ShowcaseExecutionResult,
    ShowcaseExampleResult,
    ShowcasePackageSurface,
)
from .tooling import find_clangxx, load_json, normalize_rel_path, repo_rel, run_capture


def compile_showcase_example(
    *,
    package_root: Path,
    compile_wrapper: Path,
    source_path: Path,
    compile_dir: Path,
    example_id: str,
) -> None:
    compile_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(compile_wrapper),
            str(source_path),
            "--out-dir",
            str(compile_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=package_root,
    )
    if compile_result.returncode != 0:
        raise RuntimeError(f"packaged compile wrapper failed for showcase example {example_id}")


def assert_compile_artifacts(*, compile_dir: Path, example_id: str) -> dict[str, Path]:
    compile_artifacts = {
        "object": compile_dir / "module.obj",
        "registration_manifest": compile_dir / "module.runtime-registration-manifest.json",
        "compile_manifest": compile_dir / "module.manifest.json",
    }
    expect(
        compile_artifacts["object"].is_file(),
        f"packaged showcase compile did not publish {compile_artifacts['object']}",
    )
    expect(
        compile_artifacts["registration_manifest"].is_file(),
        (
            "packaged showcase compile did not publish "
            f"{compile_artifacts['registration_manifest']}"
        ),
    )
    expect(
        compile_artifacts["compile_manifest"].is_file(),
        f"packaged showcase compile did not publish {compile_artifacts['compile_manifest']}",
    )
    return compile_artifacts


def load_registration_manifest(
    *,
    registration_manifest_path: Path,
    expected_runtime_library: str,
    example_id: str,
) -> dict[str, Any]:
    registration_manifest = load_json(registration_manifest_path)
    actual_runtime_library = str(
        registration_manifest.get("runtime_support_library_archive_relative_path", "")
    )
    expect(
        actual_runtime_library == expected_runtime_library,
        (
            "packaged showcase registration manifest drifted from the packaged "
            f"runtime archive for {example_id}"
        ),
    )
    return registration_manifest


def link_showcase_executable(
    *,
    package_root: Path,
    clangxx: str,
    object_path: Path,
    runtime_library: Path,
    registration_manifest: dict[str, Any],
    exe_path: Path,
    link_log: Path,
    example_id: str,
) -> None:
    link_command = [
        clangxx,
        str(object_path),
        str(runtime_library),
        *[str(flag) for flag in registration_manifest.get("driver_linker_flags", [])],
        "-o",
        str(exe_path),
        "-fno-color-diagnostics",
    ]
    link_result = subprocess.run(
        link_command,
        cwd=package_root,
        check=False,
        text=True,
        capture_output=True,
    )
    link_log.write_text((link_result.stdout or "") + (link_result.stderr or ""), encoding="utf-8")
    if link_result.returncode != 0:
        raise RuntimeError(f"packaged showcase link failed for {example_id}")


def run_showcase_executable(
    *,
    package_root: Path,
    exe_path: Path,
    run_log: Path,
    expected_exit: int,
    example_id: str,
) -> int:
    run_result = subprocess.run(
        [str(exe_path)],
        cwd=package_root,
        check=False,
        text=True,
        capture_output=True,
    )
    run_log.write_text((run_result.stdout or "") + (run_result.stderr or ""), encoding="utf-8")
    if run_result.returncode != expected_exit:
        raise RuntimeError(
            f"packaged showcase example {example_id} exited {run_result.returncode}, expected {expected_exit}"
        )
    return int(run_result.returncode)


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
    )


def run_showcase_examples(
    *,
    package_root: Path,
    artifacts_root: Path,
    surface: ShowcasePackageSurface,
) -> ShowcaseExecutionResult:
    clangxx = find_clangxx()
    return ShowcaseExecutionResult(
        clangxx=clangxx,
        examples=[
            run_showcase_example(
                package_root=package_root,
                artifacts_root=artifacts_root,
                surface=surface,
                example=example,
                clangxx=clangxx,
            )
            for example in surface.showcase_examples
        ],
    )


__all__ = [
    "assert_compile_artifacts",
    "compile_showcase_example",
    "link_showcase_executable",
    "load_registration_manifest",
    "run_showcase_example",
    "run_showcase_examples",
    "run_showcase_executable",
]
