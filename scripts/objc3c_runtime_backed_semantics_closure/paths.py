from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REPORT_DIR = ROOT / "tmp" / "reports" / "claimability" / "runtime-backed-semantics-closure"
JSON_OUT = REPORT_DIR / "runtime_backed_semantics_closure_summary.json"
MD_OUT = REPORT_DIR / "runtime_backed_semantics_closure_summary.md"

CONTRACT_ID = "objc3c.runtime.backed.semantics.closure.v1"
ISSUE = "#8017"
COMPILER = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
SCRATCH = ROOT / "tmp" / "artifacts" / "objc3c-native" / "runtime-backed-semantics-closure"

LOWERING_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "contracts" / "block_runtime_lowering_semantics_closure_summary.inc"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMANTIC_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
STATIC_ANALYSIS = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_static_analysis.cpp"
RUNTIME = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
STRESS_MANIFEST = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "lowering_runtime_stress_manifest.json"
CONFORMANCE_MANIFEST = ROOT / "tests" / "conformance" / "lowering_abi" / "manifest.json"
CONFORMANCE_README = ROOT / "tests" / "conformance" / "lowering_abi" / "README.md"
CONFORMANCE_POSITIVE = ROOT / "tests" / "conformance" / "lowering_abi" / "RTBACK-8017-01.json"
CONFORMANCE_NEGATIVE = ROOT / "tests" / "conformance" / "lowering_abi" / "RTBACK-8017-02.json"
DURABLE_REPLAY_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "objc3c"


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")
