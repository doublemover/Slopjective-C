"""Assertions for block/ARC storage automation acceptance cases."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.block_arc_automation_artifacts import (
    BlockArcAutomationArtifacts,
    ManifestSurface,
)
from objc3c_runtime_acceptance.expectation_matching import expect

from ..runtime_contract_block_arc import (
    RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID,
)


def assert_block_arc_automation_artifacts(
    artifacts: BlockArcAutomationArtifacts,
) -> None:
    _assert_owned_capture_surfaces(artifacts)
    _assert_nonowning_capture_surfaces(artifacts)
    _assert_arc_mode_surfaces(artifacts)
    _assert_arc_inference_surfaces(artifacts)
    _assert_arc_cleanup_surfaces(artifacts)
    _assert_arc_autorelease_return_surfaces(artifacts)
    _assert_arc_method_family_surfaces(artifacts)
    _assert_arc_autoreleasepool_destruction_order_surfaces(artifacts)
    _assert_arc_weak_autoreleasepool_surfaces(artifacts)


def _assert_owned_capture_surfaces(artifacts: BlockArcAutomationArtifacts) -> None:
    expect(
        artifacts.owned_manifest.get("runtime_block_arc_unified_source_surface", {}).get(
            "contract_id"
        )
        == RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
        "expected owned-capture helper fixture to publish the block/ARC unified source surface",
    )
    expect(
        artifacts.owned_manifest.get(
            "runtime_ownership_transfer_capture_family_source_surface", {}
        ).get("contract_id")
        == RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID,
        "expected owned-capture helper fixture to publish the ownership-transfer/capture-family source surface",
    )
    _expect_count_pair(
        artifacts.owned_copy_dispose_surface,
        "copy_helper_required_sites",
        "dispose_helper_required_sites",
        1,
        "expected owned object capture fixture to require copy/dispose helpers",
    )
    _expect_count_pair(
        artifacts.owned_copy_dispose_surface,
        "copy_helper_symbolized_sites",
        "dispose_helper_symbolized_sites",
        1,
        "expected owned object capture fixture to symbolize copy/dispose helpers",
    )
    _expect_count_pair(
        artifacts.owned_escape_surface,
        "escape_to_heap_sites",
        "escape_analysis_enabled_sites",
        1,
        "expected owned object capture fixture to publish one escaping block path",
    )
    expect(
        "; runtime_block_allocation_copy_dispose_invoke_support = "
        "contract=objc3c.runtime.block.allocation.copy.dispose.invoke.support.v1"
        in artifacts.owned_ll,
        "expected owned object capture fixture LLVM IR to publish the block allocation/copy/dispose/invoke support surface",
    )
    expect(
        "@__objc3_block_desc_" in artifacts.owned_ll
        and " = internal constant { i64, i64, i32, i32, i32, ptr }"
        in artifacts.owned_ll
        and ", ptr @__objc3_block_invoke_" in artifacts.owned_ll,
        "expected owned object capture fixture LLVM IR to emit concrete block descriptors that own invoke thunks",
    )
    expect(
        "getelementptr inbounds { i64, i64, i32, i32, i32, ptr }"
        in artifacts.owned_ll,
        "expected owned object capture fixture LLVM IR to load invoke thunks through block descriptors",
    )


def _assert_nonowning_capture_surfaces(artifacts: BlockArcAutomationArtifacts) -> None:
    _expect_count_pair(
        artifacts.nonowning_copy_dispose_surface,
        "copy_helper_required_sites",
        "dispose_helper_required_sites",
        0,
        "expected non-owning object capture fixture to elide copy/dispose helpers",
    )
    _expect_count_pair(
        artifacts.nonowning_copy_dispose_surface,
        "copy_helper_symbolized_sites",
        "dispose_helper_symbolized_sites",
        0,
        "expected non-owning object capture fixture to publish zero helper symbols",
    )
    expect(
        artifacts.nonowning_arc_diagnostics_surface.get(
            "ownership_arc_diagnostic_candidate_sites"
        )
        == 1
        and artifacts.nonowning_arc_diagnostics_surface.get(
            "ownership_arc_fixit_available_sites"
        )
        == 1
        and artifacts.nonowning_arc_diagnostics_surface.get(
            "ownership_arc_profiled_sites"
        )
        == 1,
        "expected non-owning object capture fixture to publish one ARC ownership diagnostic/fixit candidate",
    )
    expect(
        "; block_copy_dispose_lowering = " in artifacts.nonowning_ll,
        "expected non-owning object capture fixture LLVM IR to publish the block copy/dispose lowering summary",
    )


def _assert_arc_mode_surfaces(artifacts: BlockArcAutomationArtifacts) -> None:
    _expect_count_pair(
        artifacts.arc_mode_sema,
        "retain_release_operation_lowering_retain_insertion_sites",
        "retain_release_operation_lowering_release_insertion_sites",
        8,
        "expected arc mode handling fixture to publish eight retain and eight release insertions",
    )
    _expect_count_pair(
        artifacts.arc_mode_block_copy_dispose_surface,
        "copy_helper_required_sites",
        "dispose_helper_required_sites",
        1,
        "expected arc mode handling fixture to keep block copy/dispose helper lowering enabled",
    )
    expect(
        "; arc_cleanup_weak_lifetime_hooks = "
        "contract=objc3c.arc.cleanup.weak.lifetime.hooks.v1" in artifacts.arc_mode_ll,
        "expected arc mode handling fixture LLVM IR to publish the ARC cleanup/weak lifetime hooks surface",
    )


def _assert_arc_inference_surfaces(artifacts: BlockArcAutomationArtifacts) -> None:
    expect(
        artifacts.arc_inference_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 8
        and artifacts.arc_inference_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 8
        and artifacts.arc_inference_sema.get(
            "retain_release_operation_lowering_autorelease_insertion_sites"
        )
        == 0,
        "expected arc inference fixture to publish canonical retain/release insertion counts without autorelease insertion",
    )
    expect(
        artifacts.arc_inference_sema.get(
            "weak_unowned_semantics_lowering_ownership_candidate_sites"
        )
        == 8
        and artifacts.arc_inference_sema.get(
            "weak_unowned_semantics_lowering_weak_reference_sites"
        )
        == 0,
        "expected arc inference fixture to normalize eight ownership-qualified candidates without weak-reference lowering",
    )
    expect(
        "; arc_cleanup_weak_lifetime_hooks = "
        "contract=objc3c.arc.cleanup.weak.lifetime.hooks.v1"
        in artifacts.arc_inference_ll,
        "expected arc inference fixture LLVM IR to publish the ARC cleanup/weak lifetime hooks surface",
    )


def _assert_arc_cleanup_surfaces(artifacts: BlockArcAutomationArtifacts) -> None:
    _expect_arc_cleanup_counts(
        artifacts.arc_cleanup_scope_sema,
        "expected ARC cleanup scope fixture to publish one retain/release transfer pair without autorelease insertion",
    )
    _expect_arc_cleanup_counts(
        artifacts.arc_implicit_cleanup_sema,
        "expected ARC implicit cleanup fixture to publish one retain/release cleanup pair without autorelease insertion",
    )


def _assert_arc_autorelease_return_surfaces(
    artifacts: BlockArcAutomationArtifacts,
) -> None:
    expect(
        artifacts.arc_autorelease_return_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 0
        and artifacts.arc_autorelease_return_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 0
        and artifacts.arc_autorelease_return_sema.get(
            "retain_release_operation_lowering_autorelease_insertion_sites"
        )
        == 2,
        "expected ARC autorelease-return fixture to publish two autorelease insertions without retain/release insertion",
    )
    expect(
        "; arc_block_autorelease_return_lowering = "
        in artifacts.arc_autorelease_return_ll,
        "expected ARC autorelease-return fixture LLVM IR to publish the ARC block/autorelease-return lowering summary",
    )


def _assert_arc_method_family_surfaces(
    artifacts: BlockArcAutomationArtifacts,
) -> None:
    expect(
        artifacts.arc_method_family_sema.get(
            "super_dispatch_method_family_alloc_sites"
        )
        == 2
        and artifacts.arc_method_family_sema.get(
            "super_dispatch_method_family_new_sites"
        )
        == 3
        and artifacts.arc_method_family_sema.get(
            "super_dispatch_method_family_init_sites"
        )
        == 2
        and artifacts.arc_method_family_sema.get(
            "super_dispatch_method_family_copy_sites"
        )
        == 2
        and artifacts.arc_method_family_sema.get(
            "super_dispatch_method_family_mutable_copy_sites"
        )
        == 1
        and artifacts.arc_method_family_sema.get(
            "super_dispatch_method_family_none_sites"
        )
        == 4,
        "expected ARC method-family fixture to classify alloc/new/init/copy/mutableCopy and near-miss selectors deterministically",
    )
    expect(
        artifacts.arc_method_family_sema.get(
            "super_dispatch_method_family_returns_retained_result_sites"
        )
        == 10
        and artifacts.arc_method_family_sema.get(
            "super_dispatch_method_family_returns_related_result_sites"
        )
        == 2
        and artifacts.arc_method_family_sema.get(
            "super_dispatch_method_family_contract_violation_sites"
        )
        == 0,
        "expected ARC method-family fixture to publish retained/related result counts without contract violations",
    )
    expect(
        artifacts.arc_method_family_ll.count(
            "objc3_arc_method_family_retained_result_cleanup = alloc"
        )
        == 2
        and artifacts.arc_method_family_ll.count(
            "objc3_arc_method_family_retained_result_cleanup = new"
        )
        == 3
        and artifacts.arc_method_family_ll.count(
            "objc3_arc_method_family_retained_result_cleanup = init"
        )
        == 2
        and artifacts.arc_method_family_ll.count(
            "objc3_arc_method_family_retained_result_cleanup = copy"
        )
        == 2
        and artifacts.arc_method_family_ll.count(
            "objc3_arc_method_family_retained_result_cleanup = mutableCopy"
        )
        == 1,
        "expected ARC method-family fixture LLVM IR to mark exact retained-result cleanup counts for every retained family",
    )
    expect(
        "objc3_arc_method_family_related_result_consumes_receiver_cleanup = init"
        in artifacts.arc_method_family_ll,
        "expected ARC method-family init chain to consume the pending owned receiver cleanup",
    )
    expect(
        artifacts.arc_method_family_ll.count(
            "store i32 0, ptr %objc3.arc.methodfamily.result.addr."
        )
        >= 10,
        "expected ARC method-family cleanup slots to be initialized before branch-local stores",
    )
    expect(
        artifacts.arc_method_family_ll.count("objc3_runtime_release_i32") >= 10,
        "expected ARC method-family retained message results to lower to release helper traffic",
    )


def _assert_arc_autoreleasepool_destruction_order_surfaces(
    artifacts: BlockArcAutomationArtifacts,
) -> None:
    expect(
        artifacts.arc_autoreleasepool_order_sema.get(
            "autoreleasepool_scope_lowering_scope_sites"
        )
        == 1
        and artifacts.arc_autoreleasepool_order_sema.get(
            "autoreleasepool_scope_lowering_scope_entry_transition_sites"
        )
        == 1
        and artifacts.arc_autoreleasepool_order_sema.get(
            "autoreleasepool_scope_lowering_scope_exit_transition_sites"
        )
        == 1,
        "expected destruction-order fixture to publish one deterministic autoreleasepool scope",
    )
    ll = artifacts.arc_autoreleasepool_order_ll
    push_index = ll.find("call void @objc3_runtime_push_autoreleasepool_scope")
    release_index = ll.find("call i32 @objc3_runtime_release_i32")
    pop_index = ll.find("call void @objc3_runtime_pop_autoreleasepool_scope")
    expect(
        push_index >= 0 and release_index >= 0 and pop_index >= 0,
        "expected destruction-order fixture LLVM IR to contain push, ARC release, and pop helper calls",
    )
    expect(
        push_index < release_index < pop_index,
        "expected terminal ARC cleanup to release owned storage before draining the autoreleasepool",
    )


def _assert_arc_weak_autoreleasepool_surfaces(
    artifacts: BlockArcAutomationArtifacts,
) -> None:
    expect(
        artifacts.arc_weak_autoreleasepool_sema.get(
            "autoreleasepool_scope_lowering_scope_sites"
        )
        == 2
        and artifacts.arc_weak_autoreleasepool_sema.get(
            "autoreleasepool_scope_lowering_scope_entry_transition_sites"
        )
        == 2
        and artifacts.arc_weak_autoreleasepool_sema.get(
            "autoreleasepool_scope_lowering_scope_exit_transition_sites"
        )
        == 2
        and artifacts.arc_weak_autoreleasepool_sema.get(
            "autoreleasepool_scope_lowering_max_scope_depth"
        )
        == 2,
        "expected weak/autoreleasepool fixture to publish two nested autoreleasepool scopes",
    )
    ll = artifacts.arc_weak_autoreleasepool_ll
    for helper in (
        "objc3_runtime_store_weak_current_property_i32",
        "objc3_runtime_load_weak_current_property_i32",
        "objc3_runtime_push_autoreleasepool_scope",
        "objc3_runtime_pop_autoreleasepool_scope",
    ):
        expect(
            helper in ll,
            f"expected weak/autoreleasepool fixture LLVM IR to reference {helper}",
        )


def _expect_arc_cleanup_counts(surface: ManifestSurface, message: str) -> None:
    expect(
        surface.get("retain_release_operation_lowering_retain_insertion_sites") == 1
        and surface.get("retain_release_operation_lowering_release_insertion_sites")
        == 1
        and surface.get(
            "retain_release_operation_lowering_autorelease_insertion_sites"
        )
        == 0,
        message,
    )


def _expect_count_pair(
    surface: ManifestSurface,
    first_key: str,
    second_key: str,
    expected: int,
    message: str,
) -> None:
    expect(
        surface.get(first_key) == expected and surface.get(second_key) == expected,
        message,
    )


__all__ = ["assert_block_arc_automation_artifacts"]
