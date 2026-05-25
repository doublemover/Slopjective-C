#!/usr/bin/env python3
"""Build sanitizer packages and collect native runtime evidence."""

from __future__ import annotations

import argparse
import os
import re
import shutil
import stat
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.llvm_discovery import is_complete_llvm_root, llvm_root_candidates
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import command_text, python_script_command, run_completed
from scripts.objc3c_package_channels.sanitizer_contracts import (
    runtime_package_variant_contract,
)

SUMMARY_CONTRACT_ID = "objc3c.security.hardening.sanitizer.runtime-evidence.summary.v1"
PACKAGE_SCRIPT = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
PROBE_SCRIPT = ROOT / "scripts" / "probe_objc3c_sanitizer_runtime_evidence.py"
REPORT_ROOT = ROOT / "tmp" / "reports" / "sanitizer-runtime-evidence"
PACKAGE_ROOT_BASE = (
    ROOT
    / "tmp"
    / "sre"
    / "pkg"
)
WINDOWS_TARGET_PLATFORM = "windows-x64"
DEFAULT_MANIFEST_RELATIVE_PATH = "artifacts/package/objc3c-runnable-toolchain-package.json"
DEFAULT_FIXTURE_GLOB = "*assignment_basic_counter.objc3"
SAFE_RUN_ID_RE = re.compile(r"^[A-Za-z0-9][A-Za-z0-9_.-]{0,127}$")


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def output_tail(result: Any, *, max_chars: int = 4000) -> str:
    output_parts = []
    for stream_name in ("stdout", "stderr"):
        stream = getattr(result, stream_name, "")
        if stream:
            output_parts.append(f"{stream_name}:\n{stream[-max_chars:]}")
    return "\n".join(output_parts)


def pwsh_command() -> str:
    return shutil.which("pwsh") or shutil.which("powershell") or "pwsh"


def sanitizer_variants(raw: str) -> list[str]:
    if raw == "all":
        return ["address", "undefined"]
    return [raw]


def env_with_toolchain() -> dict[str, str]:
    env = os.environ.copy()
    for llvm_root in llvm_root_candidates():
        if is_complete_llvm_root(llvm_root):
            llvm_bin = str(llvm_root / "bin")
            env["PATH"] = llvm_bin + os.pathsep + env.get("PATH", "")
            break
    env.setdefault("OBJC3C_NATIVE_BUILD_PARALLELISM", "2")
    env.setdefault("CMAKE_BUILD_PARALLEL_LEVEL", "2")
    env.setdefault("CL_MPCount", "2")
    env.setdefault("LLVM_PARALLEL_COMPILE_JOBS", "2")
    return env


def is_under_root(path: Path, root: Path) -> bool:
    try:
        relative = path.resolve().relative_to(root.resolve())
    except ValueError:
        return False
    return bool(relative.parts)


def is_reparse_point(path: Path) -> bool:
    try:
        if path.is_symlink():
            return True
        is_junction = getattr(path, "is_junction", None)
        if callable(is_junction) and is_junction():
            return True
        attributes = getattr(path.lstat(), "st_file_attributes", 0)
    except OSError:
        return False
    return bool(attributes & getattr(stat, "FILE_ATTRIBUTE_REPARSE_POINT", 0))


def expect_plain_existing_ancestors(path: Path, owned_root: Path, label: str) -> None:
    owned_root = owned_root.resolve()
    path = path.resolve()
    try:
        relative = path.relative_to(owned_root)
    except ValueError as exc:
        raise RuntimeError(f"{label} must stay under owned root {repo_rel(owned_root)}: {path}") from exc

    current = owned_root
    if current.exists():
        expect(not is_reparse_point(current), f"{label} owned root is a reparse point: {current}")
    for segment in relative.parts[:-1]:
        current = current / segment
        if current.exists():
            expect(not is_reparse_point(current), f"{label} ancestor is a reparse point: {current}")


