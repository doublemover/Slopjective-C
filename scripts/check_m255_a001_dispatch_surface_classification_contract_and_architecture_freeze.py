#!/usr/bin/env python3
"""Fail-closed checker for M255-A001 dispatch surface classification freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m255-a001-dispatch-surface-classification-freeze-v1"
CONTRACT_ID = "objc3c-dispatch-surface-classification/m255-a001-v1"
LIVE_RUNTIME_ENTRYPOINT_FAMILY = "objc3_runtime_dispatch_i32-objc3_msgsend_i32-compat"
DIRECT_DISPATCH_BINDING = "reserved-non-goal"
NEXT_ISSUE = "M255-A002"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m255_dispatch_surface_classification_contract_and_architecture_freeze_a001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m255" / "m255_a001_dispatch_surface_classification_contract_and_architecture_freeze_packet.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PARSER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_RUNTIME_SHIM = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m255_a001_lane_a_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = ROOT / "tmp" / "reports" / "m255" / "M255-A001" / "dispatch_surface_classification_contract_summary.json"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M255-A001-DOC-EXP-01", "# M255 Dispatch Surface Classification Contract And Architecture Freeze Expectations (A001)"),
    SnippetCheck("M255-A001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M255-A001-DOC-EXP-03", "instance dispatch -> live runtime dispatch family"),
    SnippetCheck("M255-A001-DOC-EXP-04", "direct dispatch -> reserved non-goal in `M255-A001`"),
    SnippetCheck("M255-A001-DOC-EXP-05", "The freeze must explicitly hand off to `M255-A002`."),
)
PACKET_SNIPPETS = (
    SnippetCheck("M255-A001-DOC-PKT-01", "# M255-A001 Dispatch Surface Classification Contract And Architecture Freeze Packet"),
    SnippetCheck("M255-A001-DOC-PKT-02", "Packet: `M255-A001`"),
    SnippetCheck("M255-A001-DOC-PKT-03", "- direct -> reserved non-goal until `M255-A002`"),
    SnippetCheck("M255-A001-DOC-PKT-04", "`check:objc3c:m255-a001-lane-a-readiness`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M255-A001-NDOC-01", "## Dispatch surface classification (M255-A001)"),
    SnippetCheck("M255-A001-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-A001-NDOC-03", f"`{LIVE_RUNTIME_ENTRYPOINT_FAMILY}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M255-A001-SPC-01", "## M255 dispatch surface classification (A001)"),
    SnippetCheck("M255-A001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-A001-SPC-03", "direct dispatch remains a reserved non-goal in `M255-A001`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M255-A001-META-01", "## M255 dispatch/runtime metadata anchors (A001)"),
    SnippetCheck("M255-A001-META-02", "`objc3_msgsend_i32`"),
    SnippetCheck("M255-A001-META-03", "`objc3_runtime_dispatch_i32`"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M255-A001-HDR-01", "kObjc3DispatchSurfaceClassificationContractId"),
    SnippetCheck("M255-A001-HDR-02", "kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily"),
    SnippetCheck("M255-A001-HDR-03", "kObjc3DispatchSurfaceDirectDispatchBinding"),
    SnippetCheck("M255-A001-HDR-04", "struct Objc3DispatchSurfaceClassificationContract"),
    SnippetCheck("M255-A001-HDR-05", "std::string direct_entrypoint_family = kObjc3DispatchSurfaceDirectDispatchBinding;"),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M255-A001-IR-01", "M255-A001 dispatch-surface classification anchor"),
    SnippetCheck("M255-A001-IR-02", "direct dispatch remains an explicit non-goal for this freeze"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M255-A001-PARSE-01", "M255-A001 dispatch-surface classification anchor"),
    SnippetCheck("M255-A001-PARSE-02", "super receivers stay explicit while direct dispatch remains reserved"),
)
RUNTIME_SHIM_SNIPPETS = (
    SnippetCheck("M255-A001-SHIM-01", "M255-A001 dispatch-classification freeze"),
    SnippetCheck("M255-A001-SHIM-02", "instance/class/super/dynamic dispatch families all remain routed through"),
    SnippetCheck("M255-A001-SHIM-03", "direct dispatch remains a reserved non-goal"),
)
READINESS_RUNNER_SNIPPETS = (
    SnippetCheck("M255-A001-RUN-01", "check:objc3c:m255-a001-dispatch-surface-classification-contract-and-architecture-freeze"),
    SnippetCheck("M255-A001-RUN-02", "test_check_m255_a001_dispatch_surface_classification_contract_and_architecture_freeze.py"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M255-A001-PKG-01", '"check:objc3c:m255-a001-dispatch-surface-classification-contract-and-architecture-freeze": "python scripts/check_m255_a001_dispatch_surface_classification_contract_and_architecture_freeze.py"'),
    SnippetCheck("M255-A001-PKG-02", '"test:tooling:m255-a001-dispatch-surface-classification-contract-and-architecture-freeze": "python -m pytest tests/tooling/test_check_m255_a001_dispatch_surface_classification_contract_and_architecture_freeze.py -q"'),
    SnippetCheck("M255-A001-PKG-03", '"check:objc3c:m255-a001-lane-a-readiness": "python scripts/run_m255_a001_lane_a_readiness.py"'),
)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--parser", type=Path, default=DEFAULT_PARSER)
    parser.add_argument("--runtime-shim", type=Path, default=DEFAULT_RUNTIME_SHIM)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M255-A001-MISSING", f"required artifact is missing: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    targets = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.parser, PARSER_SNIPPETS),
        (args.runtime_shim, RUNTIME_SHIM_SNIPPETS),
        (args.readiness_runner, READINESS_RUNNER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in targets:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "live_runtime_entrypoint_family": LIVE_RUNTIME_ENTRYPOINT_FAMILY,
        "direct_dispatch_binding": DIRECT_DISPATCH_BINDING,
        "next_issue": NEXT_ISSUE,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[FAIL] {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
