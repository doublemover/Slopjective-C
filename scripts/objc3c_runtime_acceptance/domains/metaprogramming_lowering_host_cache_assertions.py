"""Assertions for metaprogramming lowering host-cache acceptance cases."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect


def expect_expansion_lowering_surface(surface: dict[str, Any]) -> None:
    expect(
        surface.get("contract_id")
        == "objc3c.metaprogramming.expansion.lowering.contract.v1",
        "expected expansion lowering fixture to preserve the metaprogramming lowering contract",
    )
    expect(
        surface.get("derive_contract_id")
        == "objc3c.metaprogramming.derive.expansion.inventory.v1"
        and surface.get("macro_contract_id")
        == "objc3c.metaprogramming.macro.safety.sandbox.determinism.semantics.v1"
        and surface.get("property_legality_contract_id")
        == "objc3c.metaprogramming.property.behavior.legality.interaction.completion.v1",
        "expected expansion lowering fixture to preserve the derive/macro/property legality dependencies",
    )
    expect(
        surface.get("derived_selector_artifact_sites") == 1
        and surface.get("macro_replay_visible_sites") == 1
        and surface.get("property_behavior_sites") == 2
        and surface.get("guard_blocked_sites") == 0
        and surface.get("contract_violation_sites") == 0
        and surface.get("deterministic_handoff") is True
        and surface.get("ready_for_ir_emission") is True,
        "expected expansion lowering fixture to preserve deterministic lowering counts and readiness",
    )


def expect_synthesized_emission_surface(
    surface: dict[str, Any], expansion_lowering_surface: dict[str, Any]
) -> None:
    expect(
        surface.get("contract_id")
        == "objc3c.metaprogramming.synthesized.ast.ir.emission.v1",
        "expected expansion lowering fixture to preserve the synthesized AST/IR emission contract",
    )
    expect(
        surface.get("dependency_contract_id")
        == expansion_lowering_surface.get("contract_id"),
        "expected synthesized AST/IR emission to depend on the lowering contract",
    )
    expect(
        surface.get("emitted_derive_method_sites") == 1
        and surface.get("emitted_macro_artifact_sites") == 1
        and surface.get("emitted_property_behavior_artifact_sites") == 2
        and surface.get("emitted_runtime_method_list_sites") == 1
        and surface.get("guard_blocked_sites") == 0
        and surface.get("contract_violation_sites") == 0
        and surface.get("deterministic_handoff") is True
        and surface.get("ready_for_ir_emission") is True,
        "expected synthesized AST/IR emission to preserve executable artifact counts and readiness",
    )
    expect(
        surface.get("dependency_replay_key")
        == expansion_lowering_surface.get("replay_key"),
        "expected synthesized AST/IR emission to preserve the lowering replay key",
    )


def expect_replay_preservation_surface(
    surface: dict[str, Any],
    expansion_lowering_surface: dict[str, Any],
    synthesized_emission_surface: dict[str, Any],
) -> None:
    expect(
        surface.get("contract_id")
        == "objc3c.metaprogramming.module.interface.replay.preservation.v1",
        "expected expansion lowering fixture to preserve the module replay preservation contract",
    )
    expect(
        surface.get("source_contract_id") == synthesized_emission_surface.get("contract_id"),
        "expected replay preservation to depend on the synthesized emission contract",
    )
    expect(
        surface.get("local_derive_method_count") == 1
        and surface.get("local_macro_artifact_count") == 1
        and surface.get("local_interface_property_behavior_artifact_count") == 1
        and surface.get("local_implementation_property_behavior_artifact_count") == 1
        and surface.get("local_runtime_method_list_count") == 1
        and surface.get("runtime_import_artifact_ready") is True
        and surface.get("separate_compilation_preservation_ready") is True
        and surface.get("deterministic") is True,
        "expected replay preservation to preserve local artifact counts and readiness",
    )
    expect(
        surface.get("expansion_lowering_replay_key")
        == expansion_lowering_surface.get("replay_key")
        and surface.get("synthesized_emission_replay_key")
        == synthesized_emission_surface.get("replay_key"),
        "expected replay preservation to preserve lowering and synthesized emission replay keys",
    )


def expect_lowering_llvm_summary(lowering_ll: str) -> None:
    expect(
        "metaprogramming_synthesized_ast_and_ir_emission" in lowering_ll,
        "expected lowering fixture LLVM IR to preserve the metaprogramming synthesized emission summary",
    )


def expect_host_cache_surfaces(
    host_cache_surface: dict[str, Any],
    host_cache_import_surface: dict[str, Any],
) -> None:
    expect(
        host_cache_surface.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1"
        and host_cache_import_surface.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        "expected host-cache artifact and runtime import surface to preserve the host-cache integration contract",
    )
    expect(
        host_cache_surface.get("source_contract_id")
        == "objc3c.metaprogramming.expansion.host.runtime.boundary.v1"
        and host_cache_import_surface.get("source_contract_id")
        == "objc3c.metaprogramming.expansion.host.runtime.boundary.v1",
        "expected host-cache artifact and runtime import surface to preserve the host runtime boundary contract",
    )
    expect(
        host_cache_surface.get("host_executable_relative_path")
        == "artifacts/bin/objc3c-frontend-c-api-runner.exe"
        and host_cache_surface.get("cache_root_relative_path")
        == "tmp/artifacts/objc3c-native/cache/metaprogramming"
        and host_cache_import_surface.get("host_executable_relative_path")
        == "artifacts/bin/objc3c-frontend-c-api-runner.exe"
        and host_cache_import_surface.get("cache_root_relative_path")
        == "tmp/artifacts/objc3c-native/cache/metaprogramming",
        "expected host-cache artifact and runtime import surface to preserve compatibility paths",
    )
    expect(
        host_cache_import_surface.get("runtime_import_artifact_ready") is True
        and host_cache_import_surface.get("separate_compilation_ready") is True
        and host_cache_import_surface.get("deterministic") is True
        and host_cache_surface.get("deterministic") is True,
        "expected host-cache artifact and runtime import surface to preserve deterministic compatibility readiness",
    )
    expect(
        host_cache_import_surface.get("replay_key") == host_cache_surface.get("replay_key"),
        "expected host-cache artifact and runtime import surface to preserve the same replay key",
    )
