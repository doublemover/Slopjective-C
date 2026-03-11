#!/usr/bin/env python3
"""Validate M260-E002 runnable ownership smoke matrix closeout."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m260-e002-runnable-ownership-smoke-matrix-closeout-v1"
CONTRACT_ID = "objc3c-runnable-ownership-smoke-matrix/m260-e002-v1"
MATRIX_MODEL = (
    "closeout-matrix-consumes-a002-b003-c002-d002-and-e001-evidence-without-widening-the-runtime-backed-ownership-slice"
)
RUNNABLE_SMOKE_MODEL = (
    "real-integrated-compile-and-runtime-probe-prove-meaningful-strong-weak-and-autoreleasepool-object-programs"
)
FAILURE_MODEL = "fail-closed-on-ownership-smoke-matrix-drift-or-closeout-doc-mismatch"
NEXT_ISSUE = "M261-A001"

A002_CONTRACT_ID = "objc3c-runtime-backed-object-ownership-attribute-surface/m260-a002-v1"
B003_CONTRACT_ID = "objc3c-runtime-backed-autoreleasepool-destruction-order-semantics/m260-b003-v1"
C002_CONTRACT_ID = "objc3c-ownership-runtime-hook-emission/m260-c002-v1"
D002_CONTRACT_ID = "objc3c-runtime-memory-management-implementation/m260-d002-v1"
E001_CONTRACT_ID = "objc3c-ownership-runtime-gate-freeze/m260-e001-v1"

EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m260_runnable_ownership_smoke_matrix_and_docs_cross_lane_integration_sync_e002_expectations.md"
)
PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m260"
    / "m260_e002_runnable_ownership_smoke_matrix_and_docs_cross_lane_integration_sync_packet.md"
)
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
PACKAGE_JSON = ROOT / "package.json"
A002_SUMMARY = (
    ROOT / "tmp" / "reports" / "m260" / "M260-A002" / "runtime_backed_object_ownership_attribute_surface_summary.json"
)
B003_SUMMARY = (
    ROOT / "tmp" / "reports" / "m260" / "M260-B003" / "pytest_autoreleasepool_destruction_order_semantics_summary.json"
)
C002_SUMMARY = ROOT / "tmp" / "reports" / "m260" / "M260-C002" / "ownership_runtime_hook_emission_summary.json"
D002_SUMMARY = (
    ROOT / "tmp" / "reports" / "m260" / "M260-D002" / "reference_counting_weak_autoreleasepool_summary.json"
)
E001_SUMMARY = (
    ROOT / "tmp" / "reports" / "m260" / "M260-E001" / "ownership_runtime_gate_contract_summary.json"
)
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m260" / "M260-E002" / "runnable_ownership_smoke_matrix_summary.json"


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
    SnippetCheck(
        "M260-E002-EXP-01",
        "# M260 Runnable Ownership Smoke Matrix And Docs Cross-Lane Integration Sync Expectations (E002)",
    ),
    SnippetCheck("M260-E002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M260-E002-EXP-03", "tmp/reports/m260/M260-E001/ownership_runtime_gate_contract_summary.json"),
    SnippetCheck("M260-E002-EXP-04", "`M261-A001` is the next issue"),
)
PACKET_SNIPPETS = (
    SnippetCheck(
        "M260-E002-PKT-01",
        "# M260-E002 Runnable Ownership Smoke Matrix And Docs Cross-Lane Integration Sync Packet",
    ),
    SnippetCheck("M260-E002-PKT-02", "Issue: `#7178`"),
    SnippetCheck("M260-E002-PKT-03", "Packet: `M260-E002`"),
    SnippetCheck("M260-E002-PKT-04", "- `M260-D002`"),
    SnippetCheck("M260-E002-PKT-05", "- `M260-E001`"),
)
DOC_SOURCE_SNIPPETS = (
    SnippetCheck("M260-E002-SRC-01", "## M260 runnable ownership smoke matrix and docs (E002)"),
    SnippetCheck("M260-E002-SRC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-E002-SRC-03", "tmp/reports/m260/M260-E002/runnable_ownership_smoke_matrix_summary.json"),
    SnippetCheck("M260-E002-SRC-04", "the next issue is `M261-A001`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M260-E002-NDOC-01", "## M260 runnable ownership smoke matrix and docs (E002)"),
    SnippetCheck("M260-E002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-E002-NDOC-03", "this is a closeout matrix, not a new ownership/runtime feature"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M260-E002-SPC-01", "## M260 runnable ownership smoke matrix and docs (E002)"),
    SnippetCheck("M260-E002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-E002-SPC-03", f"`{MATRIX_MODEL}`"),
    SnippetCheck("M260-E002-SPC-04", f"`{RUNNABLE_SMOKE_MODEL}`"),
    SnippetCheck("M260-E002-SPC-05", f"`{FAILURE_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M260-E002-META-01", "## M260 runnable ownership smoke matrix metadata anchors (E002)"),
    SnippetCheck("M260-E002-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-E002-META-03", "tmp/reports/m260/M260-E002/runnable_ownership_smoke_matrix_summary.json"),
    SnippetCheck("M260-E002-META-04", "`!objc3.objc_ownership_runtime_gate`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M260-E002-ARCH-01", "## M260 Runnable Ownership Smoke Matrix And Docs (E002)"),
    SnippetCheck("M260-E002-ARCH-02", "the closeout matrix consumes the already-live `A002`, `B003`, `C002`, `D002`,"),
    SnippetCheck("M260-E002-ARCH-03", "the next issue is `M261-A001`"),
)
SEMA_SNIPPETS = (
    SnippetCheck("M260-E002-SEMA-01", "M260-E002 ownership-smoke closeout anchor"),
)
IR_SNIPPETS = (
    SnippetCheck("M260-E002-IR-01", "M260-E002 ownership-smoke closeout anchor"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M260-E002-LCPP-01", "M260-E002 ownership-smoke closeout anchor"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M260-E002-PKG-01", '"check:objc3c:m260-e002-runnable-ownership-smoke-matrix"'),
    SnippetCheck("M260-E002-PKG-02", '"test:tooling:m260-e002-runnable-ownership-smoke-matrix"'),
    SnippetCheck("M260-E002-PKG-03", '"check:objc3c:m260-e002-lane-e-readiness"'),
)


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


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    artifact = display_path(path)
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(artifact, snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=PACKET_DOC)
    parser.add_argument("--doc-source", type=Path, default=DOC_SOURCE)
    parser.add_argument("--native-doc", type=Path, default=NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=METADATA_SPEC)
    parser.add_argument("--architecture-doc", type=Path, default=ARCHITECTURE_DOC)
    parser.add_argument("--sema-cpp", type=Path, default=SEMA_CPP)
    parser.add_argument("--ir-cpp", type=Path, default=IR_CPP)
    parser.add_argument("--lowering-cpp", type=Path, default=LOWERING_CPP)
    parser.add_argument("--package-json", type=Path, default=PACKAGE_JSON)
    parser.add_argument("--a002-summary", type=Path, default=A002_SUMMARY)
    parser.add_argument("--b003-summary", type=Path, default=B003_SUMMARY)
    parser.add_argument("--c002-summary", type=Path, default=C002_SUMMARY)
    parser.add_argument("--d002-summary", type=Path, default=D002_SUMMARY)
    parser.add_argument("--e001-summary", type=Path, default=E001_SUMMARY)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def validate_summary(
    name: str,
    path: Path,
    expected_contract_id: str,
    failures: list[Finding],
) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    total += 1
    passed += require(payload.get("contract_id") == expected_contract_id, artifact, f"{name}-01", "contract id drifted", failures)
    total += 1
    if "ok" in payload:
        passed += require(payload.get("ok") is True, artifact, f"{name}-02", "summary must report ok=true", failures)
    else:
        passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, f"{name}-02", "checks coverage drifted", failures)
    total += 1
    passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, f"{name}-03", "checks_passed must equal checks_total", failures)
    return total, passed, payload


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    file_checks: tuple[tuple[Path, Sequence[SnippetCheck]], ...] = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.doc_source, DOC_SOURCE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.ir_cpp, IR_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in file_checks:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    upstream: dict[str, Any] = {}
    for name, path, contract_id in (
        ("M260-A002", args.a002_summary, A002_CONTRACT_ID),
        ("M260-B003", args.b003_summary, B003_CONTRACT_ID),
        ("M260-C002", args.c002_summary, C002_CONTRACT_ID),
        ("M260-D002", args.d002_summary, D002_CONTRACT_ID),
        ("M260-E001", args.e001_summary, E001_CONTRACT_ID),
    ):
        total, passed, payload = validate_summary(name, path, contract_id, failures)
        checks_total += total
        checks_passed += passed
        upstream[name] = payload

    artifact = "matrix"
    matrix_rows: list[dict[str, Any]] = []

    a002 = upstream["M260-A002"]
    checks_total += 2
    checks_passed += require(
        a002.get("dynamic_probe", {}).get("executed") is True,
        artifact,
        "M260-E002-MATRIX-01",
        "A002 must preserve a live compile-backed ownership attribute proof",
        failures,
    )
    checks_passed += require(
        a002.get("dynamic_probe", {}).get("returncode") == 0,
        artifact,
        "M260-E002-MATRIX-02",
        "A002 positive compile must succeed",
        failures,
    )
    matrix_rows.append({"row": "A002-ownership-attribute-surface", "status": "ok", "evidence": display_path(args.a002_summary)})

    b003 = upstream["M260-B003"]
    checks_total += 2
    checks_passed += require(
        b003.get("dynamic_probe", {}).get("skipped") is True,
        artifact,
        "M260-E002-MATRIX-03",
        "B003 must remain consumable as a static historical semantic-guardrail proof",
        failures,
    )
    checks_passed += require(
        b003.get("ok") is True,
        artifact,
        "M260-E002-MATRIX-04",
        "B003 historical semantic-guardrail proof must remain clean",
        failures,
    )
    matrix_rows.append({"row": "B003-autoreleasepool-guardrail", "status": "ok", "evidence": display_path(args.b003_summary)})

    c002 = upstream["M260-C002"]
    checks_total += 1
    checks_passed += require(
        c002.get("ok") is True or c002.get("checks_passed") == c002.get("checks_total"),
        artifact,
        "M260-E002-MATRIX-05",
        "C002 lowering boundary must remain ready",
        failures,
    )
    matrix_rows.append({"row": "C002-ownership-runtime-hook-boundary", "status": "ok", "evidence": display_path(args.c002_summary)})

    d002 = upstream["M260-D002"]
    checks_total += 3
    checks_passed += require(
        d002.get("dynamic", {}).get("skipped") is False,
        artifact,
        "M260-E002-MATRIX-06",
        "D002 runtime probe must remain live",
        failures,
    )
    checks_passed += require(
        d002.get("dynamic", {}).get("probe_payload", {}).get("weak_after_pool") == 0,
        artifact,
        "M260-E002-MATRIX-07",
        "D002 runtime probe must prove weak zeroing after pool drain",
        failures,
    )
    checks_passed += require(
        d002.get("dynamic", {}).get("probe_payload", {}).get("memory_after_parent_release", {}).get("live_runtime_instance_count") == 0,
        artifact,
        "M260-E002-MATRIX-08",
        "D002 runtime probe must prove final release destroys all live runtime instances",
        failures,
    )
    matrix_rows.append({"row": "D002-runtime-memory-management-execution", "status": "ok", "evidence": display_path(args.d002_summary)})

    e001 = upstream["M260-E001"]
    checks_total += 3
    checks_passed += require(
        e001.get("dynamic_probes_executed") is True,
        artifact,
        "M260-E002-MATRIX-09",
        "E001 gate proof must preserve its live dynamic probe",
        failures,
    )
    boundary = str(e001.get("dynamic_case", {}).get("boundary", ""))
    checks_passed += require(
        f"contract={E001_CONTRACT_ID}" in boundary and f"memory_implementation_contract={D002_CONTRACT_ID}" in boundary,
        artifact,
        "M260-E002-MATRIX-10",
        "E001 boundary must continue to name the frozen gate and D002 implementation proof",
        failures,
    )
    checks_passed += require(
        "no-arc-automation-no-block-ownership-runtime-no-public-ownership-api-widening" in boundary,
        artifact,
        "M260-E002-MATRIX-11",
        "E001 boundary must continue to publish the non-goal model",
        failures,
    )
    matrix_rows.append(
        {
            "row": "E001-ownership-runtime-gate",
            "status": "ok",
            "evidence": display_path(args.e001_summary),
            "boundary": boundary,
        }
    )

    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "matrix_model": MATRIX_MODEL,
        "runnable_smoke_model": RUNNABLE_SMOKE_MODEL,
        "failure_model": FAILURE_MODEL,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "matrix_rows": matrix_rows,
        "upstream_summaries": {
            key: {"contract_id": value.get("contract_id")}
            for key, value in upstream.items()
        },
        "next_issue": NEXT_ISSUE,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.check_id} {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {CONTRACT_ID} passed {checks_passed}/{checks_total} checks", flush=True)
    print(f"[ok] summary: {display_path(args.summary_out)}", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
