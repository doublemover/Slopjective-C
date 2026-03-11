#!/usr/bin/env python3
"""Fail-closed validator for M226-D002 frontend build/invocation scaffold."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-frontend-build-invocation-modular-scaffold-contract-d002-v1"

ARTIFACTS: dict[str, Path] = {
    "build_script": ROOT / "scripts" / "build_objc3c_native.ps1",
    "compile_wrapper": ROOT / "scripts" / "objc3c_native_compile.ps1",
    "contract_doc": ROOT / "docs" / "contracts" / "m226_frontend_build_invocation_modular_scaffold_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "build_script": (
        ("M226-D002-BLD-01", "function Get-FrontendSharedSourcesFromModules"),
        ("M226-D002-BLD-02", "function Write-FrontendModuleScaffoldArtifact"),
        ("M226-D002-BLD-03", "contract_id = \"objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1\""),
        ("M226-D002-BLD-04", "tmp/artifacts/objc3c-native/frontend_modular_scaffold.json"),
        ("M226-D002-BLD-05", "Write-Output (\"frontend_scaffold=\""),
    ),
    "compile_wrapper": (
        ("M226-D002-CMP-01", "function Assert-FrontendModuleScaffold"),
        ("M226-D002-CMP-02", "$expectedContractId = \"objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1\""),
        ("M226-D002-CMP-03", "$requiredModules = @(\"driver\", \"diagnostics-io\", \"ir\", \"lex-parse\", \"frontend-api\", \"lowering\", \"pipeline\", \"sema\")"),
        ("M226-D002-CMP-04", "Assert-FrontendModuleScaffold -RepoRoot $repoRoot -BuildResult $buildResult"),
    ),
    "contract_doc": (
        ("M226-D002-DOC-01", "Contract ID: `objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1`"),
        ("M226-D002-DOC-02", "`tmp/artifacts/objc3c-native/frontend_modular_scaffold.json`"),
        ("M226-D002-DOC-03", "`driver`"),
        ("M226-D002-DOC-04", "`sema`"),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/reports/m226/M226-D002/frontend_build_invocation_modular_scaffold_summary.json"),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact=artifact)
        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