def expect_no_reparse_points(path: Path, label: str) -> None:
    if not path.exists():
        return
    expect(not is_reparse_point(path), f"{label} is a reparse point: {path}")
    for current, dir_names, file_names in os.walk(path):
        current_path = Path(current)
        expect(not is_reparse_point(current_path), f"{label} contains reparse point: {current_path}")
        for child_name in [*dir_names, *file_names]:
            child = current_path / child_name
            expect(not is_reparse_point(child), f"{label} contains reparse point: {child}")


def validate_run_id(raw_run_id: str, *, default_prefix: str = "") -> str:
    run_id = raw_run_id or (
        default_prefix + datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S_%f")
    )
    expect(
        SAFE_RUN_ID_RE.fullmatch(run_id) is not None,
        "sanitizer runtime evidence run id must be 1-128 chars of letters, digits, dot, underscore, or dash",
    )
    return run_id


def resolve_owned_path(raw_path: str, default_path: Path, owned_root: Path, label: str) -> Path:
    path = Path(raw_path) if raw_path else default_path
    if not path.is_absolute():
        path = ROOT / path
    resolved = path.resolve()
    expect(
        is_under_root(resolved, owned_root),
        f"{label} must stay under owned root {repo_rel(owned_root)}: {resolved}",
    )
    expect_plain_existing_ancestors(resolved, owned_root, label)
    return resolved


def expect_default_relative_path(raw_path: str, expected_path: str, label: str) -> str:
    normalized = raw_path.replace("\\", "/")
    expect(normalized == expected_path, f"{label} is pinned to {expected_path}")
    return normalized


def expect_default_probe_script(probe_script: Path) -> Path:
    resolved = probe_script.resolve()
    expect(
        resolved == PROBE_SCRIPT.resolve(),
        "sanitizer runtime evidence probe script rerouting is forbidden",
    )
    return resolved


def expected_runtime_library_artifacts(sanitizer_variant: str) -> set[str]:
    contract = runtime_package_variant_contract(sanitizer_variant)
    return {
        entry
        for entry in contract.runtime_library_payload_entries
        if entry != str(contract.runtime_library_manifest_path)
    }


def prepare_package_root(package_root: Path) -> None:
    package_root = package_root.resolve()
    expect(
        is_under_root(package_root, PACKAGE_ROOT_BASE),
        f"sanitizer package root must stay under owned root {repo_rel(PACKAGE_ROOT_BASE)}: {package_root}",
    )
    expect_plain_existing_ancestors(package_root, PACKAGE_ROOT_BASE, "sanitizer package root")
    if package_root.exists():
        expect(package_root.is_dir(), f"sanitizer package root exists but is not a directory: {package_root}")
        expect_no_reparse_points(package_root, "sanitizer package root")
        shutil.rmtree(package_root)
    package_root.mkdir(parents=True, exist_ok=True)


