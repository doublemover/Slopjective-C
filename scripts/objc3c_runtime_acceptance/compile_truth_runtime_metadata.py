"""Runtime metadata truth checks for native compile outputs."""

from __future__ import annotations

from dataclasses import dataclass
import re
from typing import Any

from .checksums import replay_key_counter


@dataclass(frozen=True)
class RuntimeMetadataTruth:
    property_descriptor_count_expected: int
    property_descriptor_definition_count: int
    property_descriptor_section_present: bool
    ivar_descriptor_count_expected: int
    ivar_descriptor_definition_count: int
    ivar_descriptor_section_present: bool
    property_synthesis_sites_expected: int
    synthesized_accessor_definition_count: int
    current_property_helper_call_count: int
    property_descriptor_counts_match: bool
    ivar_descriptor_counts_match: bool
    synthesized_property_surface_matches: bool

    @property
    def truthful(self) -> bool:
        return (
            self.property_descriptor_section_present
            and self.ivar_descriptor_section_present
            and self.property_descriptor_counts_match
            and self.ivar_descriptor_counts_match
            and self.synthesized_property_surface_matches
        )

    def failures(self) -> list[str]:
        failures: list[str] = []
        if not self.property_descriptor_section_present:
            failures.append(
                "missing property descriptor aggregate section in emitted LLVM IR"
            )
        if not self.ivar_descriptor_section_present:
            failures.append("missing ivar descriptor aggregate section in emitted LLVM IR")
        if not self.property_descriptor_counts_match:
            failures.append(
                "property descriptor count mismatch: "
                f"registration manifest={self.property_descriptor_count_expected} "
                f"emitted LLVM IR={self.property_descriptor_definition_count}"
            )
        if not self.ivar_descriptor_counts_match:
            failures.append(
                "ivar descriptor count mismatch: "
                f"registration manifest={self.ivar_descriptor_count_expected} "
                f"emitted LLVM IR={self.ivar_descriptor_definition_count}"
            )
        if not self.synthesized_property_surface_matches:
            failures.append(
                "synthesized property lowering replay claims do not match emitted "
                "runtime-backed accessor/helper surface"
            )
        return failures

    def payload_fields(self) -> dict[str, object]:
        return {
            "property_descriptor_count_expected": (
                self.property_descriptor_count_expected
            ),
            "property_descriptor_definition_count": (
                self.property_descriptor_definition_count
            ),
            "property_descriptor_section_present": (
                self.property_descriptor_section_present
            ),
            "ivar_descriptor_count_expected": self.ivar_descriptor_count_expected,
            "ivar_descriptor_definition_count": self.ivar_descriptor_definition_count,
            "ivar_descriptor_section_present": self.ivar_descriptor_section_present,
            "property_synthesis_sites_expected": self.property_synthesis_sites_expected,
            "synthesized_accessor_definition_count": (
                self.synthesized_accessor_definition_count
            ),
            "current_property_helper_call_count": self.current_property_helper_call_count,
            "property_descriptor_counts_match": self.property_descriptor_counts_match,
            "ivar_descriptor_counts_match": self.ivar_descriptor_counts_match,
            "synthesized_property_surface_matches": (
                self.synthesized_property_surface_matches
            ),
        }


def runtime_metadata_truth_from_outputs(
    *,
    manifest: dict[str, Any],
    registration_manifest: dict[str, Any],
    ll_text: str,
) -> RuntimeMetadataTruth:
    property_synthesis = manifest.get("lowering_property_synthesis_ivar_binding", {})
    property_descriptor_count_expected = int(
        registration_manifest.get("property_descriptor_count", 0)
    )
    ivar_descriptor_count_expected = int(
        registration_manifest.get("ivar_descriptor_count", 0)
    )
    property_synthesis_sites_expected = 0
    if isinstance(property_synthesis, dict):
        property_synthesis_sites_expected = replay_key_counter(
            str(property_synthesis.get("replay_key", "")),
            "property_synthesis_sites",
        )

    property_descriptor_definition_count = len(
        re.findall(r"(?m)^@__objc3_meta_property_[0-9]+ = ", ll_text)
    )
    ivar_descriptor_definition_count = len(
        re.findall(r"(?m)^@__objc3_meta_ivar_[0-9]+ = ", ll_text)
    )
    property_descriptor_section_present = (
        len(re.findall(r"(?m)^@__objc3_sec_property_descriptors = ", ll_text)) >= 1
    )
    ivar_descriptor_section_present = (
        len(re.findall(r"(?m)^@__objc3_sec_ivar_descriptors = ", ll_text)) >= 1
    )
    current_property_helper_call_count = (
        len(
            re.findall(
                r"(?m)call i32 @objc3_runtime_read_current_property_i32\(",
                ll_text,
            )
        )
        + len(
            re.findall(
                r"(?m)call void @objc3_runtime_write_current_property_i32\(",
                ll_text,
            )
        )
        + len(
            re.findall(
                r"(?m)call i32 @objc3_runtime_exchange_current_property_i32\(",
                ll_text,
            )
        )
    )
    synthesized_accessor_definition_count = (
        len(re.findall(r"(?m)^define i32 @objc3_method_.*_instance_.*\(", ll_text))
        + len(re.findall(r"(?m)^define i1 @objc3_method_.*_instance_.*\(", ll_text))
        + len(re.findall(r"(?m)^define void @objc3_method_.*_instance_.*\(", ll_text))
    )

    property_descriptor_counts_match = (
        property_descriptor_definition_count == property_descriptor_count_expected
    )
    ivar_descriptor_counts_match = (
        ivar_descriptor_definition_count == ivar_descriptor_count_expected
    )
    synthesized_property_surface_matches = (
        property_synthesis_sites_expected == 0
        or (
            property_descriptor_count_expected > 0
            and (
                (
                    current_property_helper_call_count > 0
                    and synthesized_accessor_definition_count
                    >= property_synthesis_sites_expected
                )
                or current_property_helper_call_count == 0
            )
        )
    )
    return RuntimeMetadataTruth(
        property_descriptor_count_expected=property_descriptor_count_expected,
        property_descriptor_definition_count=property_descriptor_definition_count,
        property_descriptor_section_present=property_descriptor_section_present,
        ivar_descriptor_count_expected=ivar_descriptor_count_expected,
        ivar_descriptor_definition_count=ivar_descriptor_definition_count,
        ivar_descriptor_section_present=ivar_descriptor_section_present,
        property_synthesis_sites_expected=property_synthesis_sites_expected,
        synthesized_accessor_definition_count=synthesized_accessor_definition_count,
        current_property_helper_call_count=current_property_helper_call_count,
        property_descriptor_counts_match=property_descriptor_counts_match,
        ivar_descriptor_counts_match=ivar_descriptor_counts_match,
        synthesized_property_surface_matches=synthesized_property_surface_matches,
    )


__all__ = [
    "RuntimeMetadataTruth",
    "runtime_metadata_truth_from_outputs",
]
