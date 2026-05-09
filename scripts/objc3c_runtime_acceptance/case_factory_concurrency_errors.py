"""Concurrency and error runtime acceptance case factories."""

from __future__ import annotations

from objc3c_runtime_acceptance.case_factory_types import CaseFactoryContext
from objc3c_runtime_acceptance.case_factory_types import LabeledCaseFactories


def build_concurrency_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "unified-concurrency-runtime-architecture",
            lambda: domains.concurrency.check_unified_concurrency_runtime_architecture_case(
                run_dir
            ),
        ),
        (
            "async-task-actor-normalization-completion",
            lambda: domains.concurrency.check_async_task_actor_normalization_completion_case(
                run_dir
            ),
        ),
        (
            "unified-concurrency-lowering-metadata-surface",
            lambda: domains.concurrency.check_unified_concurrency_lowering_metadata_surface_case(
                run_dir
            ),
        ),
        (
            "unified-concurrency-runtime-abi",
            lambda: domains.concurrency.check_unified_concurrency_runtime_abi_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "live-unified-concurrency-runtime-implementation",
            lambda: domains.concurrency.check_live_unified_concurrency_runtime_implementation_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "cross-module-concurrency-actor-artifact-preservation",
            lambda: domains.concurrency.check_cross_module_concurrency_actor_artifact_preservation_case(
                run_dir
            ),
        ),
    ]


def build_error_case_factories(context: CaseFactoryContext) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "error-execution-cleanup-source",
            lambda: domains.errors.check_error_execution_cleanup_source_case(run_dir),
        ),
        (
            "catch-filter-finalization-source",
            lambda: domains.errors.check_catch_filter_finalization_source_case(run_dir),
        ),
        (
            "error-propagation-cleanup-semantics",
            lambda: domains.errors.check_error_propagation_cleanup_semantics_case(
                run_dir
            ),
        ),
        (
            "executable-try-throw-do-catch-semantics",
            lambda: domains.errors.check_executable_try_throw_do_catch_semantics_case(
                run_dir
            ),
        ),
        (
            "bridging-filter-unwind-compatibility-diagnostics",
            lambda: domains.errors.check_bridging_filter_unwind_compatibility_diagnostics_case(
                run_dir
            ),
        ),
        (
            "error-lowering-unwind-bridge-helper-surface",
            lambda: domains.errors.check_error_lowering_unwind_bridge_helper_surface_case(
                run_dir
            ),
        ),
        (
            "executable-throw-catch-cleanup-lowering",
            lambda: domains.errors.check_executable_throw_catch_cleanup_lowering_case(
                run_dir
            ),
        ),
        (
            "cross-module-error-metadata-replay-preservation",
            lambda: domains.errors.check_cross_module_error_metadata_replay_preservation_case(
                run_dir
            ),
        ),
        (
            "error-runtime-abi-cleanup",
            lambda: domains.errors.check_error_runtime_abi_cleanup_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "live-error-runtime-integration",
            lambda: domains.errors.check_live_error_runtime_integration_case(
                clangxx,
                run_dir,
            ),
        ),
    ]


__all__ = ["build_concurrency_case_factories", "build_error_case_factories"]