def run_package(
    *,
    sanitizer_variant: str,
    package_root: Path,
    parallelism: int,
    manifest_relative_path: str,
) -> dict[str, Any]:
    expect(PACKAGE_SCRIPT.is_file(), f"package script missing: {PACKAGE_SCRIPT}")
    prepare_package_root(package_root)
    command = [
        pwsh_command(),
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        str(PACKAGE_SCRIPT),
        "-PackageRoot",
        str(package_root),
        "-ManifestRelativePath",
        manifest_relative_path,
        "-SanitizerVariant",
        sanitizer_variant,
        "-Parallelism",
        str(parallelism),
    ]
    result = run_completed(command, cwd=ROOT, env=env_with_toolchain(), capture_output=True)
    expect(
        result.returncode == 0,
        "\n".join(
            part
            for part in (
                f"sanitizer package build failed: {command_text(command)}",
                output_tail(result),
            )
            if part
        ),
    )
    manifest_path = package_root / manifest_relative_path.replace("/", os.sep)
    expect(manifest_path.is_file(), f"sanitizer package manifest missing: {manifest_path}")
    manifest = load_json(manifest_path)
    contract = runtime_package_variant_contract(sanitizer_variant)
    sanitizer_package = manifest.get("sanitizer_package_variant", {})
    expect(isinstance(sanitizer_package, dict), "sanitizer package variant missing from package manifest")
    expect(manifest.get("runtime_variant") == contract.runtime_variant, "package runtime variant drifted after package build")
    expect(sanitizer_package.get("package_id") == contract.package_id, "package id drifted after package build")
    expect(sanitizer_package.get("package_channel_id") == contract.package_channel_id, "package channel drifted after package build")
    expect(sanitizer_package.get("target_platform_id") == WINDOWS_TARGET_PLATFORM, "package target platform drifted after package build")
    expect(
        sanitizer_package.get("runtime_library_manifest_path") == contract.runtime_library_manifest_path,
        "package runtime library manifest path drifted after package build",
    )
    runtime_artifacts = sanitizer_package.get("runtime_library_artifacts")
    expect(isinstance(runtime_artifacts, list), "package runtime library artifacts missing after package build")
    expect(
        {
            str(artifact.get("artifact", ""))
            for artifact in runtime_artifacts
            if isinstance(artifact, dict)
        }
        == expected_runtime_library_artifacts(sanitizer_variant),
        "package runtime library artifacts drifted after package build",
    )
    expect(sanitizer_package.get("support_truth") is False, "package build promoted sanitizer support truth")
    expect(sanitizer_package.get("native_execution_claimed") is False, "package build claimed sanitizer native execution")
    expect(sanitizer_package.get("default_release_channel_allowed") is False, "package build allowed default release channel fallback")
    expect(sanitizer_package.get("release_runtime_mixing_allowed") is False, "package build allowed release/sanitizer runtime mixing")
    if contract.trap_or_recover_mode is not None:
        expect(
            sanitizer_package.get("trap_or_recover_mode") == contract.trap_or_recover_mode,
            "package UBSan trap/recover mode drifted after package build",
        )
    return {
        "command": command,
        "exit_code": result.returncode,
        "package_root": repo_rel(package_root),
        "manifest_path": repo_rel(manifest_path),
        "package_id": contract.package_id,
        "package_channel_id": contract.package_channel_id,
    }


def run_probe(
    *,
    sanitizer_variant: str,
    package_root: Path,
    probe_script: Path,
    manifest_relative_path: str,
    fixture_glob: str,
    run_id: str,
) -> dict[str, Any]:
    expect(probe_script.is_file(), f"sanitizer runtime probe script missing: {probe_script}")
    report_dir = REPORT_ROOT / sanitizer_variant
    report_dir.mkdir(parents=True, exist_ok=True)
    probe_summary = report_dir / "probe.json"
    command = [
        *python_script_command(probe_script),
        "--sanitizer-variant",
        sanitizer_variant,
        "--package-root",
        str(package_root),
        "--manifest-relative-path",
        manifest_relative_path,
        "--summary-out",
        str(probe_summary),
        "--fixture-glob",
        fixture_glob,
        "--run-id",
        f"{run_id}-{sanitizer_variant}",
    ]
    result = run_completed(command, cwd=ROOT, env=env_with_toolchain(), capture_output=True)
    expect(
        result.returncode == 0,
        "\n".join(
            [
                f"sanitizer runtime probe failed: {command_text(command)}",
                output_tail(result),
            ]
        ),
    )
    expect(probe_summary.is_file(), f"sanitizer runtime probe summary missing: {probe_summary}")
    summary = load_json(probe_summary)
    expect(summary.get("status") == "PASS", "sanitizer runtime probe did not pass")
    expect(summary.get("sanitizer_variant") == sanitizer_variant, "sanitizer runtime probe variant drifted")
    expect(summary.get("support_truth") is False, "sanitizer runtime probe promoted support truth")
    expect(summary.get("native_execution_claimed") is False, "sanitizer runtime probe claimed native execution")
    diagnostics_path = REPORT_ROOT / sanitizer_variant / "diagnostics.json"
    write_json_file(
        diagnostics_path,
        {
            "contract_id": "objc3c.security.hardening.sanitizer.runtime-evidence.diagnostics.v1",
            "status": "PASS",
            "sanitizer_variant": sanitizer_variant,
            "target_platform_id": summary.get("target_platform_id"),
            "support_truth": False,
            "native_execution_claimed": False,
            "support_promotion_allowed": False,
            "packaged_execution_smoke": summary.get("packaged_execution_smoke", {}),
            "expected_detection_record": summary.get("expected_detection_record", {}),
        },
    )
    return {
        "command": command,
        "exit_code": result.returncode,
        "probe_summary": repo_rel(probe_summary),
        "diagnostics": repo_rel(diagnostics_path),
        "probe": summary,
    }


