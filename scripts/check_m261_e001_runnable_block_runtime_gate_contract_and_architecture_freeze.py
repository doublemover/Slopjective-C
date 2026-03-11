#!/usr/bin/env python3
"""Fail-closed checker for M261-E001 runnable block-runtime gate."""

from __future__ import annotations

import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m261-e001-runnable-block-runtime-gate-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-runnable-block-runtime-gate/m261-e001-v1"
EVIDENCE_MODEL = "a003-b003-c004-d003-summary-chain"
ACTIVE_MODEL = (
    "runnable-block-gate-consumes-source-sema-lowering-and-runtime-proofs-rather-than-metadata-only-summaries"
)
FAIL_CLOSED_MODEL = "fail-closed-on-runnable-block-runtime-evidence-drift"
NEXT_CLOSEOUT_ISSUE = "M261-E002"

M261_A003_CONTRACT_ID = "objc3c-executable-block-source-storage-annotation/m261-a003-v1"
M261_B003_CONTRACT_ID = "objc3c-executable-block-byref-copy-dispose-and-object-capture-ownership/m261-b003-v1"
M261_C004_CONTRACT_ID = "objc3c-executable-block-escape-runtime-hook-lowering/m261-c004-v1"
M261_D003_CONTRACT_ID = "objc3c-runtime-block-byref-forwarding-heap-promotion-interop/m261-d003-v1"

BOUNDARY_PREFIX = "; runnable_block_runtime_gate = "
NAMED_METADATA_LINE = "!objc3.objc_runnable_block_runtime_gate = !{!73}"

