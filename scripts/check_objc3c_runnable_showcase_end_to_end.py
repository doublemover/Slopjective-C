#!/usr/bin/env python3
"""Validate showcase examples end to end from the staged runnable toolchain bundle."""

from __future__ import annotations

import json
import os
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
REPORT_PATH = ROOT / "tmp" / "reports" / "showcase" / "runnable-end-to-end-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.showcase.runnable.end.to.end.summary.v1"
PACKAGE_CONTRACT_ID = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


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


def extract_output_value(stdout: str, key: str) -> str | None:
    prefix = f"{key}:"
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith(prefix):
            return line.split(":", 1)[1].strip()
    return None


def extract_report_paths(stdout: str) -> list[str]:
    report_paths: list[str] = []
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith("summary_path:"):
            report_paths.append(line.split(":", 1)[1].strip().replace("\\", "/"))
            continue
        if line.startswith("public-workflow-report:"):
            report_paths.append(line.split(":", 1)[1].strip().replace("\\", "/"))
    return report_paths


def load_json(path: Path) -> dict[str, Any]:
    expect(path.is_file(), f"expected JSON artifact was not published: {path}")
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {path} did not contain an object")
    return payload


def find_clangxx() -> str:
    llvm_root = os.environ.get("LLVM_ROOT")
    if llvm_root:
        candidate = Path(llvm_root) / "bin" / "clang++.exe"
        if candidate.is_file():
            return str(candidate)
    candidate = shutil.which("clang++")
    if candidate:
        return candidate
    raise RuntimeError("clang++ not found; set LLVM_ROOT or ensure clang++ is on PATH")


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-showcase-e2e" / run_id
    manifest_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    artifacts_root = package_root / "tmp" / "artifacts" / "showcase-e2e"

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
    expect(
        manifest.get("contract_id") == PACKAGE_CONTRACT_ID,
        "runnable toolchain package manifest published the wrong contract id",
    )

    compile_wrapper = package_root / normalize_rel_path(str(manifest["compile_wrapper"]))
    runtime_library = package_root / normalize_rel_path(str(manifest["runtime_library"]))
    showcase_portfolio = package_root / normalize_rel_path(str(manifest["showcase_portfolio"]))
    showcase_readme = package_root / normalize_rel_path(str(manifest["showcase_readme"]))
    guided_walkthrough_manifest = package_root / normalize_rel_path(str(manifest["guided_walkthrough_manifest"]))
    repo_superclean_surface = package_root / normalize_rel_path(str(manifest["repo_superclean_surface"]))
    capability_probe_script = package_root / normalize_rel_path(str(manifest["capability_probe_script"]))
    tutorial_guides = manifest.get("tutorial_guides")
    expect(showcase_portfolio.is_file(), "packaged runnable toolchain missing showcase portfolio")
    expect(showcase_readme.is_file(), "packaged runnable toolchain missing showcase README")
    expect(guided_walkthrough_manifest.is_file(), "packaged runnable toolchain missing guided walkthrough manifest")
    expect(repo_superclean_surface.is_file(), "packaged runnable toolchain missing repo superclean surface contract")
    expect(capability_probe_script.is_file(), "packaged runnable toolchain missing capability probe script")
    expect(isinstance(tutorial_guides, list) and tutorial_guides, "package manifest did not publish tutorial guides")
    for tutorial_path in tutorial_guides:
        expect(isinstance(tutorial_path, str) and tutorial_path, "package manifest published a malformed tutorial guide path")
        expect((package_root / normalize_rel_path(tutorial_path)).is_file(), f"packaged runnable toolchain missing tutorial guide {tutorial_path}")

    showcase_examples = manifest.get("showcase_examples")
    expect(isinstance(showcase_examples, list) and showcase_examples, "package manifest did not publish showcase examples")
    bonus_experience_surfaces = manifest.get("bonus_experience_surfaces")
    expect(isinstance(bonus_experience_surfaces, dict), "package manifest did not publish bonus experience surface metadata")
    expect(
        bonus_experience_surfaces.get("playground", {}).get("public_actions") == [
            "materialize-playground-workspace",
            "compile-objc3c",
            "inspect-playground-repro",
            "inspect-compile-observability",
            "trace-compile-stages",
        ],
        "package manifest playground action surface drifted",
    )
    expect(
        "scripts/probe_objc3c_llvm_capabilities.py"
        in bonus_experience_surfaces.get("runtime_inspector", {}).get("source_roots", []),
        "package manifest runtime inspector surface did not publish the capability probe source root",
    )
    expect(
        "docs/tutorials/getting_started.md"
        in bonus_experience_surfaces.get("template_and_demo_harness", {}).get("source_roots", []),
        "package manifest template/demo surface did not publish getting_started.md",
    )
    command_surfaces = manifest.get("command_surfaces", {})
    expect(command_surfaces.get("build_playground") == "npm run build:objc3c:playground", "package manifest missing build_playground command surface")
    expect(command_surfaces.get("inspect_playground") == "npm run inspect:objc3c:playground", "package manifest missing inspect_playground command surface")
    expect(command_surfaces.get("inspect_benchmark") == "npm run inspect:objc3c:benchmark", "package manifest missing inspect_benchmark command surface")
    expect(command_surfaces.get("inspect_capabilities") == "npm run inspect:objc3c:capabilities", "package manifest missing inspect_capabilities command surface")
    expect(command_surfaces.get("inspect_runtime") == "npm run inspect:objc3c:runtime", "package manifest missing inspect_runtime command surface")
    expect(command_surfaces.get("trace_stages") == "npm run trace:objc3c:stages", "package manifest missing trace_stages command surface")
    expect(command_surfaces.get("developer_tooling") == "npm run test:objc3c:developer-tooling", "package manifest missing developer_tooling command surface")
    expect(command_surfaces.get("getting_started") == "npm run test:getting-started", "package manifest missing getting_started command surface")
    portfolio_payload = load_json(showcase_portfolio)
    expect(
        portfolio_payload.get("contract_id") == "objc3c.showcase.portfolio.surface.v1",
        "packaged showcase portfolio published the wrong contract id",
    )
    expect(
        [entry.get("id") for entry in portfolio_payload.get("examples", []) if isinstance(entry, dict)]
        == [entry.get("example_id") for entry in showcase_examples if isinstance(entry, dict)],
        "packaged showcase portfolio drifted from the manifest example inventory",
    )

    clangxx = find_clangxx()
    example_results: list[dict[str, Any]] = []

    for example in showcase_examples:
        expect(isinstance(example, dict), "package manifest published a malformed showcase example entry")
        example_id = str(example["example_id"])
        source_path = package_root / normalize_rel_path(str(example["source"]))
        workspace_manifest = package_root / normalize_rel_path(str(example["workspace_manifest"]))
        expect(source_path.is_file(), f"packaged runnable toolchain missing showcase source for {example_id}")
        expect(workspace_manifest.is_file(), f"packaged runnable toolchain missing workspace manifest for {example_id}")

        workspace_payload = load_json(workspace_manifest)
        expected_exit = int(example["expected_exit_code"])
        expect(
            int(workspace_payload["runtime_surface"]["expected_exit_code"]) == expected_exit,
            f"workspace runtime surface drifted for {example_id}",
        )

        compile_dir = artifacts_root / example_id / "compile"
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

        object_path = compile_dir / "module.obj"
        registration_manifest_path = compile_dir / "module.runtime-registration-manifest.json"
        compile_manifest_path = compile_dir / "module.manifest.json"
        expect(object_path.is_file(), f"packaged showcase compile did not publish {object_path}")
        expect(registration_manifest_path.is_file(), f"packaged showcase compile did not publish {registration_manifest_path}")
        expect(compile_manifest_path.is_file(), f"packaged showcase compile did not publish {compile_manifest_path}")

        registration_manifest = load_json(registration_manifest_path)
        expected_runtime_library = str(registration_manifest.get("runtime_support_library_archive_relative_path", ""))
        expect(
            expected_runtime_library == normalize_rel_path(str(manifest["runtime_library"])),
            f"packaged showcase registration manifest drifted from the packaged runtime archive for {example_id}",
        )

        runtime_dir = artifacts_root / example_id / "runtime"
        runtime_dir.mkdir(parents=True, exist_ok=True)
        exe_path = runtime_dir / "module.exe"
        link_log = runtime_dir / "link.log"
        run_log = runtime_dir / "run.log"
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

        example_results.append(
            {
                "example_id": example_id,
                "source": repo_rel(source_path),
                "workspace_manifest": repo_rel(workspace_manifest),
                "compile_dir": repo_rel(compile_dir),
                "executable": repo_rel(exe_path),
                "link_log": repo_rel(link_log),
                "run_log": repo_rel(run_log),
                "expected_exit_code": expected_exit,
                "actual_exit_code": run_result.returncode,
            }
        )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_showcase_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "showcase_portfolio": repo_rel(showcase_portfolio),
        "showcase_readme": repo_rel(showcase_readme),
        "examples": example_results,
        "child_report_paths": extract_report_paths(package_result.stdout),
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "exit_code": package_result.returncode,
                "package_root": extract_output_value(package_result.stdout, "package_root"),
                "manifest": extract_output_value(package_result.stdout, "manifest"),
            },
            {
                "action": "compile-link-run-packaged-showcase",
                "exit_code": 0,
                "clangxx": clangxx,
            },
        ],
    }

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
