from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REPORT_DIR = ROOT / "reports" / "claimability" / "effects-ownership-semantic-model"
JSON_OUT = REPORT_DIR / "effects_ownership_semantic_model_summary.json"
MD_OUT = REPORT_DIR / "effects_ownership_semantic_model_summary.md"
TMP_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-native" / "effects-ownership-semantic-model"

COMPILER = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "effects_ownership_semantic_model_positive.objc3"
NEGATIVE_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "recovery"
    / "negative"
    / "negative_effects_ownership_async_throws.objc3"
)
SEMANTIC_MANIFEST = ROOT / "tests" / "conformance" / "semantic" / "manifest.json"
SEMANTIC_README = ROOT / "tests" / "conformance" / "semantic" / "README.md"
CONFORMANCE_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "EFF-8014-01.json"
CONFORMANCE_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "EFF-8014-02.json"
STRESS_MANIFEST = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "lowering_runtime_stress_manifest.json"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMANTIC_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMANTIC_PASSES_H = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "artifacts" / "objc3_frontend_artifacts.cpp"
LOWERING_CONTRACT = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
IR_EMITTER_H = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")
