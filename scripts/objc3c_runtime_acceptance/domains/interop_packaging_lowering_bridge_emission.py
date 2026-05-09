"""Mixed-image package lowering bridge-emission acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..runtime_contract_interop import (
    INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
    INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
)
from ..fixture_compilation import compile_fixture_outputs_with_args
from ..fixture_compilation import compile_fixture_with_args
from ..paths import ROOT


def check_mixed_image_package_lowering_bridge_emission_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "mixed-image-package-lowering-bridge-emission"
    provider_fixture = ROOT / Path(INTEROP_HEADER_MODULE_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_HEADER_MODULE_CONSUMER_FIXTURE)

    _, provider_ll_path, _ = compile_fixture_outputs_with_args(
        provider_fixture,
        case_dir / "provider",
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    compile_fixture_with_args(
        consumer_fixture,
        case_dir / "consumer",
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(case_dir / "provider" / "module.runtime-import-surface.json"),
        ],
    )

    provider_ll = provider_ll_path.read_text(encoding="utf-8")
    provider_bridge_json = json.loads(
        ((case_dir / "provider") / "module.interop-bridge.json").read_text(
            encoding="utf-8"
        )
    )
    link_plan = json.loads(
        (
            (case_dir / "consumer") / "module.cross-module-runtime-link-plan.json"
        ).read_text(encoding="utf-8")
    )

    for needle, label in (
        ("interop_interop_lowering_abi_contract", "interop ABI lowering summary"),
        (
            "interop_ffi_metadata_interface_preservation",
            "ffi metadata/interface preservation summary",
        ),
        (
            "interop_header_module_and_bridge_generation",
            "bridge-generation lowering summary",
        ),
        ("declare i32 @ffiInbound(i32)", "imported bridge declaration"),
        ("declare i32 @ffiHeaderBridge(i32)", "local bridge declaration"),
    ):
        expect(
            needle in provider_ll,
            f"expected provider lowering to publish the {label} in LLVM IR",
        )
    expect(
        isinstance(provider_bridge_json.get("foreign_callables"), list)
        and {entry.get("name") for entry in provider_bridge_json["foreign_callables"]}
        == {"ffiInbound", "ffiHeaderBridge"},
        "expected provider bridge emission to publish both interop callables",
    )
    expect(
        link_plan.get("interop_header_module_bridge_imported_module_count") == 1
        and link_plan.get("interop_ffi_imported_module_count") == 1,
        "expected mixed-image consumer packaging to preserve one imported interop module across both bridge surfaces",
    )
    expect(
        link_plan.get("expected_interop_bridge_artifact_relative_path")
        == "module.interop-bridge.json"
        and "m274_header_module_bridge_provider"
        in link_plan.get(
            "interop_header_module_bridge_imported_module_names_lexicographic", []
        ),
        "expected mixed-image consumer packaging to preserve the emitted bridge artifact identity",
    )

    return CaseResult(
        case_id="mixed-image-package-lowering-bridge-emission",
        probe="compile-llvm-ir-runtime-import-surface-and-cross-module-link-plan",
        fixture=INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "provider_ll_path": str(provider_ll_path.relative_to(ROOT)).replace(
                "\\", "/"
            ),
            "foreign_callable_count": len(
                provider_bridge_json.get("foreign_callables", [])
            ),
            "imported_bridge_module_count": link_plan.get(
                "interop_header_module_bridge_imported_module_count"
            ),
        },
    )


__all__ = ["check_mixed_image_package_lowering_bridge_emission_case"]
