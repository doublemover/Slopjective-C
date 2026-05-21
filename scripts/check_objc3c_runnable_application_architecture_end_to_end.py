#!/usr/bin/env python3
"""Validate application architecture surfaces from the staged runnable toolchain bundle."""

from __future__ import annotations

import shutil
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.subprocesses import run_capture


ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
REPORT_PATH = ROOT / "tmp" / "reports" / "application-architecture-testing" / "package-integration-summary.json"
EXPECTED_APPLICATION_FRAMEWORK_SAMPLES: dict[str, Any] = {
    "manifest": "showcase/applicationFrameworkSamples/manifest.json",
    "readme": "showcase/applicationFrameworkSamples/README.md",
    "tutorial": "docs/tutorials/application-framework-samples.md",
    "dependency_evidence": "showcase/applicationFrameworkSamples/dependency-evidence.json",
    "contract_fixture": "tests/tooling/fixtures/application_framework_samples/contract.json",
    "checker": "scripts/check_objc3c_application_framework_samples.py",
    "summary": "tmp/reports/application-framework-samples/summary.json",
    "validate_action": "validate-application-framework-samples",
    "validate_command": "npm run objc3c -- validate-application-framework-samples",
    "package_action": "package-runnable-toolchain",
    "package_command": "npm run objc3c -- package-runnable-toolchain",
    "runnable_validation_action": "validate-runnable-application-architecture",
    "runnable_validation_command": "npm run objc3c -- validate-runnable-application-architecture",
    "manifest_contract_id": "objc3c.application_framework_samples.v1",
    "contract_fixture_id": "objc3c.application_framework_samples.contract.v1",
    "package_manifest_fields": [
        "application_framework_samples",
        "application_architecture_public_actions",
        "command_surfaces",
        "copied_files",
    ],
    "sample_ids": [
        "routeModelKit",
        "interopAdapterKit",
        "workflowStdlibCLI",
        "asyncRuntimeConsole",
    ],
    "capability_rows": [
        "applications.framework-samples.object-runtime-library",
        "applications.framework-samples.interop-adapter-library",
        "applications.framework-samples.stdlib-text-collections-cli",
        "applications.framework-samples.async-runtime-application",
    ],
}


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def _package_file(package_root: Path, relative_path: object) -> Path:
    return package_root / str(relative_path)


def _expect_packaged_file(
    *,
    package_root: Path,
    relative_path: object,
    message: str,
    failures: list[str],
) -> None:
    expect(_package_file(package_root, relative_path).is_file(), message, failures)


