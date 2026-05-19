from __future__ import annotations

from pathlib import Path
from typing import Any

from .assertions import expect
from .constants import PWSH
from .tooling import load_json, run_capture


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


__all__ = (
    "assert_compile_artifacts",
    "compile_showcase_example",
    "load_registration_manifest",
)