EXPECTATIONS_DOC = (
    ROOT / "docs" / "contracts" / "m261_runnable_block_runtime_gate_contract_and_architecture_freeze_e001_expectations.md"
)
PACKET_DOC = (
    ROOT / "spec" / "planning" / "compiler" / "m261" / "m261_e001_runnable_block_runtime_gate_contract_and_architecture_freeze_packet.md"
)
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
PART0_SPEC = ROOT / "spec" / "PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
READINESS_RUNNER = ROOT / "scripts" / "run_m261_e001_lane_e_readiness.py"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_escaping_block_runtime_hook_byref_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m261" / "e001"
A003_SUMMARY = ROOT / "tmp" / "reports" / "m261" / "M261-A003" / "block_source_storage_annotations_summary.json"
B003_SUMMARY = ROOT / "tmp" / "reports" / "m261" / "M261-B003" / "byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_summary.json"
C004_SUMMARY = ROOT / "tmp" / "reports" / "m261" / "M261-C004" / "escaping_block_runtime_hook_lowering_summary.json"
D003_SUMMARY = ROOT / "tmp" / "reports" / "m261" / "M261-D003" / "block_runtime_byref_forwarding_heap_promotion_ownership_interop_summary.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m261" / "M261-E001" / "block_runtime_gate_summary.json"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M261-E001-EXP-01", "# M261 Runnable Block Runtime Gate Contract And Architecture Freeze Expectations (E001)"),
        SnippetCheck("M261-E001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M261-E001-EXP-03", "Issue: `#7192`"),
        SnippetCheck("M261-E001-EXP-04", "tmp/reports/m261/M261-D003/block_runtime_byref_forwarding_heap_promotion_ownership_interop_summary.json"),
        SnippetCheck("M261-E001-EXP-05", "The gate must explicitly hand off to `M261-E002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M261-E001-PKT-01", "# M261-E001 Runnable Block Runtime Gate Contract And Architecture Freeze Packet"),
        SnippetCheck("M261-E001-PKT-02", "Packet: `M261-E001`"),
        SnippetCheck("M261-E001-PKT-03", "Issue: `#7192`"),
        SnippetCheck("M261-E001-PKT-04", "- `M261-C004`"),
        SnippetCheck("M261-E001-PKT-05", "- `M261-D003`"),
        SnippetCheck("M261-E001-PKT-06", "`M261-E002` is the explicit next issue"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M261-E001-SRC-01", "## M261 runnable block-runtime gate (M261-E001)"),
        SnippetCheck("M261-E001-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-E001-SRC-03", "`!objc3.objc_runnable_block_runtime_gate`"),
        SnippetCheck("M261-E001-SRC-04", "`M261-E002` is the next issue."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M261-E001-NDOC-01", "## M261 runnable block-runtime gate (M261-E001)"),
        SnippetCheck("M261-E001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-E001-NDOC-03", "`!objc3.objc_runnable_block_runtime_gate`"),
        SnippetCheck("M261-E001-NDOC-04", "`M261-E002` is the next issue."),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M261-E001-SPC-01", "## M261 runnable block-runtime gate (E001)"),
        SnippetCheck("M261-E001-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-E001-SPC-03", f"`{EVIDENCE_MODEL}`"),
        SnippetCheck("M261-E001-SPC-04", "`!objc3.objc_runnable_block_runtime_gate`"),
    ),
    PART0_SPEC: (
        SnippetCheck("M261-E001-P0-01", "(`M261-E001`)"),
        SnippetCheck("M261-E001-P0-02", "cannot regress to metadata-only"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M261-E001-ARCH-01", "## M261 Runnable Block-Runtime Gate (E001)"),
        SnippetCheck("M261-E001-ARCH-02", "`A003/B003/C004/D003`"),
        SnippetCheck("M261-E001-ARCH-03", "`!objc3.objc_runnable_block_runtime_gate`"),
        SnippetCheck("M261-E001-ARCH-04", "the next issue is `M261-E002`"),
    ),
    PARSER_CPP: (
        SnippetCheck("M261-E001-PARSE-01", "M261-E001 runnable-block-runtime gate anchor"),
    ),
    AST_HEADER: (
        SnippetCheck("M261-E001-AST-01", "kObjc3RunnableBlockRuntimeGateContractId"),
        SnippetCheck("M261-E001-AST-02", "kObjc3RunnableBlockRuntimeGateEvidenceModel"),
        SnippetCheck("M261-E001-AST-03", "kObjc3RunnableBlockRuntimeGateFailClosedModel"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M261-E001-SEMA-01", "M261-E001 runnable-block-runtime gate anchor"),
        SnippetCheck("M261-E001-SEMA-02", "metadata-only evidence"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M261-E001-LHDR-01", "kObjc3RunnableBlockRuntimeGateContractId"),
        SnippetCheck("M261-E001-LHDR-02", "kObjc3RunnableBlockRuntimeGateEvidenceModel"),
        SnippetCheck("M261-E001-LHDR-03", "std::string Objc3RunnableBlockRuntimeGateSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M261-E001-LCPP-01", "std::string Objc3RunnableBlockRuntimeGateSummary()"),
        SnippetCheck("M261-E001-LCPP-02", "runnable-block-runtime gate anchor"),
        SnippetCheck("M261-E001-LCPP-03", "source_contract="),
        SnippetCheck("M261-E001-LCPP-04", "runtime_contract="),
    ),
    IR_CPP: (
        SnippetCheck("M261-E001-IR-01", "M261-E001 runnable-block-runtime gate anchor"),
        SnippetCheck("M261-E001-IR-02", "runnable_block_runtime_gate = "),
        SnippetCheck("M261-E001-IR-03", "objc_runnable_block_runtime_gate"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M261-E001-RUN-01", "build_objc3c_native_docs.py"),
        SnippetCheck("M261-E001-RUN-02", "build:objc3c-native"),
        SnippetCheck("M261-E001-RUN-03", "check_m261_a003_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion.py"),
        SnippetCheck("M261-E001-RUN-04", "check_m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion.py"),
        SnippetCheck("M261-E001-RUN-05", "test_check_m261_e001_runnable_block_runtime_gate_contract_and_architecture_freeze.py"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M261-E001-PKG-01", '"check:objc3c:m261-e001-runnable-block-runtime-gate": "python scripts/check_m261_e001_runnable_block_runtime_gate_contract_and_architecture_freeze.py"'),
        SnippetCheck("M261-E001-PKG-02", '"test:tooling:m261-e001-runnable-block-runtime-gate": "python -m pytest tests/tooling/test_check_m261_e001_runnable_block_runtime_gate_contract_and_architecture_freeze.py -q"'),
        SnippetCheck("M261-E001-PKG-03", '"check:objc3c:m261-e001-lane-e-readiness": "python scripts/run_m261_e001_lane_e_readiness.py"'),
    ),
}


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


def run_process(command: Sequence[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT if cwd is None else cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, int, list[Finding]]:
    failures: list[Finding] = []
    total = len(snippets) + 1
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return total, 0, failures
    text = read_text(path)
    passed = 1
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return total, passed, failures


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def compile_fixture(fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_process([str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module"])


def validate_upstream_summary(
    issue: str,
    path: Path,
    expected_contract: str,
    extra_checks: Sequence[tuple[str, bool, str]],
    failures: list[Finding],
) -> tuple[int, int]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    for check_id, condition, detail in (
        (f"{issue}-SUM-01", payload.get("ok") is True, "summary must report ok=true"),
        (f"{issue}-SUM-02", payload.get("contract_id") == expected_contract, f"expected contract_id {expected_contract!r}"),
        (f"{issue}-SUM-03", payload.get("checks_passed") == payload.get("checks_total"), "summary must report checks_passed == checks_total"),
        *extra_checks,
    ):
        total += 1
        passed += require(condition, artifact, check_id, detail, failures)
    return passed, total


def main(argv: Sequence[str] | None = None) -> int:
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    dynamic: dict[str, Any] = {"skipped": False}

    for path, snippets in STATIC_SNIPPETS.items():
        total, passed, static_failures = check_static_contract(path, snippets)
        checks_total += total
        checks_passed += passed
        failures.extend(static_failures)

    a003_payload = load_json(A003_SUMMARY)
    b003_payload = load_json(B003_SUMMARY)
    c004_payload = load_json(C004_SUMMARY)
    d003_payload = load_json(D003_SUMMARY)

    for issue, path, contract, extras in (
        (
            "M261-A003",
            A003_SUMMARY,
            M261_A003_CONTRACT_ID,
            (),
        ),
        (
            "M261-B003",
            B003_SUMMARY,
            M261_B003_CONTRACT_ID,
            (),
        ),
        (
            "M261-C004",
            C004_SUMMARY,
            M261_C004_CONTRACT_ID,
            (
                (
                    "M261-C004-SUM-04",
                    c004_payload.get("execution_evidence_model")
                    == "native-compile-link-run-proves-returned-and-argument-passed-readonly-scalar-block-values-through-runtime-promotion-hooks",
                    "C004 must preserve escaping block runtime-hook execution evidence",
                ),
            ),
        ),
        (
            "M261-D003",
            D003_SUMMARY,
            M261_D003_CONTRACT_ID,
            (
                ("M261-D003-SUM-04", d003_payload.get("dynamic_probes_executed") is True, "D003 must retain live runtime probe evidence"),
                (
                    "M261-D003-SUM-05",
                    d003_payload.get("dynamic", {}).get("probe_payload", {}).get("dispose_count_after_final_release") == 1,
                    "D003 probe must still prove exactly one final dispose",
                ),
            ),
        ),
    ):
        passed, total = validate_upstream_summary(issue, path, contract, extras, failures)
        checks_passed += passed
        checks_total += total

    checks_total += 1
    if NATIVE_EXE.exists():
        checks_passed += 1
    else:
        failures.append(Finding(display_path(NATIVE_EXE), "M261-E001-DYN-EXE", "missing native compiler executable"))
    checks_total += 1
    if FIXTURE.exists():
        checks_passed += 1
    else:
        failures.append(Finding(display_path(FIXTURE), "M261-E001-DYN-FIX", "missing byref runtime fixture"))

    if NATIVE_EXE.exists() and FIXTURE.exists():
        out_dir = PROBE_ROOT / "byref-gate"
        compile_result = compile_fixture(FIXTURE, out_dir)
        dynamic["compile_rc"] = compile_result.returncode
        checks_total += 1
        if compile_result.returncode == 0:
            checks_passed += 1
            ll_path = out_dir / "module.ll"
            obj_path = out_dir / "module.obj"
            backend_path = out_dir / "module.object-backend.txt"
            ll_text = read_text(ll_path) if ll_path.exists() else ""
            backend = backend_path.read_text(encoding="utf-8").strip() if backend_path.exists() else ""
            for check_id, condition, detail in (
                ("M261-E001-DYN-LL", ll_path.exists(), "missing emitted LLVM IR"),
                ("M261-E001-DYN-OBJ", obj_path.exists(), "missing emitted object"),
                ("M261-E001-DYN-BACKEND", backend == "llvm-direct", f"expected llvm-direct backend, got {backend!r}"),
                ("M261-E001-DYN-BOUNDARY", BOUNDARY_PREFIX + "contract=" + CONTRACT_ID in ll_text, "missing runnable block gate boundary line in emitted IR"),
                ("M261-E001-DYN-META", NAMED_METADATA_LINE in ll_text, "missing runnable block gate named metadata in emitted IR"),
                (
                    "M261-E001-DYN-UPSTREAM",
                    "runtime_block_byref_forwarding_heap_promotion_ownership_interop = contract=" + M261_D003_CONTRACT_ID in ll_text,
                    "missing D003 runtime proof summary in emitted IR",
                ),
            ):
                checks_total += 1
                checks_passed += require(condition, display_path(out_dir), check_id, detail, failures)
        else:
            failures.append(
                Finding(
                    display_path(FIXTURE),
                    "M261-E001-DYN-COMPILE",
                    f"fixture compile failed with rc={compile_result.returncode}: {(compile_result.stderr or compile_result.stdout).strip()}",
                )
            )

    summary = {
        "ok": not failures,
        "mode": MODE,
        "issue": "M261-E001",
        "contract_id": CONTRACT_ID,
        "evidence_model": EVIDENCE_MODEL,
        "active_model": ACTIVE_MODEL,
        "fail_closed_model": FAIL_CLOSED_MODEL,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": True,
        "dynamic": dynamic,
        "upstream_summaries": {
            "M261-A003": {"path": display_path(A003_SUMMARY), "contract_id": M261_A003_CONTRACT_ID},
            "M261-B003": {"path": display_path(B003_SUMMARY), "contract_id": M261_B003_CONTRACT_ID},
            "M261-C004": {"path": display_path(C004_SUMMARY), "contract_id": M261_C004_CONTRACT_ID},
            "M261-D003": {"path": display_path(D003_SUMMARY), "contract_id": M261_D003_CONTRACT_ID},
        },
        "failures": [
            {"artifact": failure.artifact, "check_id": failure.check_id, "detail": failure.detail}
            for failure in failures
        ],
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for failure in failures:
            print(f"[{failure.check_id}] {failure.artifact}: {failure.detail}", file=sys.stderr)
        return 1
    print(SUMMARY_OUT)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
