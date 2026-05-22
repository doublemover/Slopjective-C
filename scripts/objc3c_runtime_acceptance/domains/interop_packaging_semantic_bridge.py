"""C/C++/Swift interop boundary packaging semantic acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..runtime_contract_interop import (
    INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
    INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
)
from ..fixture_compilation import compile_fixture_expect_failure
from ..fixture_compilation import compile_fixture_with_args
from ..paths import ROOT


def _write_tampered_bridge_import_surface(
    source_path: Path,
    target_path: Path,
) -> None:
    payload = json.loads(source_path.read_text(encoding="utf-8"))
    ffi_surface = payload["objc_interop_ffi_metadata_and_interface_preservation"]
    ffi_surface["runtime_import_artifact_ready"] = True
    ffi_surface["separate_compilation_preservation_ready"] = True
    ffi_surface["deterministic"] = True
    bridge_surface = payload["objc_interop_header_module_and_bridge_generation"]
    bridge_surface["runtime_generation_ready"] = True
    bridge_surface["cross_module_packaging_ready"] = True
    bridge_surface["deterministic"] = True
    bridge_surface["header_artifact_relative_path"] = (
        "../tampered/module.interop-bridge.h"
    )
    target_path.write_text(
        json.dumps(payload, indent=2, sort_keys=True),
        encoding="utf-8",
    )


def check_c_cpp_swift_bridge_semantics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "c-cpp-swift-interop-boundary-semantics"
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
    provider_ffi_surface = provider_import_surface.get(
        "objc_interop_foreign_surface_interface_and_module_preservation", {}
    )
    provider_bridge_surface = provider_import_surface.get(
        "objc_interop_header_module_and_bridge_generation", {}
    )
    provider_bridge_paths = [
        provider_compile_dir / "module.interop-bridge.h",
        provider_compile_dir / "module.interop-bridge.modulemap",
        provider_compile_dir / "module.interop-bridge.json",
    ]

    expect(
        provider_ffi_surface.get("local_foreign_callable_count") == 2
        and provider_ffi_surface.get("local_cpp_name_annotation_count") == 2
        and provider_ffi_surface.get("local_header_name_annotation_count") == 2
        and provider_ffi_surface.get("local_swift_name_annotation_count") == 1,
        "expected provider import surface to preserve the C/C++/Swift-facing annotation counts",
    )
    expect(
        provider_bridge_surface.get("local_import_module_names_lexicographic")
        == ['"BridgeProviderKit"'],
        "expected provider bridge-generation surface to preserve the import-module name",
    )
    for artifact_path in provider_bridge_paths:
        expect(
            not artifact_path.exists(),
            "expected C/C++/Swift source-surface provider not to publish deferred bridge artifacts",
        )
    expect(
        provider_bridge_surface.get("runtime_generation_ready") is False
        and provider_bridge_surface.get("cross_module_packaging_ready") is False
        and provider_bridge_surface.get("deterministic") is False,
        "expected provider bridge-generation surface to preserve bridge paths without claiming generated artifacts",
    )

    consumer_compile_dir = case_dir / "consumer"
    consumer_import_negative = compile_fixture_expect_failure(
        consumer_fixture,
        consumer_compile_dir,
        expected_snippets=[
            "cross-module runtime link-plan Part 11 ffi preservation surface incomplete"
        ],
        expected_codes=[],
        extra_args=[
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_compile_dir / "module.runtime-import-surface.json"),
        ],
        allow_missing_structured_diagnostics=True,
    )

    tampered_surface = (
        case_dir / "provider.tampered-bridge-path.runtime-import-surface.json"
    )
    _write_tampered_bridge_import_surface(
        provider_compile_dir / "module.runtime-import-surface.json",
        tampered_surface,
    )
    tampered_result = compile_fixture_expect_failure(
        consumer_fixture,
        case_dir / "consumer-tampered-bridge-path",
        expected_snippets=[
            "active Part 11 header/module/bridge generation header artifact path must not traverse directories"
        ],
        expected_codes=[],
        extra_args=[
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(tampered_surface),
        ],
        allow_missing_structured_diagnostics=True,
    )

    return CaseResult(
        case_id="c-cpp-swift-interop-boundary-semantics",
        probe="compile-runtime-import-surface-and-fail-closed-bridge-boundary",
        fixture=INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "provider_module_name": provider_import_surface.get("module_name"),
            "foreign_callable_count": provider_ffi_surface.get(
                "local_foreign_callable_count"
            ),
            "cpp_name_annotation_count": provider_ffi_surface.get(
                "local_cpp_name_annotation_count"
            ),
            "swift_name_annotation_count": provider_ffi_surface.get(
                "local_swift_name_annotation_count"
            ),
            "consumer_import_returncode": consumer_import_negative["returncode"],
            "tampered_bridge_path_returncode": tampered_result["returncode"],
        },
    )


__all__ = ["check_c_cpp_swift_bridge_semantics_case"]
