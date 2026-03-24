#!/usr/bin/env python3
"""Checker for M274-C002 foreign call and lifetime lowering core feature implementation."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m274-c002-part11-foreign-call-and-lifetime-lowering-v1"
CONTRACT_ID = "objc3c-part11-foreign-call-and-lifetime-lowering/m274-c002-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part11_foreign_call_and_lifetime_lowering"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m274" / "M274-C002" / "foreign_call_and_lifetime_lowering_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m274_foreign_call_and_lifetime_lowering_core_feature_implementation_c002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m274" / "m274_c002_foreign_call_and_lifetime_lowering_core_feature_implementation_packet.md"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_c002_foreign_call_and_lifetime_lowering_positive.objc3"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2, sort_keys=True) + "\n"


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
        failures.append(Finding(display_path(path), "STATIC-MISSING", f"missing artifact: {display_path(path)}"))
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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def semantic_surface(manifest: dict[str, Any]) -> dict[str, Any]:
    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    if not isinstance(surface, dict):
        raise TypeError("manifest missing frontend.pipeline.semantic_surface")
    return surface


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M274-C002-EXP-01", "# M274 Foreign Call And Lifetime Lowering Core Feature Implementation Expectations (C002)"),
        SnippetCheck("M274-C002-EXP-02", "Issue: `#7368`"),
        SnippetCheck("M274-C002-EXP-03", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M274-C002-EXP-04", SURFACE_PATH),
        SnippetCheck("M274-C002-EXP-05", "foreign calls lower into deterministic IR and object-artifact facts"),
        SnippetCheck("M274-C002-EXP-06", "lifetime and ownership bridges are preserved as explicit manifest facts"),
        SnippetCheck("M274-C002-EXP-07", "objc3c-part11-interop-lowering-and-abi-contract/m274-c001-v1"),
        SnippetCheck("M274-C002-EXP-08", "objc3c-part11-cpp-ownership-throws-and-async-interaction-completion/m274-b003-v1"),
    ),
    PACKET_DOC: (
        SnippetCheck("M274-C002-PKT-01", "# M274-C002 Packet: Foreign Call And Lifetime Lowering - Core Feature Implementation"),
        SnippetCheck("M274-C002-PKT-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M274-C002-PKT-03", SURFACE_PATH),
        SnippetCheck("M274-C002-PKT-04", "foreign-call lowering"),
        SnippetCheck("M274-C002-PKT-05", "lifetime and ownership bridges"),
        SnippetCheck("M274-C002-PKT-06", "objc3c-part11-interop-lowering-and-abi-contract/m274-c001-v1"),
        SnippetCheck("M274-C002-PKT-07", "objc3c-part11-cpp-ownership-throws-and-async-interaction-completion/m274-b003-v1"),
    ),
    FIXTURE: (
        SnippetCheck("M274-C002-FIX-01", "module Part11ForeignCallAndLifetimeLoweringPositive;"),
        SnippetCheck("M274-C002-FIX-02", "objc_foreign"),
        SnippetCheck("M274-C002-FIX-03", "objc_import_module(named(\"ForeignKit\"))"),
        SnippetCheck("M274-C002-FIX-04", "objc_cxx_name(named(\"CppLifetimeBridge\"))"),
        SnippetCheck("M274-C002-FIX-05", "objc_header_name(named(\"LifetimeBridge.hpp\"))"),
        SnippetCheck("M274-C002-FIX-06", "objc_swift_name(named(\"SwiftLifetimeBridge\"))"),
        SnippetCheck("M274-C002-FIX-07", "objc_swift_private"),
        SnippetCheck("M274-C002-FIX-08", "@protocol ForeignCallLifetimeProtocol"),
        SnippetCheck("M274-C002-FIX-09", "@interface ForeignCallLifetimeBridge"),
        SnippetCheck("M274-C002-FIX-10", "@implementation ForeignCallLifetimeBridge"),
        SnippetCheck("M274-C002-FIX-11", "@property (nonatomic, strong) id currentValue;"),
        SnippetCheck("M274-C002-FIX-12", "@property (nonatomic, weak) id observer;"),
    ),
}


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M274-C002-DYN-01", "frontend runner missing", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m274" / "c002" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    completed = run_command([
        str(args.runner_exe),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-object",
    ])
    output = (completed.stdout or "") + (completed.stderr or "")
    checks_total += 1
    checks_passed += require(completed.returncode == 0, display_path(FIXTURE), "M274-C002-DYN-02", f"positive fixture failed: {output}", failures)

    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M274-C002-DYN-03", "positive manifest missing", failures)
    checks_total += 1
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M274-C002-DYN-04", "positive IR missing", failures)

    surface_payload: dict[str, Any] = {}
    if manifest_path.exists():
        manifest = load_json(manifest_path)
        surface = semantic_surface(manifest)
        surface_payload = surface.get("objc_part11_foreign_call_and_lifetime_lowering", {})
        checks_total += 1
        checks_passed += require(isinstance(surface_payload, dict), display_path(manifest_path), "M274-C002-DYN-05", "missing objc_part11_foreign_call_and_lifetime_lowering", failures)
        if isinstance(surface_payload, dict):
            checks_total += 1
            checks_passed += require(
                surface_payload.get("contract_id") == CONTRACT_ID,
                display_path(manifest_path),
                "M274-C002-DYN-06",
                "foreign-call/lifetime contract id mismatch",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                surface_payload.get("interop_contract_id") == "objc3c-part11-interop-lowering-and-abi-contract/m274-c001-v1",
                display_path(manifest_path),
                "M274-C002-DYN-07",
                "interop dependency contract mismatch",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                surface_payload.get("bridge_dependency_contract_id") == "objc3c-part11-cpp-ownership-throws-and-async-interaction-completion/m274-b003-v1",
                display_path(manifest_path),
                "M274-C002-DYN-08",
                "bridge dependency contract mismatch",
                failures,
            )
            expected_counts = {
                "foreign_callable_sites": 4,
                "c_foreign_callable_sites": 2,
                "objc_runtime_parity_callable_sites": 3,
                "ownership_bridge_sites": 0,
                "lifetime_bridge_sites": 0,
                "metadata_preservation_sites": 4,
                "guard_blocked_sites": 0,
                "contract_violation_sites": 0,
            }
            for index, (field, expected_value) in enumerate(expected_counts.items(), start=9):
                checks_total += 1
                checks_passed += require(
                    surface_payload.get(field) == expected_value,
                    display_path(manifest_path),
                    f"M274-C002-DYN-{index:02d}",
                    f"{field} mismatch",
                    failures,
                )
            checks_total += 1
            checks_passed += require(
                surface_payload.get("deterministic_handoff") is True,
                display_path(manifest_path),
                "M274-C002-DYN-17",
                "foreign-call/lifetime lowering handoff is not deterministic",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                surface_payload.get("ready_for_ir_emission") is True,
                display_path(manifest_path),
                "M274-C002-DYN-18",
                "foreign-call/lifetime lowering is not ready for IR emission",
                failures,
            )

    ir_text = ir_path.read_text(encoding="utf-8") if ir_path.exists() else ""
    for check_id, snippet in [
        ("M274-C002-DYN-19", "; part11_foreign_call_and_lifetime_lowering = "),
        ("M274-C002-DYN-20", "!objc3.objc_part11_foreign_call_and_lifetime_lowering = !{"),
    ]:
        checks_total += 1
        checks_passed += require(snippet in ir_text, display_path(ir_path), check_id, f"IR missing snippet: {snippet}", failures)

    return checks_total, checks_passed, {
        "runner": display_path(args.runner_exe),
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": completed.returncode,
        "positive_output": output.strip(),
        "positive_manifest": display_path(manifest_path),
        "positive_ir": display_path(ir_path),
        "foreign_call_and_lifetime_lowering": surface_payload,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_summary: dict[str, Any] = {"skipped": True}
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
        "surface_path": SURFACE_PATH,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_executed,
        "dynamic_summary": dynamic_summary,
        "failures": [failure.__dict__ for failure in failures],
        "ok": checks_total > 0 and checks_passed == checks_total and not failures,
    }
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")

    if failures:
        for failure in failures:
            print(f"[{failure.check_id}] {failure.artifact}: {failure.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
