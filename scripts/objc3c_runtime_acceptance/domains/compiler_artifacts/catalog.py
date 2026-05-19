"""Compiler artifact acceptance catalog data."""

from __future__ import annotations

from objc3c_runtime_acceptance.paths import ROOT


COMPILE_BACKEND_PARITY_CASE_ID = "compile-backend-parity"
COMPILE_BACKEND_PARITY_PROBE = "direct-native-compile-plus-wrapper-contract-parity"
COMPILE_BACKEND_PARITY_CLAIM_CLASS = "compile-coupled-inspection"
COMPILE_BACKEND_PARITY_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "synthesized_accessor_property_lowering_positive.objc3"
)
COMPILE_BACKEND_PARITY_DIRECT_DIR_NAME = "direct"
COMPILE_BACKEND_PARITY_WRAPPER_DIR_NAME = "wrapper"
COMPILE_BACKEND_PARITY_CACHE_ROOT_NAME = "metaprogramming-cache-root"
COMPILE_BACKEND_PARITY_TRUTHFULNESS_FIELDS = [
    "runtime_dispatch_symbol",
    "runtime_dispatch_declaration_count",
    "runtime_dispatch_call_count",
    "property_descriptor_count_expected",
    "property_descriptor_definition_count",
    "property_descriptor_section_present",
    "ivar_descriptor_count_expected",
    "ivar_descriptor_definition_count",
    "ivar_descriptor_section_present",
    "property_synthesis_sites_expected",
    "synthesized_accessor_definition_count",
    "current_property_helper_call_count",
    "property_descriptor_counts_match",
    "ivar_descriptor_counts_match",
    "synthesized_property_surface_matches",
    "truthful",
]

ARTIFACT_REGISTRY_KEY_ISOLATION_CASE_ID = "artifact-registry-key-isolation"
ARTIFACT_REGISTRY_KEY_ISOLATION_PROBE = (
    "direct-native-artifact-registry-negative-reuse-proof"
)
ARTIFACT_REGISTRY_CLAIM_CLASS = "compile-coupled-inspection"
ARTIFACT_REGISTRY_KEY_ISOLATION_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
)
ARTIFACT_REGISTRY_REUSE_POLICY = "immutable-inspection"
ARTIFACT_REGISTRY_SHARED_EMIT_PREFIX = "module"
ARTIFACT_REGISTRY_IMPORT_SURFACE_A_NAME = "surface-a.runtime-import-surface.json"
ARTIFACT_REGISTRY_IMPORT_SURFACE_B_NAME = "surface-b.runtime-import-surface.json"
ARTIFACT_REGISTRY_NEGATIVE_REUSE_MODEL = (
    "changed bootstrap args and changed import-surface paths produce "
    "different immutable artifact registry keys; only exact key "
    "matches may copy producer artifacts"
)


def bootstrap_ordinal_args(ordinal: str) -> list[str]:
    return ["--objc3-bootstrap-registration-order-ordinal", ordinal]