def build_summary(args: argparse.Namespace) -> dict[str, Any]:
    variants = sanitizer_variants(str(args.sanitizer_variant))
    target_platform = str(args.target_platform)
    expect(target_platform == WINDOWS_TARGET_PLATFORM, "sanitizer runtime evidence currently supports only windows-x64")
    run_id = validate_run_id(str(args.run_id))
    manifest_relative_path = expect_default_relative_path(
        str(args.manifest_relative_path),
        DEFAULT_MANIFEST_RELATIVE_PATH,
        "sanitizer runtime evidence manifest relative path",
    )
    fixture_glob = expect_default_relative_path(
        str(args.fixture_glob),
        DEFAULT_FIXTURE_GLOB,
        "sanitizer runtime evidence fixture glob",
    )
    probe_script = Path(args.probe_script) if args.probe_script else PROBE_SCRIPT
    if not probe_script.is_absolute():
        probe_script = ROOT / probe_script
    probe_script = expect_default_probe_script(probe_script)
    package_base = resolve_owned_path(
        str(args.package_root),
        PACKAGE_ROOT_BASE / run_id,
        PACKAGE_ROOT_BASE,
        "sanitizer package root base",
    )
    expect(int(args.parallelism) > 0, "sanitizer runtime evidence parallelism must be positive")

    results: list[dict[str, Any]] = []
    for sanitizer_variant in variants:
        package_root = package_base / sanitizer_variant / "package"
        package_result = run_package(
            sanitizer_variant=sanitizer_variant,
            package_root=package_root,
            parallelism=int(args.parallelism),
            manifest_relative_path=manifest_relative_path,
        )
        probe_result = run_probe(
            sanitizer_variant=sanitizer_variant,
            package_root=package_root,
            probe_script=probe_script,
            manifest_relative_path=manifest_relative_path,
            fixture_glob=fixture_glob,
            run_id=run_id,
        )
        results.append(
            {
                "sanitizer_variant": sanitizer_variant,
                "target_platform_id": target_platform,
                "package": package_result,
                "runtime_probe": probe_result,
            }
        )

    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "target_platform_id": target_platform,
        "support_truth": False,
        "native_execution_claimed": False,
        "support_promotion_allowed": False,
        "variants": results,
    }


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sanitizer-variant", choices=("address", "undefined", "all"), default="all")
    parser.add_argument("--target-platform", default=WINDOWS_TARGET_PLATFORM)
    parser.add_argument("--report", default="")
    parser.add_argument("--probe-script", default=str(PROBE_SCRIPT))
    parser.add_argument("--package-root", default="")
    parser.add_argument(
        "--manifest-relative-path",
        default=DEFAULT_MANIFEST_RELATIVE_PATH,
    )
    parser.add_argument("--fixture-glob", default=DEFAULT_FIXTURE_GLOB)
    parser.add_argument("--parallelism", type=int, default=2)
    parser.add_argument("--run-id", default="")
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    summary = build_summary(args)
    report = resolve_owned_path(
        str(args.report),
        REPORT_ROOT / "summary.json",
        REPORT_ROOT,
        "sanitizer runtime evidence report",
    )
    write_json_file(report, summary)
    print(f"summary_path: {repo_rel(report)}")
    print("objc3c-sanitizer-runtime-evidence: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