def validate_application_framework_sample_package_metadata(
    *,
    manifest: dict[str, Any],
    package_root: Path,
    failures: list[str],
    run_packaged_checker: bool = True,
) -> dict[str, Any] | None:
    app_samples = manifest.get("application_framework_samples")
    expect(
        isinstance(app_samples, dict),
        "package manifest missing application_framework_samples",
        failures,
    )
    if not isinstance(app_samples, dict):
        return None

    for key, expected in EXPECTED_APPLICATION_FRAMEWORK_SAMPLES.items():
        expect(
            app_samples.get(key) == expected,
            f"package manifest application_framework_samples {key} drifted",
            failures,
        )

    copied_files = manifest.get("copied_files")
    expect(isinstance(copied_files, list), "package manifest missing copied_files", failures)
    copied_file_set = set(copied_files) if isinstance(copied_files, list) else set()
    required_copied_paths = [
        app_samples["manifest"],
        app_samples["readme"],
        app_samples["tutorial"],
        app_samples["dependency_evidence"],
        app_samples["contract_fixture"],
        app_samples["checker"],
    ]
    for relative_path in required_copied_paths:
        _expect_packaged_file(
            package_root=package_root,
            relative_path=relative_path,
            message=f"packaged application framework sample file missing: {relative_path}",
            failures=failures,
        )
        expect(
            relative_path in copied_file_set,
            (
                "package manifest copied_files missing application framework "
                f"sample file: {relative_path}"
            ),
            failures,
        )

    sample_manifest_path = _package_file(package_root, app_samples["manifest"])
    if sample_manifest_path.is_file():
        sample_manifest = load_json(sample_manifest_path)
        expect(
            sample_manifest.get("contract_id") == app_samples["manifest_contract_id"],
            "packaged application framework sample manifest contract id drifted",
            failures,
        )
        expect(
            sample_manifest.get("dependency_evidence")
            == app_samples["dependency_evidence"],
            "packaged application framework sample dependency evidence drifted",
            failures,
        )
        samples = sample_manifest.get("samples")
        expect(
            isinstance(samples, list),
            "packaged application framework sample manifest missing samples",
            failures,
        )
        if isinstance(samples, list):
            expect(
                [sample.get("id") for sample in samples if isinstance(sample, dict)]
                == app_samples["sample_ids"],
                "packaged application framework sample ids drifted",
                failures,
            )
            for sample in samples:
                expect(
                    isinstance(sample, dict),
                    "packaged application framework sample entry malformed",
                    failures,
                )
                if not isinstance(sample, dict):
                    continue
                for key in ("source", "workspace_manifest", "replay_contract", "tutorial"):
                    relative_path = sample.get(key)
                    _expect_packaged_file(
                        package_root=package_root,
                        relative_path=relative_path,
                        message=(
                            "packaged application framework sample "
                            f"{sample.get('id')} missing {key}"
                        ),
                        failures=failures,
                    )
                    expect(
                        relative_path in copied_file_set,
                        (
                            "package manifest copied_files missing application framework "
                            f"sample {key}: {relative_path}"
                        ),
                        failures,
                    )

    contract_path = _package_file(package_root, app_samples["contract_fixture"])
    if contract_path.is_file():
        contract = load_json(contract_path)
        expect(
            contract.get("contract_id") == app_samples["contract_fixture_id"],
            "packaged application framework sample contract fixture id drifted",
            failures,
        )
        expect(
            contract.get("manifest") == app_samples["manifest"],
            "packaged application framework sample contract manifest drifted",
            failures,
        )

    checker_path = _package_file(package_root, app_samples["checker"])
    if run_packaged_checker and checker_path.is_file():
        checker_result = run_capture(
            [sys.executable, str(checker_path), "--validate-only"],
            cwd=package_root,
        )
        expect(
            checker_result.returncode == 0,
            "packaged application framework sample checker validate-only failed",
            failures,
        )

    return app_samples


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-application-architecture-e2e" / run_id
    manifest_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"

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
    app_surface = manifest.get("application_architecture_surface", {})
    command_surfaces = manifest.get("command_surfaces", {})
    public_actions = manifest.get("application_architecture_public_actions", [])
    package_bridge = manifest.get("package_bridge")
    failures: list[str] = []

    expect(isinstance(app_surface, dict), "package manifest missing application_architecture_surface", failures)
    expect(
        "materialize-canonical-application-workspace" in public_actions,
        "package manifest missing application architecture public action",
        failures,
    )
    expect(
        "validate-runnable-application-architecture" in public_actions,
        "package manifest missing runnable application architecture public action",
        failures,
    )
    expect(
        "validate-application-framework-samples" in public_actions,
        "package manifest missing application framework samples public action",
        failures,
    )
    expect(
        package_bridge == "objc3c",
        "package manifest missing objc3c package bridge",
        failures,
    )
    expect(
        command_surfaces.get("build_application_workspace") == "npm run objc3c -- materialize-canonical-application-workspace",
        "package manifest missing build_application_workspace command surface",
        failures,
    )
    expect(
        command_surfaces.get("application_architecture") == "npm run objc3c -- validate-application-architecture",
        "package manifest missing application_architecture command surface",
        failures,
    )
    expect(
        command_surfaces.get("application_architecture_e2e") == "npm run objc3c -- validate-runnable-application-architecture",
        "package manifest missing application_architecture_e2e command surface",
        failures,
    )
    expect(
        command_surfaces.get("application_framework_samples") == "npm run objc3c -- validate-application-framework-samples",
        "package manifest missing application_framework_samples command surface",
        failures,
    )

    packaged_materializer = package_root / str(app_surface.get("canonical_workspace_materializer", ""))
    packaged_template_checker = package_root / str(app_surface.get("template_harness_checker", ""))
    packaged_e2e_checker = package_root / str(app_surface.get("runnable_end_to_end_validation", ""))
    packaged_framework_checker = package_root / str(app_surface.get("application_framework_samples_validation", ""))
    expect(packaged_materializer.is_file(), "packaged canonical workspace materializer missing", failures)
    expect(packaged_template_checker.is_file(), "packaged template harness checker missing", failures)
    expect(packaged_e2e_checker.is_file(), "packaged runnable application architecture validator missing", failures)
    expect(packaged_framework_checker.is_file(), "packaged application framework sample validator missing", failures)

    application_framework_samples = validate_application_framework_sample_package_metadata(
        manifest=manifest,
        package_root=package_root,
        failures=failures,
    )

    packaged_materialize_result = run_capture(
        [sys.executable, str(packaged_materializer)],
        cwd=package_root,
    )
    expect(packaged_materialize_result.returncode == 0, "packaged canonical workspace materializer failed", failures)

    packaged_template_result = run_capture(
        [sys.executable, str(packaged_template_checker)],
        cwd=package_root,
    )
    expect(packaged_template_result.returncode == 0, "packaged template harness checker failed", failures)

    canonical_summary = load_json(
        package_root / "tmp" / "reports" / "application-architecture-testing" / "canonical-application-workspace-summary.json"
    )
    template_summary = load_json(
        package_root / "tmp" / "reports" / "application-architecture-testing" / "template-harness-summary.json"
    )
    expect(canonical_summary.get("status") == "PASS", "packaged canonical workspace summary did not report PASS", failures)
    expect(canonical_summary.get("example_count") == 3, "packaged canonical workspace example count drifted", failures)
    expect(template_summary.get("status") == "PASS", "packaged template harness summary did not report PASS", failures)

    payload = {
        "contract_id": "objc3c.application.architecture.testing.runnable.end.to.end.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "package_root": repo_rel(package_root),
        "manifest_path": repo_rel(manifest_path),
        "application_architecture_surface": app_surface,
        "application_architecture_public_actions": public_actions,
        "application_framework_samples": application_framework_samples,
        "package_bridge": "objc3c",
        "packaged_package_bridge": package_bridge,
        "failures": failures,
        "packaged_reports": {
            "canonical_workspace_summary": repo_rel(
                package_root / "tmp" / "reports" / "application-architecture-testing" / "canonical-application-workspace-summary.json"
            ),
            "template_harness_summary": repo_rel(
                package_root / "tmp" / "reports" / "application-architecture-testing" / "template-harness-summary.json"
            ),
        },
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    if failures:
        print("runnable-application-architecture: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("runnable-application-architecture: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
