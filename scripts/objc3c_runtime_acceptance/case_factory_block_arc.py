"""Block ARC case factories."""

from __future__ import annotations

from objc3c_runtime_acceptance.case_factory_types import CaseFactoryContext
from objc3c_runtime_acceptance.case_factory_types import LabeledCaseFactories


def build_block_arc_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "escaping-block-capture-legality",
            lambda: domains.block_arc.check_escaping_block_capture_legality_case(
                run_dir
            ),
        ),
        (
            "block-storage-arc-automation-semantics",
            lambda: domains.block_arc.check_block_storage_arc_automation_semantics_case(
                run_dir
            ),
        ),
        (
            "block-arc-runtime-abi",
            lambda: domains.block_arc.check_block_arc_runtime_abi_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "block-helper-runtime-execution",
            lambda: domains.block_arc.check_block_helper_runtime_execution_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "arc-property-helper",
            lambda: domains.block_arc.check_arc_property_helper_case(
                clangxx,
                run_dir,
            ),
        ),
    ]


__all__ = ["build_block_arc_case_factories"]
