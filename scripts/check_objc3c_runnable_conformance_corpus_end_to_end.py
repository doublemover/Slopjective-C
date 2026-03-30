#!/usr/bin/env python3
"""Validate the staged runnable conformance corpus surface end to end from the package root."""

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
REPORT_PATH = ROOT / "tmp" / "reports" / "conformance" / "corpus-runnable-end-to-end-summary.json"
PACKAGE_CONTRACT_ID = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"
SUMMARY_CONTRACT_ID = "objc3c.conformance.corpus.runnable.end.to.end.summary.v1"


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
    package_root = ROOT / "tmp" / "pkg" / "objc3c-conformance-corpus-e2e" / run_id
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
    expect(manifest.get("contract_id") == PACKAGE_CONTRACT_ID, "unexpected package contract id")

    command_surfaces = manifest.get("command_surfaces", {})
    conformance_surface = manifest.get("conformance_corpus_surface", {})
    expect(isinstance(conformance_surface, dict), "package manifest missing conformance_corpus_surface")
    expect(
        command_surfaces.get("conformance_corpus") == "npm run test:objc3c:conformance-corpus",
        "package manifest missing conformance_corpus command surface",
    )
    expect(
        command_surfaces.get("conformance_corpus_e2e") == "npm run test:objc3c:runnable-conformance-corpus",
        "package manifest missing conformance_corpus_e2e command surface",
    )

    suite_readme = package_root / normalize_rel_path(str(manifest["conformance_suite_readme"]))
    coverage_map = package_root / normalize_rel_path(str(manifest["conformance_coverage_map"]))
    runbook = package_root / normalize_rel_path(str(manifest["conformance_runbook"]))
    surface_check_script = package_root / normalize_rel_path(str(manifest["conformance_surface_check_script"]))
    coverage_index_script = package_root / normalize_rel_path(str(manifest["conformance_coverage_index_script"]))
    suite_gate_script = package_root / normalize_rel_path(str(manifest["conformance_legacy_suite_gate_script"]))
    longitudinal_manifest = package_root / normalize_rel_path(str(manifest["conformance_longitudinal_manifest"]))
    corpus_contract = package_root / "tests" / "conformance" / "corpus_surface.json"

    for path in (
        suite_readme,
        coverage_map,
        runbook,
        surface_check_script,
        coverage_index_script,
        suite_gate_script,
        longitudinal_manifest,
        corpus_contract,
    ):
        expect(path.is_file(), f"packaged runnable toolchain missing required conformance file {path}")

    surface_check_result = run_capture([sys.executable, str(surface_check_script)], cwd=package_root)
    if surface_check_result.returncode != 0:
        raise RuntimeError("packaged conformance corpus surface check failed")

    coverage_index_result = run_capture([sys.executable, str(coverage_index_script)], cwd=package_root)
    if coverage_index_result.returncode != 0:
        raise RuntimeError("packaged conformance corpus index generation failed")

    packaged_corpus_contract = load_json(corpus_contract)
    packaged_longitudinal_manifest = load_json(longitudinal_manifest)
    artifact_surface = packaged_corpus_contract.get("artifact_surface", {})
    expect(isinstance(artifact_surface, dict), "packaged corpus contract missing artifact_surface")
    surface_summary_path = package_root / normalize_rel_path(str(artifact_surface["surface_summary"]))
    coverage_index_path = package_root / normalize_rel_path(str(artifact_surface["coverage_index"]))
    packaged_surface_summary = load_json(surface_summary_path)
    packaged_coverage_index = load_json(coverage_index_path)

    expect(
        packaged_surface_summary.get("contract_id") == "objc3c.conformance.corpus.surface.summary.v1",
        "packaged conformance surface summary contract drifted",
    )
    expect(
        packaged_coverage_index.get("contract_id") == "objc3c.conformance.corpus.index.v1",
        "packaged conformance index contract drifted",
    )
    expect(
        conformance_surface.get("corpus_contract") == "tests/conformance/corpus_surface.json",
        "package manifest conformance_corpus_surface drifted from the packaged corpus contract path",
    )
    expect(
        conformance_surface.get("suite_readme") == packaged_corpus_contract.get("suite_readme"),
        "package manifest conformance suite README drifted from the packaged corpus contract",
    )
    expect(
        conformance_surface.get("coverage_map") == packaged_corpus_contract.get("coverage_map"),
        "package manifest conformance coverage map drifted from the packaged corpus contract",
    )
    expect(
        conformance_surface.get("runbook") == packaged_corpus_contract.get("runbook"),
        "package manifest conformance runbook drifted from the packaged corpus contract",
    )
    expect(
        conformance_surface.get("longitudinal_manifest")
        == packaged_corpus_contract.get("longitudinal_policy", {}).get("suite_manifest"),
        "package manifest conformance longitudinal manifest drifted from the packaged corpus contract",
    )
    expect(
        conformance_surface.get("workflow_surface") == packaged_corpus_contract.get("workflow_surface"),
        "package manifest conformance workflow surface drifted from the packaged corpus contract",
    )
    retained_partition = packaged_coverage_index.get("retained_partition")
    retained_suites = packaged_longitudinal_manifest.get("retained_suites")
    expect(
        isinstance(retained_partition, list)
        and isinstance(retained_suites, list)
        and len(retained_partition) == len(retained_suites),
        "packaged conformance retained partition drifted from the longitudinal suite manifest",
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_conformance_corpus_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "packaged_command_surfaces": {
            "conformance_corpus": command_surfaces["conformance_corpus"],
            "conformance_corpus_e2e": command_surfaces["conformance_corpus_e2e"],
        },
        "packaged_conformance_surface": {
            "suite_readme": repo_rel(suite_readme),
            "coverage_map": repo_rel(coverage_map),
            "runbook": repo_rel(runbook),
            "surface_check_script": repo_rel(surface_check_script),
            "coverage_index_script": repo_rel(coverage_index_script),
            "legacy_suite_gate_script": repo_rel(suite_gate_script),
            "longitudinal_manifest": repo_rel(longitudinal_manifest),
            "corpus_contract": repo_rel(corpus_contract),
        },
        "child_report_paths": [
            repo_rel(surface_summary_path),
            repo_rel(coverage_index_path),
        ],
        "retained_suite_count": len(retained_partition),
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "exit_code": package_result.returncode,
            },
            {
                "action": "check-conformance-corpus-surface",
                "exit_code": surface_check_result.returncode,
            },
            {
                "action": "generate-conformance-corpus-index",
                "exit_code": coverage_index_result.returncode,
            },
        ],
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print("objc3c-runnable-conformance-corpus: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
