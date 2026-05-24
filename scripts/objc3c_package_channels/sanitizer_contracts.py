"""Shared runtime package variant contracts for package-channel surfaces."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class RuntimePackageVariantContract:
    sanitizer_variant: str
    package_id: str
    package_channel_id: str
    archive_suffix: str
    runtime_variant: str
    runtime_library_ids: tuple[str, ...]
    sanitizer_payload_entries: tuple[str, ...] = ()
    runtime_library_payload_entries: tuple[str, ...] = ()
    package_variant_row_id: str | None = None
    install_selector: str | None = None
    metadata_manifest_path: str | None = None
    runtime_library_manifest_path: str | None = None
    missing_runtime_behavior: str | None = None
    trap_or_recover_mode: str | None = None
    support_truth: bool = False
    native_execution_claimed: bool = False

    def as_metadata(self) -> dict[str, Any]:
        payload: dict[str, Any] = {
            "sanitizer_variant": self.sanitizer_variant,
            "package_id": self.package_id,
            "package_channel_id": self.package_channel_id,
            "archive_suffix": self.archive_suffix,
            "runtime_variant": self.runtime_variant,
            "runtime_library_ids": list(self.runtime_library_ids),
            "support_truth": self.support_truth,
            "native_execution_claimed": self.native_execution_claimed,
        }
        optional_values = {
            "package_variant_row_id": self.package_variant_row_id,
            "install_selector": self.install_selector,
            "metadata_manifest_path": self.metadata_manifest_path,
            "runtime_library_manifest_path": self.runtime_library_manifest_path,
            "missing_runtime_behavior": self.missing_runtime_behavior,
            "trap_or_recover_mode": self.trap_or_recover_mode,
        }
        for key, value in optional_values.items():
            if value is not None:
                payload[key] = value
        if self.runtime_library_payload_entries:
            payload["runtime_library_payload_entries"] = list(
                self.runtime_library_payload_entries
            )
        return payload


ASAN_RUNTIME_LIBRARY_ARTIFACTS = (
    "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.dll",
    "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.lib",
    "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib",
)
UBSAN_RUNTIME_LIBRARY_ARTIFACTS = (
    "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone-x86_64.lib",
    "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone_cxx-x86_64.lib",
)
ASAN_RUNTIME_LIBRARY_MANIFEST = "share/objc3c/sanitizer/asan-runtime-libraries.json"
UBSAN_RUNTIME_LIBRARY_MANIFEST = "share/objc3c/sanitizer/ubsan-runtime-libraries.json"
ASAN_METADATA_MANIFEST = "share/objc3c/sanitizer/asan-metadata.json"
UBSAN_METADATA_MANIFEST = "share/objc3c/sanitizer/ubsan-metadata.json"

RUNTIME_PACKAGE_VARIANT_CONTRACTS: dict[str, RuntimePackageVariantContract] = {
    "release": RuntimePackageVariantContract(
        sanitizer_variant="release",
        package_id="org.objc3c.runtime:objc3c-runtime-release",
        package_channel_id="windows-x64-release",
        archive_suffix="windows-x64",
        runtime_variant="release",
        runtime_library_ids=("objc3-runtime",),
    ),
    "address": RuntimePackageVariantContract(
        sanitizer_variant="address",
        package_id="org.objc3c.runtime:objc3c-runtime-asan",
        package_variant_row_id="objc3c.package.sanitizer.asan.reserved",
        package_channel_id="windows-x64-sanitizer-asan",
        archive_suffix="windows-x64-asan",
        runtime_variant="sanitizer=address",
        install_selector="sanitizer=address",
        metadata_manifest_path=ASAN_METADATA_MANIFEST,
        runtime_library_manifest_path=ASAN_RUNTIME_LIBRARY_MANIFEST,
        sanitizer_payload_entries=(
            ASAN_METADATA_MANIFEST,
            ASAN_RUNTIME_LIBRARY_MANIFEST,
            *ASAN_RUNTIME_LIBRARY_ARTIFACTS,
        ),
        runtime_library_payload_entries=(
            ASAN_RUNTIME_LIBRARY_MANIFEST,
            *ASAN_RUNTIME_LIBRARY_ARTIFACTS,
        ),
        runtime_library_ids=("objc3-runtime", "clang_rt.asan"),
        missing_runtime_behavior="fail-closed-before-package-install",
    ),
    "undefined": RuntimePackageVariantContract(
        sanitizer_variant="undefined",
        package_id="org.objc3c.runtime:objc3c-runtime-ubsan",
        package_variant_row_id="objc3c.package.sanitizer.ubsan.reserved",
        package_channel_id="windows-x64-sanitizer-ubsan",
        archive_suffix="windows-x64-ubsan",
        runtime_variant="sanitizer=undefined",
        install_selector="sanitizer=undefined",
        metadata_manifest_path=UBSAN_METADATA_MANIFEST,
        runtime_library_manifest_path=UBSAN_RUNTIME_LIBRARY_MANIFEST,
        sanitizer_payload_entries=(
            UBSAN_METADATA_MANIFEST,
            UBSAN_RUNTIME_LIBRARY_MANIFEST,
            *UBSAN_RUNTIME_LIBRARY_ARTIFACTS,
        ),
        runtime_library_payload_entries=(
            UBSAN_RUNTIME_LIBRARY_MANIFEST,
            *UBSAN_RUNTIME_LIBRARY_ARTIFACTS,
        ),
        runtime_library_ids=("objc3-runtime", "clang_rt.ubsan"),
        missing_runtime_behavior="fail-closed-before-package-install",
        trap_or_recover_mode="trap",
    ),
}

SANITIZER_VARIANTS = tuple(RUNTIME_PACKAGE_VARIANT_CONTRACTS)
PACKAGE_IDS = {
    variant: contract.package_id
    for variant, contract in RUNTIME_PACKAGE_VARIANT_CONTRACTS.items()
}
PACKAGE_CHANNEL_IDS = {
    variant: contract.package_channel_id
    for variant, contract in RUNTIME_PACKAGE_VARIANT_CONTRACTS.items()
}
SANITIZER_PAYLOAD_ENTRIES = {
    variant: list(contract.sanitizer_payload_entries)
    for variant, contract in RUNTIME_PACKAGE_VARIANT_CONTRACTS.items()
    if contract.sanitizer_payload_entries
}
SANITIZER_RUNTIME_LIBRARY_ENTRIES = {
    variant: list(contract.runtime_library_payload_entries)
    for variant, contract in RUNTIME_PACKAGE_VARIANT_CONTRACTS.items()
    if contract.runtime_library_payload_entries
}
SANITIZER_RUNTIME_LIBRARY_MANIFEST_PATHS = {
    variant: str(contract.runtime_library_manifest_path)
    for variant, contract in RUNTIME_PACKAGE_VARIANT_CONTRACTS.items()
    if contract.runtime_library_manifest_path is not None
}


def runtime_package_variant_contract(
    sanitizer_variant: str,
) -> RuntimePackageVariantContract:
    try:
        return RUNTIME_PACKAGE_VARIANT_CONTRACTS[sanitizer_variant]
    except KeyError as error:
        raise ValueError(f"unsupported sanitizer variant: {sanitizer_variant}") from error


def sanitizer_variant_metadata(sanitizer_variant: str) -> dict[str, Any]:
    return runtime_package_variant_contract(sanitizer_variant).as_metadata()


def payload_entries_for_variant(
    base_entries: tuple[str, ...] | list[str],
    sanitizer_variant: str,
) -> list[str]:
    contract = runtime_package_variant_contract(sanitizer_variant)
    return [*base_entries, *contract.sanitizer_payload_entries]


__all__ = [
    "PACKAGE_CHANNEL_IDS",
    "PACKAGE_IDS",
    "RUNTIME_PACKAGE_VARIANT_CONTRACTS",
    "SANITIZER_PAYLOAD_ENTRIES",
    "SANITIZER_RUNTIME_LIBRARY_ENTRIES",
    "SANITIZER_RUNTIME_LIBRARY_MANIFEST_PATHS",
    "SANITIZER_VARIANTS",
    "RuntimePackageVariantContract",
    "payload_entries_for_variant",
    "runtime_package_variant_contract",
    "sanitizer_variant_metadata",
]
