#!/usr/bin/env python3
"""Checker for M314-B005 prototype compiler surface retirement."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m314" / "M314-B005" / "prototype_compiler_surface_retirement_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m314_prototype_compiler_surface_retirement_edge_case_and_compatibility_completion_b005_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_b005_prototype_compiler_surface_retirement_edge_case_and_compatibility_completion_packet.md"
REGISTRY_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_b005_prototype_compiler_surface_retirement_edge_case_and_compatibility_completion_registry.json"
PACKAGE_JSON = ROOT / "package.json"
README = ROOT / "README.md"


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def git_ls_files(relative_root: str) -> list[str]:
    result = subprocess.run(
        ["git", "ls-files", relative_root],
        cwd=ROOT,
        check=True,
        capture_output=True,
        text=True,
    )
    return [line.strip().replace("\\", "/") for line in result.stdout.splitlines() if line.strip()]


def repo_reference_hits() -> list[str]:
    result = subprocess.run(
        [
            "rg",
            "-l",
            "--hidden",
            "--glob",
            "!tmp/**",
            "--glob",
            "!.git/**",
            "compiler/objc3c/semantic.py|compiler/objc3c",
            ".",
        ],
        cwd=ROOT,
        check=True,
        capture_output=True,
        text=True,
    )
    hits: list[str] = []
    for raw in result.stdout.splitlines():
        path = raw.strip().removeprefix(".\\").replace("\\", "/")
        if path:
            hits.append(path)
    return sorted(set(hits))


def is_allowed_reference(path: str, allowed_prefixes: Sequence[str]) -> bool:
    return any(path == prefix or path.startswith(prefix) for prefix in allowed_prefixes)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    registry = json.loads(read_text(REGISTRY_JSON))
    package_text = read_text(PACKAGE_JSON)
    package = json.loads(package_text)
    compatibility = package.get("objc3cCommandCompatibility", {})
    prototype_surface = compatibility.get("prototypeSurface", {})
    readme = read_text(README)
    archive_path = ROOT / registry["retired_surface"]["archive_path"]
    archive_readme = ROOT / registry["retired_surface"]["archive_readme"]
    archive_text = read_text(archive_path)
    archive_readme_text = read_text(archive_readme)
    tracked_compiler_files = [
        path for path in git_ls_files("compiler") if not path.endswith("__pycache__") and not path.endswith(".pyc")
    ]
    reference_hits = repo_reference_hits()
    allowed_prefixes = registry["historical_reference_policy"]["allowed_reference_prefixes"]
    disallowed_hits = [path for path in reference_hits if not is_allowed_reference(path, allowed_prefixes)]

    checks_total += 5
    checks_passed += require(
        "Contract ID: `objc3c-cleanup-prototype-compiler-surface-retirement/m314-b005-v1`" in expectations,
        str(EXPECTATIONS_DOC),
        "M314-B005-EXP-01",
        "expectations contract id missing",
        failures,
    )
    checks_passed += require(
        "No tracked non-`__pycache__` source files remain under `compiler/`." in expectations,
        str(EXPECTATIONS_DOC),
        "M314-B005-EXP-02",
        "expectations missing compiler cleanup rule",
        failures,
    )
    checks_passed += require(
        "archived copy non-operational" in packet or "non-operational by storing it as text" in packet,
        str(PACKET_DOC),
        "M314-B005-PKT-01",
        "packet missing archive-as-text rule",
        failures,
    )
    checks_passed += require(
        "Next issue: `M314-C001`." in packet,
        str(PACKET_DOC),
        "M314-B005-PKT-02",
        "packet missing next issue",
        failures,
    )
    checks_passed += require(
        "machine-readable retirement state" in expectations,
        str(EXPECTATIONS_DOC),
        "M314-B005-EXP-03",
        "expectations missing package metadata rule",
        failures,
    )

    checks_total += 10
    checks_passed += require(registry.get("mode") == "m314-b005-prototype-compiler-surface-retirement-v1", str(REGISTRY_JSON), "M314-B005-REG-01", "mode drifted", failures)
    checks_passed += require(registry.get("contract_id") == "objc3c-cleanup-prototype-compiler-surface-retirement/m314-b005-v1", str(REGISTRY_JSON), "M314-B005-REG-02", "contract id drifted", failures)
    checks_passed += require(registry.get("depends_on") == "M314-B004", str(REGISTRY_JSON), "M314-B005-REG-03", "dependency drifted", failures)
    checks_passed += require(registry.get("retired_surface", {}).get("original_tracked_path") == "compiler/objc3c/semantic.py", str(REGISTRY_JSON), "M314-B005-REG-04", "original path drifted", failures)
    checks_passed += require(registry.get("retired_surface", {}).get("archive_path") == "spec/governance/retired_surfaces/compiler_objc3c/semantic.py.txt", str(REGISTRY_JSON), "M314-B005-REG-05", "archive path drifted", failures)
    checks_passed += require(registry.get("retired_surface", {}).get("state") == "retired-archived-text", str(REGISTRY_JSON), "M314-B005-REG-06", "retirement state drifted", failures)
    checks_passed += require(registry.get("retired_surface", {}).get("supported_compiler_root") == "native/objc3c", str(REGISTRY_JSON), "M314-B005-REG-07", "supported compiler root drifted", failures)
    checks_passed += require(registry.get("retired_surface", {}).get("tracked_source_files_under_compiler") == 0, str(REGISTRY_JSON), "M314-B005-REG-08", "tracked compiler count drifted", failures)
    checks_passed += require("README.md" in allowed_prefixes, str(REGISTRY_JSON), "M314-B005-REG-09", "allowed prefix set drifted", failures)
    checks_passed += require(registry.get("next_issue") == "M314-C001", str(REGISTRY_JSON), "M314-B005-REG-10", "next issue drifted", failures)

    checks_total += 8
    checks_passed += require(tracked_compiler_files == [], "git ls-files compiler", "M314-B005-GIT-01", "tracked compiler files remain under compiler/", failures)
    checks_passed += require(archive_path.is_file(), str(archive_path), "M314-B005-ARC-01", "archive file missing", failures)
    checks_passed += require(archive_readme.is_file(), str(archive_readme), "M314-B005-ARC-02", "archive readme missing", failures)
    checks_passed += require("Original tracked path: compiler/objc3c/semantic.py" in archive_text, str(archive_path), "M314-B005-ARC-03", "archive file missing provenance header", failures)
    checks_passed += require("Do not treat this directory as an executable compiler path." in archive_readme_text, str(archive_readme), "M314-B005-ARC-04", "archive readme missing non-operational note", failures)
    checks_passed += require("native/objc3c/" in archive_readme_text, str(archive_readme), "M314-B005-ARC-05", "archive readme missing supported root note", failures)
    checks_passed += require(not disallowed_hits, "repo reference scan", "M314-B005-REF-01", f"disallowed references found: {disallowed_hits}", failures)
    checks_passed += require("python scripts/objc3c_public_workflow_runner.py" not in archive_readme_text, str(archive_readme), "M314-B005-ARC-06", "archive readme must not advertise active runner use", failures)

    checks_total += 6
    checks_passed += require(prototype_surface.get("state") == "retired-archived-text", str(PACKAGE_JSON), "M314-B005-PKG-01", "package prototype state drifted", failures)
    checks_passed += require(prototype_surface.get("archivePath") == registry["retired_surface"]["archive_path"], str(PACKAGE_JSON), "M314-B005-PKG-02", "package archive path drifted", failures)
    checks_passed += require(prototype_surface.get("supportedCompilerRoot") == "native/objc3c", str(PACKAGE_JSON), "M314-B005-PKG-03", "package supported root drifted", failures)
    checks_passed += require("native/objc3c/" in readme, str(README), "M314-B005-RD-01", "README missing supported compiler root", failures)
    checks_passed += require("prototype Python compiler surface has been retired into the governance archive" in readme, str(README), "M314-B005-RD-02", "README missing retirement note", failures)
    checks_passed += require('"check:objc3c:m314-b005-prototype-compiler-surface-retirement-edge-case-and-compatibility-completion"' in package_text and '"check:objc3c:m314-b005-lane-b-readiness"' in package_text, str(PACKAGE_JSON), "M314-B005-PKG-04", "package missing issue-local scripts", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": registry["mode"],
        "contract_id": registry["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "tracked_compiler_files": tracked_compiler_files,
        "archive_path": registry["retired_surface"]["archive_path"],
        "reference_hits": reference_hits,
        "disallowed_reference_hits": disallowed_hits,
        "next_issue": "M314-C001",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M314-B005 prototype surface retirement checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
