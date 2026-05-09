"""Block/ARC storage automation runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import (
    NegativeDiagnosticExpectation,
    compile_fixture_outputs,
    compile_fixture_with_args,
    compile_negative_diagnostic_batch,
)
from objc3c_runtime_acceptance.paths import ROOT

from ..runtime_contract_block_arc import (
    RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID,
)


def check_block_storage_arc_automation_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "block-storage-arc-automation-semantics"

    owned_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "owned_object_capture_helper_positive.objc3"
    )
    _, owned_ll_path, owned_manifest_path = compile_fixture_outputs(
        owned_fixture, case_dir / "owned-positive"
    )
    owned_manifest = json.loads(owned_manifest_path.read_text(encoding="utf-8"))
    owned_semantic_surface = (
        owned_manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})
    )
    owned_copy_dispose_surface = owned_semantic_surface.get(
        "objc_block_copy_dispose_lowering_surface", {}
    )
    owned_escape_surface = owned_semantic_surface.get(
        "objc_block_storage_escape_lowering_surface", {}
    )
    owned_ll = owned_ll_path.read_text(encoding="utf-8")

    nonowning_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "nonowning_object_capture_helper_elided_positive.objc3"
    )
    _, nonowning_ll_path, nonowning_manifest_path = compile_fixture_outputs(
        nonowning_fixture, case_dir / "nonowning-positive"
    )
    nonowning_manifest = json.loads(
        nonowning_manifest_path.read_text(encoding="utf-8")
    )
    nonowning_semantic_surface = (
        nonowning_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
    )
    nonowning_copy_dispose_surface = nonowning_semantic_surface.get(
        "objc_block_copy_dispose_lowering_surface", {}
    )
    nonowning_arc_diagnostics_surface = nonowning_semantic_surface.get(
        "objc_arc_diagnostics_fixit_lowering_surface", {}
    )
    nonowning_ll = nonowning_ll_path.read_text(encoding="utf-8")

    arc_mode_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_mode_handling_positive.objc3"
    )
    compile_fixture_with_args(
        arc_mode_fixture, case_dir / "arc-mode-positive", extra_args=["-fobjc-arc"]
    )
    arc_mode_ll_path = case_dir / "arc-mode-positive" / "module.ll"
    arc_mode_manifest_path = case_dir / "arc-mode-positive" / "module.manifest.json"
    arc_mode_manifest = json.loads(arc_mode_manifest_path.read_text(encoding="utf-8"))
    arc_mode_ll = arc_mode_ll_path.read_text(encoding="utf-8")
    arc_mode_sema = arc_mode_manifest.get("frontend", {}).get("pipeline", {}).get(
        "sema_pass_manager", {}
    )
    arc_mode_block_copy_dispose_surface = (
        arc_mode_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )

    arc_inference_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_inference_lifetime_positive.objc3"
    )
    compile_fixture_with_args(
        arc_inference_fixture,
        case_dir / "arc-inference-positive",
        extra_args=["-fobjc-arc"],
    )
    arc_inference_ll_path = case_dir / "arc-inference-positive" / "module.ll"
    arc_inference_manifest_path = (
        case_dir / "arc-inference-positive" / "module.manifest.json"
    )
    arc_inference_manifest = json.loads(
        arc_inference_manifest_path.read_text(encoding="utf-8")
    )
    arc_inference_ll = arc_inference_ll_path.read_text(encoding="utf-8")
    arc_inference_sema = arc_inference_manifest.get("frontend", {}).get(
        "pipeline", {}
    ).get("sema_pass_manager", {})

    arc_cleanup_scope_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_cleanup_scope_positive.objc3"
    )
    compile_fixture_with_args(
        arc_cleanup_scope_fixture,
        case_dir / "arc-cleanup-scope-positive",
        extra_args=["-fobjc-arc"],
    )
    arc_cleanup_scope_manifest_path = (
        case_dir / "arc-cleanup-scope-positive" / "module.manifest.json"
    )
    arc_cleanup_scope_manifest = json.loads(
        arc_cleanup_scope_manifest_path.read_text(encoding="utf-8")
    )
    arc_cleanup_scope_sema = arc_cleanup_scope_manifest.get("frontend", {}).get(
        "pipeline", {}
    ).get("sema_pass_manager", {})

    arc_implicit_cleanup_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_implicit_cleanup_void_positive.objc3"
    )
    compile_fixture_with_args(
        arc_implicit_cleanup_fixture,
        case_dir / "arc-implicit-cleanup-positive",
        extra_args=["-fobjc-arc"],
    )
    arc_implicit_cleanup_manifest_path = (
        case_dir / "arc-implicit-cleanup-positive" / "module.manifest.json"
    )
    arc_implicit_cleanup_manifest = json.loads(
        arc_implicit_cleanup_manifest_path.read_text(encoding="utf-8")
    )
    arc_implicit_cleanup_sema = arc_implicit_cleanup_manifest.get(
        "frontend", {}
    ).get("pipeline", {}).get("sema_pass_manager", {})

    arc_autorelease_return_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_autorelease_return_positive.objc3"
    )
    compile_fixture_with_args(
        arc_autorelease_return_fixture,
        case_dir / "arc-autorelease-return-positive",
        extra_args=["-fobjc-arc"],
    )
    arc_autorelease_return_ll_path = (
        case_dir / "arc-autorelease-return-positive" / "module.ll"
    )
    arc_autorelease_return_manifest_path = (
        case_dir / "arc-autorelease-return-positive" / "module.manifest.json"
    )
    arc_autorelease_return_manifest = json.loads(
        arc_autorelease_return_manifest_path.read_text(encoding="utf-8")
    )
    arc_autorelease_return_ll = arc_autorelease_return_ll_path.read_text(
        encoding="utf-8"
    )
    arc_autorelease_return_sema = arc_autorelease_return_manifest.get(
        "frontend", {}
    ).get("pipeline", {}).get("sema_pass_manager", {})

    negative_batch = compile_negative_diagnostic_batch(
        case_id="block-storage-arc-automation-semantics",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key="weak-mutation-negative",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "weak_object_capture_mutation_negative.objc3",
                expected_snippets=[
                    "type mismatch: block mutated capture 'weakValue' requires owned runtime-backed storage"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="unowned-mutation-negative",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "unowned_object_capture_mutation_negative.objc3",
                expected_snippets=[
                    "type mismatch: block mutated capture 'borrowedValue' requires owned runtime-backed storage"
                ],
                expected_codes=["O3S206"],
            ),
        ],
    )
    weak_negative = negative_batch["results"][0]
    unowned_negative = negative_batch["results"][1]

    expect(
        owned_manifest.get("runtime_block_arc_unified_source_surface", {}).get(
            "contract_id"
        )
        == RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
        "expected owned-capture helper fixture to publish the block/ARC unified source surface",
    )
    expect(
        owned_manifest.get(
            "runtime_ownership_transfer_capture_family_source_surface", {}
        ).get("contract_id")
        == RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID,
        "expected owned-capture helper fixture to publish the ownership-transfer/capture-family source surface",
    )
    expect(
        owned_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and owned_copy_dispose_surface.get("dispose_helper_required_sites") == 1,
        "expected owned object capture fixture to require copy/dispose helpers",
    )
    expect(
        owned_copy_dispose_surface.get("copy_helper_symbolized_sites") == 1
        and owned_copy_dispose_surface.get("dispose_helper_symbolized_sites") == 1,
        "expected owned object capture fixture to symbolize copy/dispose helpers",
    )
    expect(
        owned_escape_surface.get("escape_to_heap_sites") == 1
        and owned_escape_surface.get("escape_analysis_enabled_sites") == 1,
        "expected owned object capture fixture to publish one escaping block path",
    )
    expect(
        "; runtime_block_allocation_copy_dispose_invoke_support = "
        "contract=objc3c.runtime.block.allocation.copy.dispose.invoke.support.v1"
        in owned_ll,
        "expected owned object capture fixture LLVM IR to publish the block allocation/copy/dispose/invoke support surface",
    )

    expect(
        nonowning_copy_dispose_surface.get("copy_helper_required_sites") == 0
        and nonowning_copy_dispose_surface.get("dispose_helper_required_sites") == 0,
        "expected non-owning object capture fixture to elide copy/dispose helpers",
    )
    expect(
        nonowning_copy_dispose_surface.get("copy_helper_symbolized_sites") == 0
        and nonowning_copy_dispose_surface.get("dispose_helper_symbolized_sites") == 0,
        "expected non-owning object capture fixture to publish zero helper symbols",
    )
    expect(
        nonowning_arc_diagnostics_surface.get(
            "ownership_arc_diagnostic_candidate_sites"
        )
        == 1
        and nonowning_arc_diagnostics_surface.get("ownership_arc_fixit_available_sites")
        == 1
        and nonowning_arc_diagnostics_surface.get("ownership_arc_profiled_sites")
        == 1,
        "expected non-owning object capture fixture to publish one ARC ownership diagnostic/fixit candidate",
    )
    expect(
        "; block_copy_dispose_lowering = " in nonowning_ll,
        "expected non-owning object capture fixture LLVM IR to publish the block copy/dispose lowering summary",
    )

    expect(
        arc_mode_sema.get("retain_release_operation_lowering_retain_insertion_sites")
        == 8
        and arc_mode_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 8,
        "expected arc mode handling fixture to publish eight retain and eight release insertions",
    )
    expect(
        arc_mode_block_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and arc_mode_block_copy_dispose_surface.get("dispose_helper_required_sites")
        == 1,
        "expected arc mode handling fixture to keep block copy/dispose helper lowering enabled",
    )
    expect(
        "; arc_cleanup_weak_lifetime_hooks = "
        "contract=objc3c.arc.cleanup.weak.lifetime.hooks.v1" in arc_mode_ll,
        "expected arc mode handling fixture LLVM IR to publish the ARC cleanup/weak lifetime hooks surface",
    )

    expect(
        arc_inference_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 8
        and arc_inference_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 8
        and arc_inference_sema.get(
            "retain_release_operation_lowering_autorelease_insertion_sites"
        )
        == 0,
        "expected arc inference fixture to publish canonical retain/release insertion counts without autorelease insertion",
    )
    expect(
        arc_inference_sema.get(
            "weak_unowned_semantics_lowering_ownership_candidate_sites"
        )
        == 8
        and arc_inference_sema.get(
            "weak_unowned_semantics_lowering_weak_reference_sites"
        )
        == 0,
        "expected arc inference fixture to normalize eight ownership-qualified candidates without weak-reference lowering",
    )
    expect(
        "; arc_cleanup_weak_lifetime_hooks = "
        "contract=objc3c.arc.cleanup.weak.lifetime.hooks.v1" in arc_inference_ll,
        "expected arc inference fixture LLVM IR to publish the ARC cleanup/weak lifetime hooks surface",
    )
    expect(
        arc_cleanup_scope_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 1
        and arc_cleanup_scope_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 1
        and arc_cleanup_scope_sema.get(
            "retain_release_operation_lowering_autorelease_insertion_sites"
        )
        == 0,
        "expected ARC cleanup scope fixture to publish one retain/release transfer pair without autorelease insertion",
    )
    expect(
        arc_implicit_cleanup_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 1
        and arc_implicit_cleanup_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 1
        and arc_implicit_cleanup_sema.get(
            "retain_release_operation_lowering_autorelease_insertion_sites"
        )
        == 0,
        "expected ARC implicit cleanup fixture to publish one retain/release cleanup pair without autorelease insertion",
    )
    expect(
        arc_autorelease_return_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 0
        and arc_autorelease_return_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 0
        and arc_autorelease_return_sema.get(
            "retain_release_operation_lowering_autorelease_insertion_sites"
        )
        == 2,
        "expected ARC autorelease-return fixture to publish two autorelease insertions without retain/release insertion",
    )
    expect(
        "; arc_block_autorelease_return_lowering = " in arc_autorelease_return_ll,
        "expected ARC autorelease-return fixture LLVM IR to publish the ARC block/autorelease-return lowering summary",
    )

    return CaseResult(
        case_id="block-storage-arc-automation-semantics",
        probe="compile-manifest-diagnostics-and-llvm-ir",
        fixture="tests/tooling/fixtures/native/owned_object_capture_helper_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "owned_copy_helper_required_sites": owned_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "nonowning_copy_helper_required_sites": nonowning_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "arc_mode_retain_insertions": arc_mode_sema.get(
                "retain_release_operation_lowering_retain_insertion_sites"
            ),
            "arc_inference_retain_insertions": arc_inference_sema.get(
                "retain_release_operation_lowering_retain_insertion_sites"
            ),
            "arc_cleanup_scope_release_insertions": arc_cleanup_scope_sema.get(
                "retain_release_operation_lowering_release_insertion_sites"
            ),
            "arc_implicit_cleanup_release_insertions": arc_implicit_cleanup_sema.get(
                "retain_release_operation_lowering_release_insertion_sites"
            ),
            "arc_autorelease_return_autorelease_insertions": arc_autorelease_return_sema.get(
                "retain_release_operation_lowering_autorelease_insertion_sites"
            ),
            "weak_negative_diagnostic_count": weak_negative["diagnostic_count"],
            "unowned_negative_diagnostic_count": unowned_negative["diagnostic_count"],
            "negative_diagnostics_batch": negative_batch,
        },
    )
