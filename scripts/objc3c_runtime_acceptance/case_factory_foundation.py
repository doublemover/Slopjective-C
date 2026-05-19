"""Foundation runtime acceptance case factory sections."""

from __future__ import annotations

from objc3c_runtime_acceptance.case_factory_types import CaseFactoryContext
from objc3c_runtime_acceptance.case_factory_types import LabeledCaseFactories
from objc3c_runtime_acceptance.domains.probe_helpers import (
    check_runtime_probe_helper_support_case,
)


def build_core_case_factories(context: CaseFactoryContext) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "runtime-library",
            lambda: domains.object_model.check_runtime_library_case(clangxx, run_dir),
        ),
        (
            "dispatch-lookup-runtime-probe",
            lambda: domains.object_model.check_dispatch_lookup_runtime_probe_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "method-cache-slow-path-probe",
            lambda: domains.object_model.check_method_cache_slow_path_probe_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "typed-dispatch-abi-probe",
            lambda: domains.object_model.check_typed_dispatch_abi_probe_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "runtime-probe-helper-support",
            lambda: check_runtime_probe_helper_support_case(clangxx, run_dir),
        ),
        (
            "compile-backend-parity",
            lambda: domains.compiler_artifacts.check_compile_backend_parity_case(
                run_dir
            ),
        ),
        (
            "artifact-registry-key-isolation",
            lambda: domains.compiler_artifacts.check_artifact_registry_key_isolation_case(
                run_dir
            ),
        ),
    ]


def build_registration_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "installation-lifecycle",
            lambda: domains.registration.check_installation_lifecycle_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "multi-image-registration-reset-replay",
            lambda: domains.registration.check_multi_image_registration_reset_replay_case(
                clangxx,
                run_dir,
            ),
        ),
    ]


def build_cross_module_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    run_dir = context.run_dir
    return [
        (
            "cross-module-block-ownership-artifact-preservation",
            lambda: domains.block_arc.check_cross_module_block_ownership_artifact_preservation_case(
                run_dir
            ),
        ),
        (
            "cross-module-storage-reflection-artifact-preservation",
            lambda: domains.storage_reflection.check_cross_module_storage_reflection_artifact_preservation_case(
                run_dir
            ),
        ),
    ]


__all__ = [
    "build_core_case_factories",
    "build_cross_module_case_factories",
    "build_registration_case_factories",
]
