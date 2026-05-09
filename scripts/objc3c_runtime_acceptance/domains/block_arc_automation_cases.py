"""Block/ARC storage automation runtime acceptance cases."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.block_arc_automation_artifacts import (
    load_block_arc_automation_artifacts,
)
from objc3c_runtime_acceptance.domains.block_arc_automation_assertions import (
    assert_block_arc_automation_artifacts,
)


def check_block_storage_arc_automation_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "block-storage-arc-automation-semantics"
    artifacts = load_block_arc_automation_artifacts(case_dir)
    assert_block_arc_automation_artifacts(artifacts)
    weak_negative = artifacts.negative_batch["results"][0]
    unowned_negative = artifacts.negative_batch["results"][1]

    return CaseResult(
        case_id="block-storage-arc-automation-semantics",
        probe="compile-manifest-diagnostics-and-llvm-ir",
        fixture="tests/tooling/fixtures/native/owned_object_capture_helper_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "owned_copy_helper_required_sites": artifacts.owned_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "nonowning_copy_helper_required_sites": (
                artifacts.nonowning_copy_dispose_surface.get(
                    "copy_helper_required_sites"
                )
            ),
            "arc_mode_retain_insertions": artifacts.arc_mode_sema.get(
                "retain_release_operation_lowering_retain_insertion_sites"
            ),
            "arc_inference_retain_insertions": artifacts.arc_inference_sema.get(
                "retain_release_operation_lowering_retain_insertion_sites"
            ),
            "arc_cleanup_scope_release_insertions": artifacts.arc_cleanup_scope_sema.get(
                "retain_release_operation_lowering_release_insertion_sites"
            ),
            "arc_implicit_cleanup_release_insertions": artifacts.arc_implicit_cleanup_sema.get(
                "retain_release_operation_lowering_release_insertion_sites"
            ),
            "arc_autorelease_return_autorelease_insertions": (
                artifacts.arc_autorelease_return_sema.get(
                    "retain_release_operation_lowering_autorelease_insertion_sites"
                )
            ),
            "weak_negative_diagnostic_count": weak_negative["diagnostic_count"],
            "unowned_negative_diagnostic_count": unowned_negative["diagnostic_count"],
            "negative_diagnostics_batch": artifacts.negative_batch,
        },
    )
