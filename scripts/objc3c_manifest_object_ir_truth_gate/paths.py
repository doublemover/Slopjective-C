from __future__ import annotations

from pathlib import Path

from objc3c_tooling.artifact_identity import current_host_artifact_identity

ROOT = Path(__file__).resolve().parents[2]
ARTIFACT_IDENTITY = current_host_artifact_identity()
REPORT_DIR = ROOT / "reports" / "claimability" / "manifest-object-ir-truth-gate"
JSON_OUT = REPORT_DIR / "manifest_object_ir_truth_gate_summary.json"
MD_OUT = REPORT_DIR / "manifest_object_ir_truth_gate_summary.md"

ISSUE = "#8018"
CONTRACT_ID = "objc3c.manifest.object.ir.truth.gate.v1"
COMPILER = ROOT / ARTIFACT_IDENTITY.native_executable_relative_path
SCRATCH = ROOT / "tmp" / "artifacts" / "objc3c-native" / "manifest-object-ir-truth-gate"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "dispatch" / "parser_container_inherited_ivar_layout.objc3"
NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_parser_container_ivar_layout_cycle.objc3"

LOWERING_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMANTIC_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
STATIC_ANALYSIS = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_static_analysis.cpp"
CONFORMANCE_MANIFEST = ROOT / "tests" / "conformance" / "lowering_abi" / "manifest.json"
CONFORMANCE_README = ROOT / "tests" / "conformance" / "lowering_abi" / "README.md"
CONFORMANCE_POSITIVE = ROOT / "tests" / "conformance" / "lowering_abi" / "TRUTH-8018-01.json"
CONFORMANCE_NEGATIVE = ROOT / "tests" / "conformance" / "lowering_abi" / "TRUTH-8018-02.json"
SUPPORT_CLASSIFICATION = ROOT / "reports" / "claimability" / "support-classification" / "support_classification_summary.json"
PUBLIC_CLAIM_DRIFT = ROOT / "reports" / "claimability" / "public-claim-drift" / "public_claim_drift_summary.json"
DASHBOARD_BLOCKERS = ROOT / "reports" / "claimability" / "dashboard-release-blockers" / "dashboard_release_blocker_contract_summary.json"

REQUIRED_ARTIFACTS = [
    "module.manifest.json",
    "module.ll",
    "module.obj",
    "module.runtime-registration-descriptor.json",
    "module.runtime-registration-manifest.json",
    "module.runtime-metadata.bin",
    "module.runtime-metadata-discovery.json",
    "module.runtime-metadata-linker-options.rsp",
    "module.objc3-conformance-report.json",
    "module.objc3-conformance-publication.json",
    "module.objc3-advanced-feature-gate.json",
    "module.objc3-release-candidate-matrix.json",
]

DETERMINISTIC_ARTIFACTS = [
    "module.manifest.json",
    "module.ll",
    "module.obj",
    "module.runtime-registration-descriptor.json",
    "module.runtime-registration-manifest.json",
    "module.runtime-metadata.bin",
    "module.runtime-metadata-discovery.json",
    "module.runtime-metadata-linker-options.rsp",
    "module.objc3-conformance-report.json",
    "module.objc3-conformance-publication.json",
]

REQUIRED_IR_TOKENS = [
    "manifest_object_ir_truth_gate = contract=objc3c.manifest.object.ir.truth.gate.v1",
    "runtime_metadata_emission_gate = contract=objc3c.runtime.metadata.emission.gate.v1",
    "runtime_metadata_object_emission_closeout = contract=objc3c.runtime.cross.lane.object.emission.closeout.v1",
    "versioned_conformance_report_lowering = contract=objc3c.versioned.conformance.report.lowering.v1",
    "runtime_capability_reporting = contract=objc3c.runtime.capability.reporting.v1",
    "runtime_bootstrap_ctor_init_emission = contract=objc3c.runtime.constructor.init.stub.emission.v1",
    "runtime_registration_table_image_local_initialization = contract=objc3c.runtime.registration.table.image.local.initialization.v1",
    "manifest_artifact=module.manifest.json",
    "object_artifact=module.obj",
    "conformance_report_artifact=module.objc3-conformance-report.json",
]

REQUIRED_MANIFEST_KEYS = [
    "runtime_metadata_source_records",
    "runtime_bootstrap_lowering_registration_artifact_surface",
    "runtime_realization_lowering_reflection_artifact_surface",
    "runtime_release_candidate_claim_abi_surface",
    "runtime_block_arc_runtime_abi_surface",
    "runtime_unified_concurrency_runtime_abi_surface",
    "lowering_error_handling_result_and_bridging_artifact_replay",
    "interfaces",
]

REQUIRED_OBJECT_SECTIONS = [
    "objc3.runtime.image_info",
    "objc3.runtime.class_descriptors",
    "objc3.runtime.protocol_descriptors",
    "objc3.runtime.category_descriptors",
    "objc3.runtime.property_descriptors",
    "objc3.runtime.ivar_descriptors",
    "objc3.runtime.selector_pool",
    "objc3.runtime.string_pool",
    "objc3.runtime.discovery_root",
    "objc3.runtime.linker_anchor",
    "objc3.runtime.image_root",
    "objc3.runtime.registration_descriptor",
]

REQUIRED_OBJECT_SYMBOLS = [
    "__objc3_image_info",
    "__objc3_sec_class_descriptors",
    "__objc3_sec_protocol_descriptors",
    "__objc3_sec_category_descriptors",
    "__objc3_sec_property_descriptors",
    "__objc3_sec_ivar_descriptors",
    "objc3_runtime_metadata_discovery_root_",
    "objc3_runtime_metadata_link_anchor_",
    "__objc3_runtime_registration_table_",
    "__objc3_runtime_image_root_",
    "__objc3_runtime_registration_descriptor_",
    "objc3_runtime_stage_registration_table_for_bootstrap",
    "objc3_runtime_register_image",
]


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()
