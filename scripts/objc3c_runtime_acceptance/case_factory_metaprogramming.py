"""Metaprogramming runtime acceptance case factories."""

from __future__ import annotations

from objc3c_runtime_acceptance.case_factory_types import CaseFactoryContext
from objc3c_runtime_acceptance.case_factory_types import LabeledCaseFactories


def build_metaprogramming_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "metaprogramming-source-surface",
            lambda: domains.metaprogramming.check_metaprogramming_source_surface_case(
                run_dir
            ),
        ),
        (
            "metaprogramming-package-provenance-source-surface",
            lambda: domains.metaprogramming.check_metaprogramming_package_provenance_source_surface_case(
                run_dir
            ),
        ),
        (
            "metaprogramming-semantics",
            lambda: domains.metaprogramming.check_metaprogramming_semantics_case(
                run_dir
            ),
        ),
        (
            "metaprogramming-derive-property-behavior-semantics",
            lambda: domains.metaprogramming.check_metaprogramming_derive_property_behavior_semantics_case(
                run_dir
            ),
        ),
        (
            "metaprogramming-macro-safety-cache-diagnostics",
            lambda: domains.metaprogramming.check_metaprogramming_macro_safety_cache_diagnostics_case(
                run_dir
            ),
        ),
        (
            "metaprogramming-lowering-host-cache-surface",
            lambda: domains.metaprogramming.check_metaprogramming_lowering_host_cache_surface_case(
                run_dir
            ),
        ),
        (
            "metaprogramming-executable-lowering",
            lambda: domains.metaprogramming.check_metaprogramming_executable_lowering_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "cross-module-metaprogramming-artifact-preservation",
            lambda: domains.metaprogramming.check_cross_module_metaprogramming_artifact_preservation_case(
                run_dir
            ),
        ),
        (
            "metaprogramming-runtime-abi-cache-surface",
            lambda: domains.metaprogramming.check_metaprogramming_runtime_abi_cache_surface_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "live-metaprogramming-cache-runtime-integration",
            lambda: domains.metaprogramming.check_live_metaprogramming_cache_runtime_integration_case(
                clangxx,
                run_dir,
            ),
        ),
    ]


__all__ = ["build_metaprogramming_case_factories"]
