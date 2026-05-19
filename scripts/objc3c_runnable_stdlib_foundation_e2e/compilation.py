from __future__ import annotations

from pathlib import Path

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.paths import normalize_rel_path, repo_rel
from objc3c_tooling.subprocesses import run_capture

from .assertions import expect
from .constants import PWSH
from .models import StdlibCompileResult, StdlibPackageSurface


def compile_stdlib_module(
    *,
    package_root: Path,
    artifacts_root: Path,
    surface: StdlibPackageSurface,
    module: object,
) -> StdlibCompileResult:
    expect(
        isinstance(module, dict),
        "package manifest published a malformed stdlib module entry",
    )
    assert isinstance(module, dict)
    canonical_module = str(module["canonical_module"])
    implementation_module = str(module["implementation_module"])
    smoke_source = package_root / normalize_rel_path(str(module["smoke_source"]))
    compile_dir = artifacts_root / canonical_module.replace(".", "_")
    compile_dir.mkdir(parents=True, exist_ok=True)

    expect(
        smoke_source.is_file(),
        f"packaged runnable toolchain missing stdlib smoke source for {canonical_module}",
    )

    compile_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(surface.compile_wrapper),
            str(smoke_source),
            "--out-dir",
            str(compile_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=package_root,
    )
    if compile_result.returncode != 0:
        raise RuntimeError(
            f"packaged compile wrapper failed for stdlib module {canonical_module}"
        )

    artifact_filenames = surface.lowering_import_surface_payload["artifact_filenames"]
    object_path = compile_dir / str(artifact_filenames["object"])
    compile_manifest_path = compile_dir / str(artifact_filenames["compile_manifest"])
    registration_manifest_path = compile_dir / str(
        artifact_filenames["runtime_registration_manifest"]
    )
    expect(
        object_path.is_file(),
        f"packaged stdlib smoke compile did not publish {object_path}",
    )
    expect(
        compile_manifest_path.is_file(),
        f"packaged stdlib smoke compile did not publish {compile_manifest_path}",
    )
    expect(
        registration_manifest_path.is_file(),
        f"packaged stdlib smoke compile did not publish {registration_manifest_path}",
    )

    registration_manifest = load_json(registration_manifest_path)
    expect(
        str(registration_manifest.get("runtime_support_library_archive_relative_path", ""))
        == normalize_rel_path(str(surface.manifest["runtime_library"])),
        (
            "packaged stdlib registration manifest drifted from the packaged runtime "
            f"archive for {canonical_module}"
        ),
    )

    return StdlibCompileResult(
        canonical_module=canonical_module,
        implementation_module=implementation_module,
        smoke_source=repo_rel(smoke_source),
        artifact_root=repo_rel(compile_dir),
        object=repo_rel(object_path),
        compile_manifest=repo_rel(compile_manifest_path),
        registration_manifest=repo_rel(registration_manifest_path),
    )


def compile_stdlib_modules(
    *,
    package_root: Path,
    artifacts_root: Path,
    surface: StdlibPackageSurface,
) -> list[StdlibCompileResult]:
    return [
        compile_stdlib_module(
            package_root=package_root,
            artifacts_root=artifacts_root,
            surface=surface,
            module=module,
        )
        for module in surface.stdlib_modules
    ]
