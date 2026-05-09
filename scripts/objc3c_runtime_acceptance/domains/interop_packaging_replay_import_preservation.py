"""Cross-language replay import-surface preservation acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..runtime_contract_interop import (
    INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
    INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
)
from ..fixture_compilation import compile_fixture_with_args
from ..paths import ROOT


def check_cross_language_replay_import_surface_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "cross-language-replay-import-surface-preservation"
    provider_fixture = ROOT / Path(INTEROP_HEADER_MODULE_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_HEADER_MODULE_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_import_surface = json.loads(
        (provider_compile_dir / "module.runtime-import-surface.json").read_text(
            encoding="utf-8"
        )
    )
    consumer_compile_dir = case_dir / "consumer"
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_compile_dir / "module.runtime-import-surface.json"),
        ],
    )
    link_plan = json.loads(
        (
            consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
        ).read_text(encoding="utf-8")
    )
    imported_modules = link_plan.get("imported_modules", [])
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected replay-preservation consumer compile to publish one imported module",
    )
    imported_module = imported_modules[0]
    provider_ffi_surface = provider_import_surface.get(
        "objc_interop_foreign_surface_interface_and_module_preservation", {}
    )
    provider_bridge_surface = provider_import_surface.get(
        "objc_interop_header_module_and_bridge_generation", {}
    )

    expect(
        imported_module.get("interop_ffi_replay_key")
        == provider_bridge_surface.get("preservation_replay_key")
        and imported_module.get("interop_ffi_lowering_replay_key")
        in imported_module.get("interop_ffi_replay_key", ""),
        "expected consumer link plan to preserve the imported ffi replay and lowering replay keys",
    )
    expect(
        imported_module.get("interop_header_module_bridge_replay_key")
        == provider_bridge_surface.get("replay_key")
        and imported_module.get("interop_header_module_bridge_preservation_replay_key")
        == provider_bridge_surface.get("preservation_replay_key"),
        "expected consumer link plan to preserve the imported bridge replay and preservation replay keys",
    )
    expect(
        imported_module.get("interop_ffi_preservation_replay_key")
        == provider_ffi_surface.get("replay_key"),
        "expected consumer link plan to preserve the imported ffi preservation replay key through the full ffi preservation packet",
    )
    expect(
        imported_module.get("interop_ffi_source_contract_id")
        == "objc3c.interop.foreign.call.and.lifetime.lowering.v1"
        and imported_module.get("interop_header_module_bridge_source_contract_id")
        == "objc3c.interop.bridge.packaging.and.toolchain.contract.v1",
        "expected consumer link plan to preserve the imported replay source contracts",
    )

    return CaseResult(
        case_id="cross-language-replay-import-surface-preservation",
        probe="compile-runtime-import-surface-and-cross-module-link-plan-replay-key-inspection",
        fixture=INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "imported_module_name": imported_module.get("module_name"),
            "ffi_replay_key": imported_module.get("interop_ffi_replay_key"),
            "bridge_replay_key": imported_module.get(
                "interop_header_module_bridge_replay_key"
            ),
        },
    )


__all__ = ["check_cross_language_replay_import_surface_preservation_case"]
