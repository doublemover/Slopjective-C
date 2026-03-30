#!/usr/bin/env python3
"""Validate the staged runnable advanced stdlib helper surface end to end from the package root."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
REPORT_PATH = ROOT / "tmp" / "reports" / "stdlib" / "advanced-runnable-end-to-end-summary.json"
PACKAGE_CONTRACT_ID = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"
SUMMARY_CONTRACT_ID = "objc3c.stdlib.advanced.runnable.end.to.end.summary.v1"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def normalize_rel_path(raw_path: str) -> str:
    return raw_path.replace("\\", "/")


def run_capture(command: Sequence[str], *, cwd: Path) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        list(command),
        cwd=cwd,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def load_json(path: Path) -> dict[str, Any]:
    expect(path.is_file(), f"expected JSON artifact was not published: {path}")
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {path} did not contain an object")
    return payload


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-stdlib-advanced-e2e" / run_id
    manifest_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    artifacts_root = package_root / "tmp" / "artifacts" / "stdlib" / "advanced-runnable-e2e"

    package_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(PACKAGE_PS1),
            "-PackageRoot",
            str(package_root),
        ],
        cwd=ROOT,
    )
    if package_result.returncode != 0:
        raise RuntimeError("runnable toolchain package command failed")

    manifest = load_json(manifest_path)
    expect(manifest.get("contract_id") == PACKAGE_CONTRACT_ID, "unexpected package contract id")

    command_surfaces = manifest.get("command_surfaces", {})
    expect(command_surfaces.get("stdlib_advanced") == "npm run test:stdlib:advanced", "package manifest missing stdlib_advanced command surface")
    expect(command_surfaces.get("stdlib_advanced_e2e") == "npm run test:stdlib:advanced:e2e", "package manifest missing stdlib_advanced_e2e command surface")

    compile_wrapper = package_root / normalize_rel_path(str(manifest["compile_wrapper"]))
    runtime_library = package_root / normalize_rel_path(str(manifest["runtime_library"]))
    advanced_helper_package_surface = package_root / normalize_rel_path(
        str(manifest["stdlib_advanced_helper_package_surface"])
    )

    for path in (compile_wrapper, runtime_library, advanced_helper_package_surface):
        expect(path.is_file(), f"packaged runnable toolchain missing required advanced stdlib file {path}")

    advanced_helper_package_surface_payload = load_json(advanced_helper_package_surface)
    expect(
        manifest.get("advanced_helper_modules") == advanced_helper_package_surface_payload.get("advanced_helper_modules"),
        "package manifest advanced helper module inventory drifted",
    )
    expect(
        manifest.get("advanced_helper_command_surfaces")
        == advanced_helper_package_surface_payload.get("advanced_helper_command_surfaces"),
        "package manifest advanced helper command surfaces drifted",
    )
    expect(
        manifest.get("advanced_helper_profile_gates")
        == advanced_helper_package_surface_payload.get("advanced_helper_profile_gates"),
        "package manifest advanced helper profile gates drifted",
    )

    compile_results: list[dict[str, Any]] = []
    for module in manifest["advanced_helper_modules"]:
        expect(isinstance(module, dict), "package manifest published a malformed advanced helper module entry")
        canonical_module = str(module["canonical_module"])
        implementation_module = str(module["implementation_module"])
        smoke_source = package_root / normalize_rel_path(str(module["smoke_source"]))
        compile_dir = artifacts_root / canonical_module.replace(".", "_")
        compile_dir.mkdir(parents=True, exist_ok=True)

        expect(smoke_source.is_file(), f"packaged runnable toolchain missing advanced helper smoke source for {canonical_module}")

        compile_result = run_capture(
            [
                PWSH,
                "-NoProfile",
                "-ExecutionPolicy",
                "Bypass",
                "-File",
                str(compile_wrapper),
                str(smoke_source),
                "--out-dir",
                str(compile_dir),
                "--emit-prefix",
                "module",
            ],
            cwd=package_root,
        )
        if compile_result.returncode != 0:
            raise RuntimeError(f"packaged compile wrapper failed for advanced helper module {canonical_module}")

        object_path = compile_dir / "module.obj"
        compile_manifest_path = compile_dir / "module.manifest.json"
        registration_manifest_path = compile_dir / "module.runtime-registration-manifest.json"
        expect(object_path.is_file(), f"packaged advanced helper smoke compile did not publish {object_path}")
        expect(compile_manifest_path.is_file(), f"packaged advanced helper smoke compile did not publish {compile_manifest_path}")
        expect(registration_manifest_path.is_file(), f"packaged advanced helper smoke compile did not publish {registration_manifest_path}")

        registration_manifest = load_json(registration_manifest_path)
        expect(
            str(registration_manifest.get("runtime_support_library_archive_relative_path", "")) == normalize_rel_path(str(manifest["runtime_library"])),
            f"packaged advanced helper registration manifest drifted from the packaged runtime archive for {canonical_module}",
        )

        compile_results.append(
            {
                "canonical_module": canonical_module,
                "implementation_module": implementation_module,
                "smoke_source": repo_rel(smoke_source),
                "artifact_root": repo_rel(compile_dir),
                "object": repo_rel(object_path),
                "compile_manifest": repo_rel(compile_manifest_path),
                "registration_manifest": repo_rel(registration_manifest_path),
            }
        )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_stdlib_advanced_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "packaged_command_surfaces": {
            "stdlib_advanced": command_surfaces["stdlib_advanced"],
            "stdlib_advanced_e2e": command_surfaces["stdlib_advanced_e2e"],
        },
        "advanced_helper_surface": {
            "advanced_helper_package_surface": repo_rel(advanced_helper_package_surface),
            "modules": manifest["advanced_helper_modules"],
            "command_surfaces": manifest["advanced_helper_command_surfaces"],
            "profile_gates": manifest["advanced_helper_profile_gates"],
        },
        "compile_results": compile_results,
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "exit_code": package_result.returncode,
            },
            {
                "action": "compile-packaged-advanced-helper-smoke-sources",
                "compiled_module_count": len(compile_results),
            },
        ],
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print("objc3c-runnable-stdlib-advanced: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
