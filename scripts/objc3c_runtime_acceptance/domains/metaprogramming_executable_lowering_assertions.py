"""Assertions for metaprogramming executable lowering acceptance cases."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.paths import RUNTIME_LIB_RELATIVE_PATH


def expect_synthesized_emission_surface(surface: dict[str, Any]) -> None:
    expect(
        surface.get("contract_id")
        == "objc3c.metaprogramming.synthesized.ast.ir.emission.v1",
        "expected synthesized AST/IR macro fixture to preserve the synthesized AST/IR emission contract",
    )
    expect(
        surface.get("emitted_derive_method_sites") == 1
        and surface.get("emitted_macro_artifact_sites") == 1
        and surface.get("emitted_property_behavior_artifact_sites") == 2
        and surface.get("emitted_global_artifact_sites") == 4
        and surface.get("emitted_runtime_method_list_sites") == 1
        and surface.get("guard_blocked_sites") == 0
        and surface.get("contract_violation_sites") == 0
        and surface.get("deterministic_handoff") is True
        and surface.get("ready_for_ir_emission") is True,
        "expected synthesized AST/IR macro fixture to preserve executable metaprogramming lowering counts and readiness",
    )


def expect_emission_llvm_summary(emission_ll: str) -> None:
    expect(
        "metaprogramming_synthesized_ast_and_ir_emission" in emission_ll
        and "emitted_derive_method_sites=1" in emission_ll
        and "emitted_macro_artifact_sites=1" in emission_ll
        and "emitted_property_behavior_artifact_sites=2" in emission_ll,
        "expected synthesized AST/IR macro fixture LLVM IR to preserve the executable lowering summary",
    )


def expect_boundary_llvm_summary(boundary_ll: str) -> None:
    expect(
        "metaprogramming_expansion_host_runtime_boundary" in boundary_ll
        and "property_runtime_ready=true" in boundary_ll
        and "macro_host_execution_ready=false" in boundary_ll
        and "runtime_package_loader_ready=false" in boundary_ll,
        "expected expansion host/runtime boundary fixture LLVM IR to preserve the deferred macro-execution boundary summary",
    )


def expect_runtime_boundary_payload(payload: dict[str, Any]) -> None:
    expect(
        payload.get("copy_status") == 0
        and payload.get("property_runtime_ready") == 1
        and payload.get("macro_host_execution_ready") == 0
        and payload.get("macro_host_process_launch_ready") == 0
        and payload.get("runtime_package_loader_ready") == 0
        and payload.get("deterministic") == 1,
        "expected runtime boundary probe to preserve executable lowering readiness while macro host execution remains deferred",
    )
    expect(
        payload.get("runtime_support_library_archive_relative_path")
        == RUNTIME_LIB_RELATIVE_PATH,
        "expected runtime boundary probe to preserve the runtime support library archive path",
    )
    expect(
        payload.get("property_behavior_runtime_model")
        == "supported-property-behavior-lowering-reuses-existing-private-runtime-property-accessor-layout-and-current-property-hooks",
        "expected runtime boundary probe to preserve the property behavior runtime model",
    )
    expect(
        payload.get("macro_expansion_host_model")
        == "macro-host-execution-process-launch-and-runtime-package-loading-remain-disabled-and-fail-closed",
        "expected runtime boundary probe to preserve the macro expansion host model",
    )
    expect(
        payload.get("fail_closed_model")
        == "no-live-macro-expansion-host-or-runtime-package-loader-is-claimed-yet",
        "expected runtime boundary probe to preserve the fail-closed runtime model",
    )
