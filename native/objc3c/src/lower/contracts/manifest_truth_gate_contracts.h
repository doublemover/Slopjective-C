#pragma once

#include <string>

// manifest/object/IR truth gate anchor: this binds the compiler sidecar
// manifest, emitted LLVM IR, native object, runtime registration artifacts,
// and release-claim sidecars into one deterministic evidence boundary.
inline constexpr const char *kObjc3ManifestObjectIrTruthGateContractId =
    "objc3c.manifest.object.ir.truth.gate.v1";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateEvidenceModel =
    "manifest-ir-object-registration-and-conformance-sidecars-form-one-regenerated-truth-set";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateManifestModel =
    "module.manifest.json-publishes-the-semantic-lowering-runtime-metadata-and-replay-key-source-of-truth";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateIrModel =
    "module.ll-republishes-the-same-contract-boundaries-and-runtime-registration-roots-as-reviewable-ir-evidence";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateObjectModel =
    "host-default-object-artifact-materializes-the-same-objc3-runtime-sections-symbols-and-registration-roots-observed-in-ir";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateClaimModel =
    "versioned-conformance-and-runtime-capability-sidecars-are-bound-to-the-same-replay-key-and-remain-narrower-than-evidence";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateFailureModel =
    "missing-artifact-hash-drift-object-section-drift-or-unsupported-negative-emission-fails-closed";

std::string Objc3ManifestObjectIrTruthGateSummary();
