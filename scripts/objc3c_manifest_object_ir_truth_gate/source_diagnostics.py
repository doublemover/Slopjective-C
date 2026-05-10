from __future__ import annotations

from .manifest_loading import read, read_json
from .paths import (
    CONFORMANCE_MANIFEST,
    CONFORMANCE_NEGATIVE,
    CONFORMANCE_POSITIVE,
    CONFORMANCE_README,
    IR_EMITTER,
    LOWERING_CONTRACT_CPP,
    LOWERING_CONTRACT_H,
    SEMA_PASS_MANAGER,
    SEMANTIC_PASSES,
    STATIC_ANALYSIS,
    rel,
)


def check_source_tokens() -> dict[str, bool]:
    source_checks = {
        "contract_header": [
            (LOWERING_CONTRACT_H, "kObjc3ManifestObjectIrTruthGateContractId"),
            (LOWERING_CONTRACT_H, "kObjc3ManifestObjectIrTruthGateManifestModel"),
            (LOWERING_CONTRACT_H, "kObjc3ManifestObjectIrTruthGateObjectModel"),
            (LOWERING_CONTRACT_H, "kObjc3ManifestObjectIrTruthGateClaimModel"),
            (LOWERING_CONTRACT_H, "Objc3ManifestObjectIrTruthGateSummary"),
        ],
        "contract_cpp": [
            (LOWERING_CONTRACT_CPP, "Objc3ManifestObjectIrTruthGateSummary()"),
            (LOWERING_CONTRACT_CPP, "module.manifest.json,module.ll,module.obj"),
            (LOWERING_CONTRACT_CPP, "kObjc3ManifestObjectIrTruthGateFailureModel"),
        ],
        "ir_emitter": [
            (IR_EMITTER, "manifest_object_ir_truth_gate"),
            (IR_EMITTER, "Objc3ManifestObjectIrTruthGateSummary()"),
            (IR_EMITTER, "runtime_metadata_object_emission_closeout"),
        ],
        "sema_pipeline": [
            (SEMA_PASS_MANAGER, "SemaPassManager"),
            (SEMANTIC_PASSES, "ResolveGlobalInitializerValues"),
            (STATIC_ANALYSIS, "BlockAlwaysReturns"),
        ],
    }
    result: dict[str, bool] = {}
    for group, checks in source_checks.items():
        for path, token in checks:
            result[f"{group}:{rel(path)}::{token}"] = token in read(path)
    return result


def check_conformance() -> dict[str, bool]:
    manifest = read_json(CONFORMANCE_MANIFEST)
    files = {"TRUTH-8018-01.json", "TRUTH-8018-02.json"}
    readme = read(CONFORMANCE_README)
    return {
        "positive_fixture_exists": CONFORMANCE_POSITIVE.is_file(),
        "negative_fixture_exists": CONFORMANCE_NEGATIVE.is_file(),
        "manifest_references_truth": any(
            set(group.get("files", [])) >= files
            for group in manifest.get("groups", [])
        ),
        "readme_references_truth": "TRUTH-8018-01.json" in readme
        and "TRUTH-8018-02.json" in readme,
    }
