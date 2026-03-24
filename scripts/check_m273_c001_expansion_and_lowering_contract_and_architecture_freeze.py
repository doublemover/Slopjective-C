#!/usr/bin/env python3
"""Checker for M273-C001 expansion/lowering contract freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m273-c001-part10-expansion-lowering-contract-v1"
CONTRACT_ID = "objc3c-part10-expansion-lowering-contract/m273-c001-v1"
SURFACE_KEY = "objc_part10_expansion_and_lowering_contract"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m273" / "M273-C001" / "expansion_lowering_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m273_expansion_and_lowering_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m273" / "m273_c001_expansion_and_lowering_contract_and_architecture_freeze_packet.md"
DOC_ARTIFACTS = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_METADATA = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
SPEC_LOWERING = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_c001_expansion_lowering_positive.objc3"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M273-C001-MISSING", f"missing artifact: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m273-c001-readiness",
        "--summary-out",
        "tmp/reports/m273/M273-C001/ensure_build_runtime_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M273-C001-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)

    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M273-C001-DYN-02", "frontend runner missing after build", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m273" / "c001" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    completed = run_command([str(args.runner_exe), str(FIXTURE), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-object"])
    output = (completed.stdout or "") + (completed.stderr or "")
    checks_total += 1
    checks_passed += require(completed.returncode == 0, display_path(FIXTURE), "M273-C001-DYN-03", f"positive fixture failed: {output}", failures)

    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M273-C001-DYN-04", "positive manifest missing", failures)
    checks_total += 1
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M273-C001-DYN-05", "positive IR missing", failures)

    packet: dict[str, object] = {}
    if manifest_path.exists():
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        packet = manifest["frontend"]["pipeline"]["semantic_surface"][SURFACE_KEY]

    expected = {
        "derive_inventory_sites": 1,
        "derived_selector_artifact_sites": 1,
        "macro_replay_visible_sites": 1,
        "property_behavior_sites": 2,
        "synthesized_binding_sites": 2,
        "synthesized_getter_sites": 2,
        "synthesized_setter_sites": 2,
        "replay_visible_metadata_sites": 10,
        "guard_blocked_sites": 0,
        "contract_violation_sites": 0,
    }
    for index, (field, expected_value) in enumerate(expected.items(), start=6):
        checks_total += 1
        checks_passed += require(packet.get(field) == expected_value, display_path(manifest_path), f"M273-C001-DYN-{index:02d}", f"packet field {field} mismatch", failures)

    bool_checks = [
        ("deterministic_handoff", True, 16),
        ("ready_for_ir_emission", True, 17),
    ]
    for field, expected_value, index in bool_checks:
        checks_total += 1
        checks_passed += require(packet.get(field) is expected_value, display_path(manifest_path), f"M273-C001-DYN-{index:02d}", f"packet field {field} mismatch", failures)

    ir_text = ir_path.read_text(encoding="utf-8") if ir_path.exists() else ""
    ir_checks = [
        ("M273-C001-DYN-18", "; part10_expansion_lowering_contract = "),
        ("M273-C001-DYN-19", "; frontend_objc_part10_expansion_lowering_profile = derive_inventory_sites=1"),
        ("M273-C001-DYN-20", "!objc3.objc_part10_expansion_and_lowering_contract = !{!104}"),
    ]
    for check_id, snippet in ir_checks:
        checks_total += 1
        checks_passed += require(snippet in ir_text, display_path(ir_path), check_id, f"IR missing snippet: {snippet}", failures)

    return checks_total, checks_passed, {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": completed.returncode,
        "positive_output": output.strip(),
        "positive_manifest": display_path(manifest_path),
        "positive_ir": display_path(ir_path),
        "part10_expansion_lowering_packet": packet,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M273-C001-EXP-01", "# M273 Expansion and Lowering Contract and Architecture Freeze Expectations (C001)"),
            SnippetCheck("M273-C001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M273-C001-PKT-01", "# M273-C001 Packet: Expansion and Lowering Contract - Contract and Architecture Freeze"),
            SnippetCheck("M273-C001-PKT-02", SURFACE_KEY),
        ],
        DOC_ARTIFACTS: [
            SnippetCheck("M273-C001-ARTDOC-01", "## M273 expansion and lowering contract"),
            SnippetCheck("M273-C001-ARTDOC-02", SURFACE_KEY),
        ],
        DOC_NATIVE: [
            SnippetCheck("M273-C001-DOC-01", "## M273 expansion and lowering contract"),
            SnippetCheck("M273-C001-DOC-02", "part10_expansion_lowering_contract"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M273-C001-ATTR-01", "## M273 expansion and lowering contract (C001)"),
            SnippetCheck("M273-C001-ATTR-02", SURFACE_KEY),
        ],
        SPEC_METADATA: [
            SnippetCheck("M273-C001-META-01", "## M273 expansion/lowering metadata note"),
            SnippetCheck("M273-C001-META-02", CONTRACT_ID),
        ],
        SPEC_LOWERING: [
            SnippetCheck("M273-C001-LOW-01", "## M273 part 10 expansion lowering contract"),
            SnippetCheck("M273-C001-LOW-02", CONTRACT_ID),
        ],
        LOWERING_HEADER: [
            SnippetCheck("M273-C001-HDR-01", "kObjc3Part10ExpansionLoweringContractId"),
            SnippetCheck("M273-C001-HDR-02", "struct Objc3Part10ExpansionLoweringContract"),
        ],
        LOWERING_CPP: [
            SnippetCheck("M273-C001-CPP-01", "IsValidObjc3Part10ExpansionLoweringContract"),
            SnippetCheck("M273-C001-CPP-02", "Objc3Part10ExpansionLoweringReplayKey"),
        ],
        IR_HEADER: [
            SnippetCheck("M273-C001-IRH-01", "lowering_part10_expansion_replay_key"),
        ],
        IR_CPP: [
            SnippetCheck("M273-C001-IRC-01", "part10_expansion_lowering_contract = "),
            SnippetCheck("M273-C001-IRC-02", "!objc3.objc_part10_expansion_and_lowering_contract = !{!104}"),
        ],
        ARTIFACTS_CPP: [
            SnippetCheck("M273-C001-ART-01", "BuildPart10ExpansionLoweringContract"),
            SnippetCheck("M273-C001-ART-02", "BuildPart10ExpansionLoweringContractJson"),
            SnippetCheck("M273-C001-ART-03", SURFACE_KEY),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M273-C001-PKG-01", '"check:objc3c:m273-c001-expansion-and-lowering-contract-and-architecture-freeze"'),
            SnippetCheck("M273-C001-PKG-02", '"check:objc3c:m273-c001-lane-c-readiness"'),
        ],
    }

    for path, path_snippets in snippets.items():
        checks_total += len(path_snippets)
        checks_passed += ensure_snippets(path, path_snippets, failures)

    dynamic_summary: dict[str, object] = {"skipped": True}
    dynamic_executed = False
    if not args.skip_dynamic_probes:
        dynamic_executed = True
        dyn_total, dyn_passed, dynamic_summary = run_dynamic_probes(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "surface_key": SURFACE_KEY,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_executed": dynamic_executed,
        "dynamic_summary": dynamic_summary,
        "failures": [failure.__dict__ for failure in failures],
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for failure in failures:
            print(f"[fail] {failure.check_id} {failure.artifact}: {failure.detail}", file=sys.stderr)
        return 1

    print(f"[ok] M273-C001 expansion/lowering checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
