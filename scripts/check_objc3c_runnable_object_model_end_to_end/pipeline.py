"""Execution pipeline for the runnable object-model E2E checker."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import normalize_rel_path, repo_rel
from objc3c_tooling.probe_compile import find_clangxx
from objc3c_tooling.probe_output import parse_json_output
from objc3c_tooling.public_workflow_output import extract_output_value
from objc3c_tooling.public_workflow_output import extract_report_paths
from objc3c_tooling.subprocesses import run_capture

from .config import (
    PACKAGE_PS1,
    PWSH,
    REPORT_PATH,
    ROOT,
    RUNNER_PATH,
    SUMMARY_CONTRACT_ID,
)
from .expectations import (
    expect_compile_artifacts,
    expect_manifest_contract,
    expect_manifest_files,
    expect_probe_payload,
)
from .models import CompileArtifacts, E2ERunPaths, PackagedToolchain


def build_run_paths() -> E2ERunPaths:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-om-e2e" / run_id
    return E2ERunPaths(
        package_root=package_root,
        manifest_path=(
            package_root
            / "artifacts"
            / "package"
            / "objc3c-runnable-toolchain-package.json"
        ),
        compile_out_dir=(
            package_root
            / "tmp"
            / "artifacts"
            / "runnable-object-model-e2e"
            / "compile"
        ),
        probe_exe=(
            package_root
            / "tmp"
            / "artifacts"
            / "runnable-object-model-e2e"
            / "probe.exe"
        ),
    )


def package_toolchain(run_paths: E2ERunPaths) -> Any:
    package_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(PACKAGE_PS1),
            "-PackageRoot",
            str(run_paths.package_root),
        ],
        cwd=ROOT,
    )
    if package_result.returncode != 0:
        raise RuntimeError("runnable toolchain package command failed")
    return package_result


def load_package_manifest(run_paths: E2ERunPaths) -> dict[str, Any]:
    manifest = load_json(run_paths.manifest_path)
    expect_manifest_contract(manifest)
    return manifest


def resolve_packaged_toolchain(
    manifest: dict[str, Any],
    package_root: Path,
) -> PackagedToolchain:
    expect_manifest_files(manifest, package_root)
    return PackagedToolchain(
        compile_script=package_root / normalize_rel_path(str(manifest["compile_wrapper"])),
        canonical_fixture=(
            package_root / normalize_rel_path(str(manifest["canonical_runnable_fixture"]))
        ),
        runtime_library=package_root / normalize_rel_path(str(manifest["runtime_library"])),
        object_model_probe=(
            package_root / normalize_rel_path(str(manifest["object_model_probe"]))
        ),
        smoke_script=(
            package_root / normalize_rel_path(str(manifest["execution_smoke_script"]))
        ),
        replay_script=(
            package_root / normalize_rel_path(str(manifest["execution_replay_script"]))
        ),
    )


def compile_canonical_fixture(
    toolchain: PackagedToolchain,
    run_paths: E2ERunPaths,
) -> Any:
    compile_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(toolchain.compile_script),
            str(toolchain.canonical_fixture),
            "--out-dir",
            str(run_paths.compile_out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=run_paths.package_root,
    )
    if compile_result.returncode != 0:
        raise RuntimeError(
            "packaged compile wrapper failed for the canonical object-model fixture"
        )
    return compile_result


def compile_artifacts_for(run_paths: E2ERunPaths) -> CompileArtifacts:
    compile_artifacts = CompileArtifacts(
        manifest=run_paths.compile_out_dir / "module.manifest.json",
        registration_manifest=(
            run_paths.compile_out_dir / "module.runtime-registration-manifest.json"
        ),
        compile_provenance=run_paths.compile_out_dir / "module.compile-provenance.json",
        object=run_paths.compile_out_dir / "module.obj",
    )
    expect_compile_artifacts(compile_artifacts)
    return compile_artifacts


def compile_packaged_probe(
    toolchain: PackagedToolchain,
    compile_artifacts: CompileArtifacts,
    run_paths: E2ERunPaths,
) -> tuple[str, Any]:
    clangxx = find_clangxx()
    run_paths.probe_exe.parent.mkdir(parents=True, exist_ok=True)
    probe_compile_result = run_capture(
        [
            clangxx,
            "-std=c++20",
            "-fms-runtime-lib=dll",
            "-I",
            str((run_paths.package_root / "native/objc3c/src").resolve()),
            str(toolchain.object_model_probe),
            str(compile_artifacts.object),
            str(toolchain.runtime_library),
            "-o",
            str(run_paths.probe_exe),
        ],
        cwd=run_paths.package_root,
    )
    if probe_compile_result.returncode != 0:
        raise RuntimeError("packaged object-model probe compile failed")
    return clangxx, probe_compile_result


def run_packaged_probe(run_paths: E2ERunPaths) -> dict[str, Any]:
    probe_payload = parse_json_output(
        run_capture([str(run_paths.probe_exe)], cwd=run_paths.package_root),
        "packaged object-model runtime probe",
    )
    expect_probe_payload(probe_payload)
    return probe_payload


def run_packaged_smoke(toolchain: PackagedToolchain, package_root: Path) -> Any:
    smoke_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(toolchain.smoke_script),
            "-Limit",
            "12",
        ],
        cwd=package_root,
    )
    if smoke_result.returncode != 0:
        raise RuntimeError("packaged execution smoke failed")
    return smoke_result


def run_packaged_replay(toolchain: PackagedToolchain, package_root: Path) -> Any:
    replay_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(toolchain.replay_script),
            "-CaseId",
            "canonical-runnable",
        ],
        cwd=package_root,
    )
    if replay_result.returncode != 0:
        raise RuntimeError("packaged execution replay proof failed")
    return replay_result


def build_payload(
    run_paths: E2ERunPaths,
    toolchain: PackagedToolchain,
    compile_artifacts: CompileArtifacts,
    probe_payload: dict[str, Any],
    package_result: Any,
    compile_result: Any,
    probe_compile_result: Any,
    smoke_result: Any,
    replay_result: Any,
    clangxx: str,
) -> dict[str, Any]:
    compile_artifact_paths = compile_artifacts.as_dict()
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": RUNNER_PATH,
        "package_manifest_path": repo_rel(run_paths.manifest_path),
        "package_root": repo_rel(run_paths.package_root),
        "packaged_compile_fixture": repo_rel(toolchain.canonical_fixture),
        "packaged_probe": repo_rel(toolchain.object_model_probe),
        "packaged_probe_executable": repo_rel(run_paths.probe_exe),
        "packaged_compile_artifacts": {
            key: repo_rel(path) for key, path in compile_artifact_paths.items()
        },
        "probe_payload": probe_payload,
        "child_report_paths": [
            *extract_report_paths(package_result.stdout),
            *extract_report_paths(smoke_result.stdout),
            *extract_report_paths(replay_result.stdout),
        ],
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "exit_code": package_result.returncode,
                "package_root": extract_output_value(package_result.stdout, "package_root"),
                "manifest": extract_output_value(package_result.stdout, "manifest"),
            },
            {
                "action": "compile-canonical-object-model-fixture",
                "exit_code": compile_result.returncode,
            },
            {
                "action": "compile-packaged-object-model-probe",
                "exit_code": probe_compile_result.returncode,
                "clangxx": clangxx,
            },
            {
                "action": "run-packaged-object-model-probe",
                "exit_code": 0,
            },
            {
                "action": "packaged-execution-smoke",
                "exit_code": smoke_result.returncode,
                "report_paths": extract_report_paths(smoke_result.stdout),
            },
            {
                "action": "packaged-execution-replay",
                "exit_code": replay_result.returncode,
                "report_paths": extract_report_paths(replay_result.stdout),
            },
        ],
    }


def write_summary(payload: dict[str, Any]) -> None:
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)


def run_object_model_end_to_end() -> dict[str, Any]:
    run_paths = build_run_paths()
    package_result = package_toolchain(run_paths)
    manifest = load_package_manifest(run_paths)
    toolchain = resolve_packaged_toolchain(manifest, run_paths.package_root)
    compile_result = compile_canonical_fixture(toolchain, run_paths)
    compile_artifacts = compile_artifacts_for(run_paths)
    clangxx, probe_compile_result = compile_packaged_probe(
        toolchain,
        compile_artifacts,
        run_paths,
    )
    probe_payload = run_packaged_probe(run_paths)
    smoke_result = run_packaged_smoke(toolchain, run_paths.package_root)
    replay_result = run_packaged_replay(toolchain, run_paths.package_root)
    payload = build_payload(
        run_paths=run_paths,
        toolchain=toolchain,
        compile_artifacts=compile_artifacts,
        probe_payload=probe_payload,
        package_result=package_result,
        compile_result=compile_result,
        probe_compile_result=probe_compile_result,
        smoke_result=smoke_result,
        replay_result=replay_result,
        clangxx=clangxx,
    )
    write_summary(payload)
    return payload


def main() -> int:
    run_object_model_end_to_end()
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


__all__ = [
    "build_payload",
    "build_run_paths",
    "compile_artifacts_for",
    "compile_canonical_fixture",
    "compile_packaged_probe",
    "load_package_manifest",
    "main",
    "package_toolchain",
    "resolve_packaged_toolchain",
    "run_object_model_end_to_end",
    "run_packaged_probe",
    "run_packaged_replay",
    "run_packaged_smoke",
    "write_summary",
]
