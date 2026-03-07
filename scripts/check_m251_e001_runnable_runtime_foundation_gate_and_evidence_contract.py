#!/usr/bin/env python3
"""Fail-closed checker for M251-E001 runnable-runtime foundation gate."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m251-e001-runnable-runtime-foundation-gate-evidence-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m251_runnable_runtime_foundation_gate_and_evidence_contract_e001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m251"
    / "m251_e001_runnable_runtime_foundation_gate_and_evidence_contract_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_A003_SUMMARY = (
    ROOT / "tmp" / "reports" / "m251" / "M251-A003" / "runtime_record_manifest_handoff_contract_summary.json"
)
DEFAULT_B003_SUMMARY = (
    ROOT / "tmp" / "reports" / "m251" / "M251-B003" / "illegal_runtime_exposed_declaration_diagnostics_summary.json"
)
DEFAULT_C003_SUMMARY = (
    ROOT / "tmp" / "reports" / "m251" / "M251-C003" / "runtime_metadata_object_inspection_harness_summary.json"
)
DEFAULT_D003_SUMMARY = (
    ROOT / "tmp" / "reports" / "m251" / "M251-D003" / "runtime_support_library_link_wiring_summary.json"
)
DEFAULT_D003_SMOKE_SUMMARY = (
    ROOT
    / "tmp"
    / "artifacts"
    / "objc3c-native"
    / "execution-smoke"
    / "m251_d003_runtime_library_link_wiring"
    / "summary.json"
)
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m251/M251-E001/runnable_runtime_foundation_gate_contract_summary.json"
)
CANONICAL_RUNTIME_LIBRARY = "artifacts/lib/objc3_runtime.lib"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E001-DOC-EXP-01",
        "# M251 Lane E Runnable-Runtime Foundation Gate and Evidence Contract Expectations (E001)",
    ),
    SnippetCheck(
        "M251-E001-DOC-EXP-02",
        "Contract ID: `objc3c-runnable-runtime-foundation-gate-evidence-contract/m251-e001-v1`",
    ),
    SnippetCheck("M251-E001-DOC-EXP-03", "`M251-A003`"),
    SnippetCheck("M251-E001-DOC-EXP-04", "`M251-B003`"),
    SnippetCheck("M251-E001-DOC-EXP-05", "`M251-C003`"),
    SnippetCheck("M251-E001-DOC-EXP-06", "`M251-D003`"),
    SnippetCheck("M251-E001-DOC-EXP-07", "`artifacts/lib/objc3_runtime.lib`"),
    SnippetCheck(
        "M251-E001-DOC-EXP-08",
        "`tmp/artifacts/objc3c-native/execution-smoke/m251_d003_runtime_library_link_wiring/summary.json`",
    ),
    SnippetCheck(
        "M251-E001-DOC-EXP-09",
        "`check:objc3c:m251-e001-runnable-runtime-foundation-gate-and-evidence-contract`",
    ),
    SnippetCheck("M251-E001-DOC-EXP-10", "`check:objc3c:m251-e001-lane-e-readiness`"),
    SnippetCheck(
        "M251-E001-DOC-EXP-11",
        "`tmp/reports/m251/M251-E001/runnable_runtime_foundation_gate_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E001-DOC-PKT-01",
        "# M251-E001 Runnable-Runtime Foundation Gate and Evidence Contract Packet",
    ),
    SnippetCheck("M251-E001-DOC-PKT-02", "Packet: `M251-E001`"),
    SnippetCheck("M251-E001-DOC-PKT-03", "Issue: `#7068`"),
    SnippetCheck("M251-E001-DOC-PKT-04", "- none"),
    SnippetCheck("M251-E001-DOC-PKT-05", "`M251-A003`"),
    SnippetCheck("M251-E001-DOC-PKT-06", "`M251-B003`"),
    SnippetCheck("M251-E001-DOC-PKT-07", "`M251-C003`"),
    SnippetCheck("M251-E001-DOC-PKT-08", "`M251-D003`"),
    SnippetCheck(
        "M251-E001-DOC-PKT-09",
        "`scripts/check_m251_e001_runnable_runtime_foundation_gate_and_evidence_contract.py`",
    ),
    SnippetCheck(
        "M251-E001-DOC-PKT-10",
        "`tests/tooling/test_check_m251_e001_runnable_runtime_foundation_gate_and_evidence_contract.py`",
    ),
    SnippetCheck(
        "M251-E001-DOC-PKT-11",
        "`tmp/artifacts/objc3c-native/execution-smoke/m251_d003_runtime_library_link_wiring/summary.json`",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E001-ARCH-01",
        "M251 lane-E E001 runnable-runtime foundation gate anchors explicit aggregate",
    ),
    SnippetCheck(
        "M251-E001-ARCH-02",
        "`M251-A003`, `M251-B003`, `M251-C003`, and `M251-D003`",
    ),
    SnippetCheck("M251-E001-ARCH-03", "`artifacts/lib/objc3_runtime.lib`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E001-SPC-01",
        "M251 runnable-runtime foundation gate and evidence contract (E001)",
    ),
    SnippetCheck(
        "M251-E001-SPC-02",
        "`M251-A003` to remain the canonical manifest/runtime-record handoff proof",
    ),
    SnippetCheck(
        "M251-E001-SPC-03",
        "`M251-D003` to remain the canonical emitted-object runtime-link proof",
    ),
    SnippetCheck("M251-E001-SPC-04", "`runtime_library = artifacts/lib/objc3_runtime.lib`"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E001-META-01",
        "M251 runnable-runtime foundation gate metadata anchors (E001)",
    ),
    SnippetCheck(
        "M251-E001-META-02",
        "objc3c-runnable-runtime-foundation-gate-evidence-contract/m251-e001-v1",
    ),
    SnippetCheck(
        "M251-E001-META-03",
        "`tmp/reports/m251/M251-A003/runtime_record_manifest_handoff_contract_summary.json`",
    ),
    SnippetCheck("M251-E001-META-04", "canonical runtime archive path `artifacts/lib/objc3_runtime.lib`"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E001-PKG-01",
        '"check:objc3c:m251-e001-runnable-runtime-foundation-gate-and-evidence-contract": '
        '"python scripts/check_m251_e001_runnable_runtime_foundation_gate_and_evidence_contract.py"',
    ),
    SnippetCheck(
        "M251-E001-PKG-02",
        '"test:tooling:m251-e001-runnable-runtime-foundation-gate-and-evidence-contract": '
        '"python -m pytest tests/tooling/test_check_m251_e001_runnable_runtime_foundation_gate_and_evidence_contract.py -q"',
    ),
    SnippetCheck(
        "M251-E001-PKG-03",
        '"check:objc3c:m251-e001-lane-e-readiness": '
        '"npm run check:objc3c:m251-a003-lane-a-readiness && npm run check:objc3c:m251-b003-lane-b-readiness '
        '&& npm run check:objc3c:m251-c003-lane-c-readiness && npm run check:objc3c:m251-d003-lane-d-readiness '
        '&& npm run check:objc3c:m251-e001-runnable-runtime-foundation-gate-and-evidence-contract '
        '&& npm run test:tooling:m251-e001-runnable-runtime-foundation-gate-and-evidence-contract"',
    ),
    SnippetCheck("M251-E001-PKG-04", '"check:objc3c:m251-a003-lane-a-readiness": '),
    SnippetCheck("M251-E001-PKG-05", '"check:objc3c:m251-b003-lane-b-readiness": '),
    SnippetCheck("M251-E001-PKG-06", '"check:objc3c:m251-c003-lane-c-readiness": '),
    SnippetCheck("M251-E001-PKG-07", '"check:objc3c:m251-d003-lane-d-readiness": '),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--a003-summary", type=Path, default=DEFAULT_A003_SUMMARY)
    parser.add_argument("--b003-summary", type=Path, default=DEFAULT_B003_SUMMARY)
    parser.add_argument("--c003-summary", type=Path, default=DEFAULT_C003_SUMMARY)
    parser.add_argument("--d003-summary", type=Path, default=DEFAULT_D003_SUMMARY)
    parser.add_argument("--d003-smoke-summary", type=Path, default=DEFAULT_D003_SMOKE_SUMMARY)
    parser.add_argument("--runtime-library", type=Path, default=DEFAULT_RUNTIME_LIBRARY)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_doc_contract(
    *,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings
    if not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"missing required snippet: {snippet.snippet}",
                )
            )
    return checks_total, findings


def load_json_payload(path: Path, *, exists_check_id: str, parse_check_id: str) -> tuple[int, list[Finding], dict[str, Any] | None]:
    checks_total = 2
    findings: list[Finding] = []
    payload: dict[str, Any] | None = None
    if not path.exists():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required JSON payload is missing: {display_path(path)}",
            )
        )
        return checks_total, findings, None
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=parse_check_id,
                detail=f"invalid JSON: {exc}",
            )
        )
    return checks_total, findings, payload


def validate_summary_payload(
    *,
    path: Path,
    payload: dict[str, Any] | None,
    prefix: str,
    expected_mode: str,
) -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    if payload is None:
        return checks_total, findings

    checks_total += 1
    if payload.get("mode") != expected_mode:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=f"{prefix}-MODE",
                detail=f"expected mode {expected_mode!r}, found {payload.get('mode')!r}",
            )
        )

    checks_total += 1
    if payload.get("ok") is not True:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=f"{prefix}-OK",
                detail="summary did not report ok=true",
            )
        )

    checks_total += 1
    if payload.get("dynamic_probes_executed") is not True:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=f"{prefix}-DYN",
                detail="summary did not report dynamic_probes_executed=true",
            )
        )
    return checks_total, findings


def find_case(payload: dict[str, Any], case_id: str) -> dict[str, Any] | None:
    for case in payload.get("dynamic_cases", []):
        if case.get("case_id") == case_id:
            return case
    return None


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M251-E001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M251-E001-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M251-E001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M251-E001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M251-E001-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M251-E001-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        check_count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += check_count
        failures.extend(findings)

    upstream_payloads: dict[str, dict[str, Any] | None] = {}
    for label, path, expected_mode in (
        ("A003", args.a003_summary, "m251-a003-runtime-record-manifest-handoff-contract-v1"),
        ("B003", args.b003_summary, "m251-b003-runtime-export-diagnostics-contract-v1"),
        ("C003", args.c003_summary, "m251-c003-runtime-metadata-object-inspection-harness-v1"),
        ("D003", args.d003_summary, "m251-d003-runtime-library-link-wiring-v1"),
    ):
        check_count, findings, payload = load_json_payload(
            path,
            exists_check_id=f"M251-E001-{label}-SUM-EXISTS",
            parse_check_id=f"M251-E001-{label}-SUM-PARSE",
        )
        checks_total += check_count
        failures.extend(findings)
        extra_checks, extra_findings = validate_summary_payload(
            path=path,
            payload=payload,
            prefix=f"M251-E001-{label}",
            expected_mode=expected_mode,
        )
        checks_total += extra_checks
        failures.extend(extra_findings)
        upstream_payloads[label] = payload

    a003_payload = upstream_payloads["A003"]
    if a003_payload is not None:
        a003_class = find_case(a003_payload, "M251-A003-CASE-CLASS")
        a003_cli = find_case(a003_payload, "M251-A003-CASE-CLI")
        checks_total += 2
        if not a003_class or a003_class.get("success") is not True:
            failures.append(
                Finding(
                    artifact=display_path(args.a003_summary),
                    check_id="M251-E001-A003-CASE-CLASS",
                    detail="A003 class manifest handoff case is missing or not successful",
                )
            )
        if not a003_cli or a003_cli.get("manifest_exists") is not True or a003_cli.get("ir_exists") is not False:
            failures.append(
                Finding(
                    artifact=display_path(args.a003_summary),
                    check_id="M251-E001-A003-CASE-CLI",
                    detail="A003 CLI fail-closed manifest preservation evidence drifted",
                )
            )

    b003_payload = upstream_payloads["B003"]
    if b003_payload is not None:
        b003_class = find_case(b003_payload, "M251-B003-CASE-CLASS")
        b003_incomplete = find_case(b003_payload, "M251-B003-CASE-INCOMPLETE-INTERFACE")
        checks_total += 2
        if not b003_class or b003_class.get("runtime_export_ready_for_runtime_export") is not True:
            failures.append(
                Finding(
                    artifact=display_path(args.b003_summary),
                    check_id="M251-E001-B003-CASE-CLASS",
                    detail="B003 class export-ready proof is missing or false",
                )
            )
        if not b003_incomplete or "O3S260" not in b003_incomplete.get("diagnostic_codes", []):
            failures.append(
                Finding(
                    artifact=display_path(args.b003_summary),
                    check_id="M251-E001-B003-CASE-INCOMPLETE",
                    detail="B003 incomplete-interface diagnostic proof is missing O3S260",
                )
            )

    c003_payload = upstream_payloads["C003"]
    if c003_payload is not None:
        c003_object = find_case(c003_payload, "M251-C003-CASE-OBJECT-ZERO-DESCRIPTOR")
        checks_total += 3
        if not c003_payload.get("tool_paths", {}).get("llvm_readobj"):
            failures.append(
                Finding(
                    artifact=display_path(args.c003_summary),
                    check_id="M251-E001-C003-TOOL-READOBJ",
                    detail="C003 tool_paths.llvm_readobj is missing",
                )
            )
        if not c003_payload.get("tool_paths", {}).get("llvm_objdump"):
            failures.append(
                Finding(
                    artifact=display_path(args.c003_summary),
                    check_id="M251-E001-C003-TOOL-OBJDUMP",
                    detail="C003 tool_paths.llvm_objdump is missing",
                )
            )
        expected_sections = {
            "objc3.runtime.image_info",
            "objc3.runtime.class_descriptors",
            "objc3.runtime.protocol_descriptors",
            "objc3.runtime.category_descriptors",
            "objc3.runtime.property_descriptors",
            "objc3.runtime.ivar_descriptors",
        }
        actual_sections = {
            section.get("name")
            for section in (c003_object or {}).get("metadata_sections", [])
            if isinstance(section, dict)
        }
        if not c003_object or not expected_sections.issubset(actual_sections):
            failures.append(
                Finding(
                    artifact=display_path(args.c003_summary),
                    check_id="M251-E001-C003-CASE-OBJECT",
                    detail="C003 object inspection section inventory drifted",
                )
            )

    d003_payload = upstream_payloads["D003"]
    if d003_payload is not None:
        d003_case = find_case(d003_payload, "M251-D003-CASE-SMOKE")
        checks_total += 2
        if not d003_case or d003_case.get("summary_status") != "PASS":
            failures.append(
                Finding(
                    artifact=display_path(args.d003_summary),
                    check_id="M251-E001-D003-CASE-SMOKE",
                    detail="D003 smoke case is missing or not PASS",
                )
            )
        if not d003_case or d003_case.get("runtime_library") != CANONICAL_RUNTIME_LIBRARY:
            failures.append(
                Finding(
                    artifact=display_path(args.d003_summary),
                    check_id="M251-E001-D003-RUNTIME-LIB",
                    detail="D003 summary no longer points at artifacts/lib/objc3_runtime.lib",
                )
            )

    check_count, findings, d003_smoke_payload = load_json_payload(
        args.d003_smoke_summary,
        exists_check_id="M251-E001-D003-SMOKE-EXISTS",
        parse_check_id="M251-E001-D003-SMOKE-PARSE",
    )
    checks_total += check_count
    failures.extend(findings)
    smoke_positive_count = 0
    if d003_smoke_payload is not None:
        checks_total += 4
        if d003_smoke_payload.get("status") != "PASS":
            failures.append(
                Finding(
                    artifact=display_path(args.d003_smoke_summary),
                    check_id="M251-E001-D003-SMOKE-STATUS",
                    detail="D003 smoke summary did not report PASS",
                )
            )
        if d003_smoke_payload.get("runtime_library") != CANONICAL_RUNTIME_LIBRARY:
            failures.append(
                Finding(
                    artifact=display_path(args.d003_smoke_summary),
                    check_id="M251-E001-D003-SMOKE-LIB",
                    detail="D003 smoke summary runtime_library drifted from artifacts/lib/objc3_runtime.lib",
                )
            )
        positive_runtime_results = [
            result
            for result in d003_smoke_payload.get("results", [])
            if result.get("kind") == "positive"
            and result.get("requires_runtime_shim") is True
            and result.get("runtime_library") == CANONICAL_RUNTIME_LIBRARY
            and result.get("passed") is True
        ]
        smoke_positive_count = len(positive_runtime_results)
        if smoke_positive_count == 0:
            failures.append(
                Finding(
                    artifact=display_path(args.d003_smoke_summary),
                    check_id="M251-E001-D003-SMOKE-POSITIVE",
                    detail="no passing runtime-linked positive fixtures were found in the D003 smoke summary",
                )
            )
        if d003_smoke_payload.get("failed") != 0:
            failures.append(
                Finding(
                    artifact=display_path(args.d003_smoke_summary),
                    check_id="M251-E001-D003-SMOKE-FAILED",
                    detail="D003 smoke summary reported failed fixtures",
                )
            )

    checks_total += 1
    runtime_library_exists = args.runtime_library.exists()
    if not runtime_library_exists:
        failures.append(
            Finding(
                artifact=display_path(args.runtime_library),
                check_id="M251-E001-RUNTIME-LIB-EXISTS",
                detail="canonical runtime archive is missing",
            )
        )

    checks_passed = checks_total - len(failures)
    ok = not failures
    summary_payload = {
        "mode": MODE,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "upstream_evidence": {
            "a003_summary": display_path(args.a003_summary),
            "b003_summary": display_path(args.b003_summary),
            "c003_summary": display_path(args.c003_summary),
            "d003_summary": display_path(args.d003_summary),
            "d003_smoke_summary": display_path(args.d003_smoke_summary),
            "runtime_library": display_path(args.runtime_library),
        },
        "smoke_positive_runtime_fixture_count": smoke_positive_count,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in failures
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")

    if ok:
        print(
            f"[PASS] M251-E001 runnable-runtime foundation gate preserved; summary: {display_path(args.summary_out)}"
        )
        return 0

    print(
        f"[FAIL] M251-E001 runnable-runtime foundation gate drift detected; summary: {display_path(args.summary_out)}",
        file=sys.stderr,
    )
    for finding in failures:
        print(f" - {finding.check_id} :: {finding.detail} [{finding.artifact}]", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
