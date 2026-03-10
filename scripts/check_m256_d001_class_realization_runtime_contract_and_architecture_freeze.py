#!/usr/bin/env python3
"""Validate M256-D001 class realization runtime freeze."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
import uuid
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "M256-D001"
CONTRACT_ID = "objc3c-runtime-class-realization-freeze/m256-d001-v1"
CLASS_REALIZATION_MODEL = (
    "registered-class-bundles-realize-one-deterministic-class-metaclass-chain-per-class-name"
)
METACLASS_GRAPH_MODEL = (
    "known-class-and-class-self-receivers-normalize-onto-the-metaclass-record-chain"
)
CATEGORY_ATTACHMENT_MODEL = (
    "preferred-category-implementation-records-attach-after-class-bundle-resolution"
)
PROTOCOL_CHECK_MODEL = (
    "adopted-and-inherited-protocol-method-lists-provide-declaration-aware-negative-runtime-checks"
)
FAIL_CLOSED_MODEL = (
    "invalid-bundle-graphs-category-conflicts-and-ambiguous-runtime-resolution-fail-closed"
)
SUMMARY_RELATIVE_PATH = "tmp/reports/m256/M256-D001/class_realization_runtime_contract_summary.json"
RUNTIME_PROBE_SOURCE = "tests/tooling/runtime/m256_d001_class_realization_runtime_probe.cpp"
FIXTURE_SOURCE = "tests/tooling/fixtures/native/m256_d001_class_realization_runtime_library.objc3"
BOUNDARY_PREFIX = "; runtime_class_realization = "
REALIZATION_PREFIX = "; executable_realization_records = "
EXPECTED_CLASS_RECEIVER = 1041
EXPECTED_INSTANCE_RECEIVER = 1042
EXPECTED_METACLASS_RECEIVER = 1043
EXPECTED_INHERITED_OWNER = "implementation:Base::instance_method:inheritedValue"
EXPECTED_CATEGORY_OWNER = "implementation:Widget(Tracing)::instance_method:tracedValue"
EXPECTED_CLASS_OWNER = "implementation:Widget::class_method:classValue"

EXPECTATIONS_SNIPPETS = (
    "# M256 Class Realization Runtime Contract and Architecture Freeze Expectations (D001)",
    f"Contract ID: `{CONTRACT_ID}`",
    f"`{CLASS_REALIZATION_MODEL}`",
    f"`{METACLASS_GRAPH_MODEL}`",
    f"`{CATEGORY_ATTACHMENT_MODEL}`",
    f"`{PROTOCOL_CHECK_MODEL}`",
    f"`{FAIL_CLOSED_MODEL}`",
    f"`{RUNTIME_PROBE_SOURCE}`",
    f"`{SUMMARY_RELATIVE_PATH}`",
)
PACKET_SNIPPETS = (
    "# M256-D001 Class Realization Runtime Contract and Architecture Freeze Packet",
    "Packet: `M256-D001`",
    "Issue: `#7139`",
    f"Contract ID: `{CONTRACT_ID}`",
    f"`{FIXTURE_SOURCE}`",
    f"`{RUNTIME_PROBE_SOURCE}`",
    f"`{SUMMARY_RELATIVE_PATH}`",
)
NATIVE_DOC_SNIPPETS = (
    "## Class realization runtime freeze (M256-D001)",
    CONTRACT_ID,
    "`; runtime_class_realization = ...`",
    f"`{RUNTIME_PROBE_SOURCE}`",
    f"`{SUMMARY_RELATIVE_PATH}`",
)
LOWERING_SPEC_SNIPPETS = (
    "## M256 class realization runtime freeze (D001)",
    CONTRACT_ID,
    METACLASS_GRAPH_MODEL,
    CATEGORY_ATTACHMENT_MODEL,
    PROTOCOL_CHECK_MODEL,
)
METADATA_SPEC_SNIPPETS = (
    "## M256 class realization runtime anchors (D001)",
    f"`{CONTRACT_ID}`",
    "`; runtime_class_realization = contract=objc3c-runtime-class-realization-freeze/m256-d001-v1`",
    f"`{RUNTIME_PROBE_SOURCE}`",
)
ARCHITECTURE_SNIPPETS = (
    "## M256 class realization runtime freeze (D001)",
    "known-class and class-self receivers normalize onto the metaclass chain",
    "category attachment remains runtime-owned after class-bundle selection",
    "protocol records remain declaration-aware negative lookup evidence only",
)
RUNTIME_README_SNIPPETS = (
    "`M256-D001` freezes the next runtime-owned object-model boundary",
    CONTRACT_ID,
    CATEGORY_ATTACHMENT_MODEL,
    FAIL_CLOSED_MODEL,
)
TOOLING_RUNTIME_README_SNIPPETS = (
    "`M256-D001` freezes the runtime-owned class realization boundary",
    CONTRACT_ID,
    "proof surface remains the private runtime snapshots in",
)
LOWERING_HEADER_SNIPPETS = (
    "kObjc3RuntimeClassRealizationContractId",
    "kObjc3RuntimeClassRealizationModel",
    "kObjc3RuntimeMetaclassGraphModel",
    "kObjc3RuntimeClassRealizationCategoryAttachmentModel",
    "kObjc3RuntimeProtocolCheckModel",
    "kObjc3RuntimeClassRealizationFailClosedModel",
    "Objc3RuntimeClassRealizationSummary();",
)
LOWERING_CPP_SNIPPETS = (
    "std::string Objc3RuntimeClassRealizationSummary()",
    'out << "contract=" << kObjc3RuntimeClassRealizationContractId',
    '<< ";class_realization_model="',
    '<< ";metaclass_graph_model="',
    '<< ";category_attachment_model="',
    '<< ";protocol_check_model="',
    '<< ";fail_closed_model="',
    '<< ";non_goals=no-property-storage-no-ivar-layout-no-protocol-body-dispatch"',
)
PARSER_SNIPPETS = (
    "M256-D001 class-realization-runtime freeze anchor",
    "runtime class realization, metaclass graph walking, category attachment,",
)
SEMA_SNIPPETS = (
    "M256-D001 class-realization-runtime freeze anchor",
    "realization must consume that closure rather than reinterpreting source.",
)
IR_EMITTER_SNIPPETS = (
    "M256-D001 class-realization-runtime freeze anchor",
    "; runtime_class_realization = ",
    '<< ";class_bundle_count="',
    '<< ";protocol_record_count="',
    '<< ";category_record_count="',
)
RUNTIME_HEADER_SNIPPETS = (
    "M256-D001 class-realization-runtime anchor",
    "realization boundary still fits behind this public lookup/dispatch ABI",
)
BOOTSTRAP_HEADER_SNIPPETS = (
    "M256-D001 class-realization-runtime anchor",
    "snapshots remain the canonical proof surface for realized class/metaclass",
)
RUNTIME_CPP_SNIPPETS = (
    "M256-D001 class-realization-runtime freeze anchor",
    "resolved from emitted category records only after one concrete class name",
    "emitted class/metaclass chain directly, consults attached categories at",
    "declaration-aware negative evidence only.",
)
PROBE_SNIPPETS = (
    '#include "runtime/objc3_runtime_bootstrap_internal.h"',
    'objc3_runtime_dispatch_i32(1042, "inheritedValue", 0, 0, 0, 0)',
    'objc3_runtime_dispatch_i32(1042, "tracedValue", 0, 0, 0, 0)',
    'objc3_runtime_dispatch_i32(1043, "classValue", 0, 0, 0, 0)',
    'objc3_runtime_dispatch_i32(1041, "classValue", 0, 0, 0, 0)',
    'objc3_runtime_dispatch_i32(1042, "ignoredValue", 0, 0, 0, 0)',
)
PACKAGE_SNIPPETS = (
    '"check:objc3c:m256-d001-class-realization-runtime-contract": "python scripts/check_m256_d001_class_realization_runtime_contract_and_architecture_freeze.py"',
    '"test:tooling:m256-d001-class-realization-runtime-contract": "python -m pytest tests/tooling/test_check_m256_d001_class_realization_runtime_contract_and_architecture_freeze.py -q"',
    '"check:objc3c:m256-d001-lane-d-readiness": "python scripts/run_m256_d001_lane_d_readiness.py"',
)


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    try:
        return str(path.relative_to(ROOT))
    except ValueError:
        return str(path)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, artifact: str, check_id: str, detail: str,
            failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def ensure_snippets(path: Path, snippets: Sequence[str], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    for index, snippet in enumerate(snippets, start=1):
        passed += require(
            snippet in text,
            display_path(path),
            f"{MODE}-DOC-{index:02d}",
            f"missing snippet: {snippet}",
            failures,
        )
    return passed


def resolve_tool(name: str) -> str | None:
    resolved = shutil.which(name)
    if resolved:
        return resolved
    if sys.platform == "win32":
        candidate = Path("C:/Program Files/LLVM/bin") / name
        if candidate.exists():
            return str(candidate)
        candidate = Path("C:/Program Files/LLVM/bin") / f"{name}.exe"
        if candidate.exists():
            return str(candidate)
    return None


def run_command(command: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=ROOT / "docs/contracts/m256_class_realization_runtime_contract_and_architecture_freeze_d001_expectations.md")
    parser.add_argument("--packet-doc", type=Path, default=ROOT / "spec/planning/compiler/m256/m256_d001_class_realization_runtime_contract_and_architecture_freeze_packet.md")
    parser.add_argument("--native-doc", type=Path, default=ROOT / "docs/objc3c-native.md")
    parser.add_argument("--lowering-spec", type=Path, default=ROOT / "spec/LOWERING_AND_RUNTIME_CONTRACTS.md")
    parser.add_argument("--metadata-spec", type=Path, default=ROOT / "spec/MODULE_METADATA_AND_ABI_TABLES.md")
    parser.add_argument("--architecture-doc", type=Path, default=ROOT / "native/objc3c/src/ARCHITECTURE.md")
    parser.add_argument("--runtime-readme", type=Path, default=ROOT / "native/objc3c/src/runtime/README.md")
    parser.add_argument("--tooling-runtime-readme", type=Path, default=ROOT / "tests/tooling/runtime/README.md")
    parser.add_argument("--lowering-header", type=Path, default=ROOT / "native/objc3c/src/lower/objc3_lowering_contract.h")
    parser.add_argument("--lowering-cpp", type=Path, default=ROOT / "native/objc3c/src/lower/objc3_lowering_contract.cpp")
    parser.add_argument("--parser-cpp", type=Path, default=ROOT / "native/objc3c/src/parse/objc3_parser.cpp")
    parser.add_argument("--sema-cpp", type=Path, default=ROOT / "native/objc3c/src/sema/objc3_semantic_passes.cpp")
    parser.add_argument("--ir-emitter", type=Path, default=ROOT / "native/objc3c/src/ir/objc3_ir_emitter.cpp")
    parser.add_argument("--runtime-header", type=Path, default=ROOT / "native/objc3c/src/runtime/objc3_runtime.h")
    parser.add_argument("--bootstrap-header", type=Path, default=ROOT / "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h")
    parser.add_argument("--runtime-source", type=Path, default=ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp")
    parser.add_argument("--runtime-probe", type=Path, default=ROOT / RUNTIME_PROBE_SOURCE)
    parser.add_argument("--fixture", type=Path, default=ROOT / FIXTURE_SOURCE)
    parser.add_argument("--package-json", type=Path, default=ROOT / "package.json")
    parser.add_argument("--native-exe", type=Path, default=ROOT / "artifacts/bin/objc3c-native.exe")
    parser.add_argument("--runtime-library", type=Path, default=ROOT / "artifacts/lib/objc3_runtime.lib")
    parser.add_argument("--runtime-include-root", type=Path, default=ROOT / "native/objc3c/src")
    parser.add_argument("--clangxx", default="clang++.exe")
    parser.add_argument("--probe-root", type=Path, default=ROOT / "tmp/artifacts/compilation/objc3c-native/m256/d001-class-realization-runtime")
    parser.add_argument("--summary-out", type=Path, default=ROOT / SUMMARY_RELATIVE_PATH)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(list(argv))


def boundary_line(ir_text: str, prefix: str) -> str:
    return next((line for line in ir_text.splitlines() if line.startswith(prefix)), "")


def payload_checks(probe_payload: dict[str, Any], module_manifest: dict[str, Any],
                   registration_manifest: dict[str, Any],
                   failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    artifact = "runtime_probe"

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    registration_state = probe_payload.get("registration_state", {})
    selector_state = probe_payload.get("selector_table_state", {})
    inherited_state = probe_payload.get("inherited_state", {})
    category_state = probe_payload.get("category_state", {})
    class_state = probe_payload.get("class_state", {})
    known_class_state = probe_payload.get("known_class_state", {})
    protocol_fallback_state = probe_payload.get("protocol_fallback_state", {})
    protocol_fallback_cached_state = probe_payload.get(
        "protocol_fallback_cached_state", {}
    )
    inherited_entry = probe_payload.get("inherited_entry", {})
    category_entry = probe_payload.get("category_entry", {})
    known_class_entry = probe_payload.get("known_class_entry", {})
    protocol_fallback_entry = probe_payload.get("protocol_fallback_entry", {})

    check(probe_payload.get("inherited_value") == 7,
          "M256-D001-DYN-PAY-01", "inheritedValue dispatch must resolve Base::inheritedValue => 7")
    check(probe_payload.get("category_value") == 13,
          "M256-D001-DYN-PAY-02", "tracedValue dispatch must resolve Widget(Tracing)::tracedValue => 13")
    check(probe_payload.get("class_value") == 11,
          "M256-D001-DYN-PAY-03", "classValue dispatch over 1043 must resolve Widget::classValue => 11")
    check(probe_payload.get("known_class_value") == 11,
          "M256-D001-DYN-PAY-04", "classValue dispatch over 1041 must normalize onto the metaclass path")
    check(probe_payload.get("protocol_fallback_expected") == probe_payload.get("protocol_fallback"),
          "M256-D001-DYN-PAY-05", "ignoredValue fallback must match the compatibility formula")
    check(probe_payload.get("protocol_fallback_cached") == probe_payload.get("protocol_fallback_expected"),
          "M256-D001-DYN-PAY-06", "cached ignoredValue fallback must preserve the compatibility result")

    module_name = module_manifest.get("module")
    translation_unit_key = registration_manifest.get("translation_unit_identity_key")
    total_descriptor_count = registration_manifest.get("total_descriptor_count")
    order_ordinal = registration_manifest.get("translation_unit_registration_order_ordinal")
    success_status_code = registration_manifest.get("success_status_code")

    check(registration_state.get("registered_image_count") == 1,
          "M256-D001-DYN-PAY-07", "runtime must have exactly one registered image for the probe object")
    check(registration_state.get("registered_descriptor_total") == total_descriptor_count,
          "M256-D001-DYN-PAY-08", "runtime registered descriptor total must match the registration manifest")
    check(registration_state.get("last_registration_status") == success_status_code,
          "M256-D001-DYN-PAY-09", "runtime registration snapshot must publish success status")
    check(registration_state.get("last_registered_module_name") == module_name,
          "M256-D001-DYN-PAY-10", "runtime registration snapshot must preserve the module name")
    check(registration_state.get("last_registered_translation_unit_identity_key") == translation_unit_key,
          "M256-D001-DYN-PAY-11", "runtime registration snapshot must preserve the translation-unit identity key")
    check(registration_state.get("last_successful_registration_order_ordinal") == order_ordinal,
          "M256-D001-DYN-PAY-12", "runtime registration snapshot must preserve the registration order ordinal")
    check(registration_state.get("next_expected_registration_order_ordinal") == order_ordinal + 1,
          "M256-D001-DYN-PAY-13", "runtime registration snapshot must advance the next expected ordinal")

    check(selector_state.get("selector_table_entry_count") == 5,
          "M256-D001-DYN-PAY-14", "selector table must materialize the five exercised metadata selectors")
    check(selector_state.get("metadata_backed_selector_count") == 5,
          "M256-D001-DYN-PAY-15", "selector table must report five metadata-backed selectors for the exercised surface")
    check(selector_state.get("dynamic_selector_count") == 0,
          "M256-D001-DYN-PAY-16", "selector table must not materialize dynamic selectors in the D001 proof")
    check(selector_state.get("last_materialized_selector") == "tracedValue",
          "M256-D001-DYN-PAY-17", "tracedValue should be the last metadata selector recorded by the startup materialization pass")
    check(selector_state.get("last_materialized_from_metadata") == 1,
          "M256-D001-DYN-PAY-18", "the last materialized selector must come from metadata, not dynamic interning")

    check(inherited_state.get("last_dispatch_used_cache") == 0,
          "M256-D001-DYN-PAY-19", "first inheritedValue dispatch should miss cache")
    check(inherited_state.get("last_dispatch_resolved_live_method") == 1,
          "M256-D001-DYN-PAY-20", "inheritedValue dispatch must resolve a live method")
    check(inherited_state.get("last_dispatch_fell_back") == 0,
          "M256-D001-DYN-PAY-21", "inheritedValue dispatch must not fall back")
    check(inherited_state.get("last_normalized_receiver_identity") == EXPECTED_INSTANCE_RECEIVER,
          "M256-D001-DYN-PAY-22", "instance dispatch must preserve the canonical instance receiver identity")
    check(inherited_state.get("last_category_probe_count") == 1,
          "M256-D001-DYN-PAY-23", "inheritedValue dispatch must preserve the class-local category probe count")
    check(inherited_state.get("last_protocol_probe_count") == 2,
          "M256-D001-DYN-PAY-24", "inheritedValue dispatch must preserve the inherited protocol negative-check count")
    check(inherited_state.get("last_resolved_class_name") == "Base",
          "M256-D001-DYN-PAY-25", "inheritedValue must resolve against Base")
    check(inherited_state.get("last_resolved_owner_identity") == EXPECTED_INHERITED_OWNER,
          "M256-D001-DYN-PAY-26", "inheritedValue must resolve the Base owner identity")

    check(category_state.get("last_dispatch_used_cache") == 0,
          "M256-D001-DYN-PAY-27", "first tracedValue dispatch should miss cache")
    check(category_state.get("last_dispatch_resolved_live_method") == 1,
          "M256-D001-DYN-PAY-28", "tracedValue dispatch must resolve a live method")
    check(category_state.get("last_dispatch_fell_back") == 0,
          "M256-D001-DYN-PAY-29", "tracedValue dispatch must not fall back")
    check(category_state.get("last_category_probe_count") == 1,
          "M256-D001-DYN-PAY-30", "tracedValue dispatch must report one category probe")
    check(category_state.get("last_protocol_probe_count") == 0,
          "M256-D001-DYN-PAY-31", "tracedValue dispatch must not need protocol negative probes")
    check(category_state.get("last_resolved_class_name") == "Widget",
          "M256-D001-DYN-PAY-32", "tracedValue must resolve against Widget after category attachment")
    check(category_state.get("last_resolved_owner_identity") == EXPECTED_CATEGORY_OWNER,
          "M256-D001-DYN-PAY-33", "tracedValue must resolve the category implementation owner")

    check(class_state.get("last_dispatch_used_cache") == 0,
          "M256-D001-DYN-PAY-34", "first classValue dispatch over 1043 should miss cache")
    check(class_state.get("last_dispatch_resolved_live_method") == 1,
          "M256-D001-DYN-PAY-35", "classValue dispatch over 1043 must resolve a live method")
    check(class_state.get("last_dispatch_fell_back") == 0,
          "M256-D001-DYN-PAY-36", "classValue dispatch over 1043 must not fall back")
    check(class_state.get("last_normalized_receiver_identity") == EXPECTED_METACLASS_RECEIVER,
          "M256-D001-DYN-PAY-37", "metaclass dispatch must normalize onto 1043")
    check(class_state.get("last_resolved_class_name") == "Widget",
          "M256-D001-DYN-PAY-38", "classValue over 1043 must resolve against Widget")
    check(class_state.get("last_resolved_owner_identity") == EXPECTED_CLASS_OWNER,
          "M256-D001-DYN-PAY-39", "classValue over 1043 must resolve Widget::classValue")

    check(known_class_state.get("last_dispatch_used_cache") == 1,
          "M256-D001-DYN-PAY-40", "known-class receiver dispatch must reuse the metaclass cache entry")
    check(known_class_state.get("last_normalized_receiver_identity") == EXPECTED_METACLASS_RECEIVER,
          "M256-D001-DYN-PAY-41", "known-class receiver must normalize onto 1043")
    check(known_class_state.get("last_resolved_class_name") == "Widget",
          "M256-D001-DYN-PAY-42", "known-class receiver must still resolve Widget")
    check(known_class_state.get("last_resolved_owner_identity") == EXPECTED_CLASS_OWNER,
          "M256-D001-DYN-PAY-43", "known-class receiver must reuse Widget::classValue")

    check(protocol_fallback_state.get("last_dispatch_used_cache") == 0,
          "M256-D001-DYN-PAY-44", "first ignoredValue dispatch should miss cache")
    check(protocol_fallback_state.get("last_dispatch_resolved_live_method") == 0,
          "M256-D001-DYN-PAY-45", "ignoredValue must remain unresolved")
    check(protocol_fallback_state.get("last_dispatch_fell_back") == 1,
          "M256-D001-DYN-PAY-46", "ignoredValue must fall back compatibly")
    check(protocol_fallback_state.get("last_category_probe_count") == 1,
          "M256-D001-DYN-PAY-47", "ignoredValue must preserve category probe evidence")
    check(protocol_fallback_state.get("last_protocol_probe_count") == 2,
          "M256-D001-DYN-PAY-48", "ignoredValue must preserve protocol probe evidence")

    check(protocol_fallback_cached_state.get("last_dispatch_used_cache") == 1,
          "M256-D001-DYN-PAY-49", "second ignoredValue dispatch must use the negative cache entry")
    check(protocol_fallback_cached_state.get("last_dispatch_resolved_live_method") == 0,
          "M256-D001-DYN-PAY-50", "cached ignoredValue dispatch must remain unresolved")
    check(protocol_fallback_cached_state.get("last_dispatch_fell_back") == 1,
          "M256-D001-DYN-PAY-51", "cached ignoredValue dispatch must still fall back compatibly")
    check(protocol_fallback_cached_state.get("last_category_probe_count") == 1,
          "M256-D001-DYN-PAY-52", "cached ignoredValue dispatch must preserve category probe count")
    check(protocol_fallback_cached_state.get("last_protocol_probe_count") == 2,
          "M256-D001-DYN-PAY-53", "cached ignoredValue dispatch must preserve protocol probe count")

    check(inherited_entry.get("found") == 1 and inherited_entry.get("resolved") == 1,
          "M256-D001-DYN-PAY-54", "inheritedValue cache entry must be materialized and resolved")
    check(inherited_entry.get("normalized_receiver_identity") == EXPECTED_INSTANCE_RECEIVER,
          "M256-D001-DYN-PAY-55", "inheritedValue cache entry must store 1042")
    check(inherited_entry.get("category_probe_count") == 1,
          "M256-D001-DYN-PAY-56", "inheritedValue cache entry must preserve category probe count")
    check(inherited_entry.get("protocol_probe_count") == 2,
          "M256-D001-DYN-PAY-57", "inheritedValue cache entry must preserve protocol probe count")
    check(inherited_entry.get("resolved_class_name") == "Base",
          "M256-D001-DYN-PAY-58", "inheritedValue cache entry must point at Base")
    check(inherited_entry.get("resolved_owner_identity") == EXPECTED_INHERITED_OWNER,
          "M256-D001-DYN-PAY-59", "inheritedValue cache entry must point at the Base owner")

    check(category_entry.get("found") == 1 and category_entry.get("resolved") == 1,
          "M256-D001-DYN-PAY-60", "tracedValue cache entry must be materialized and resolved")
    check(category_entry.get("normalized_receiver_identity") == EXPECTED_INSTANCE_RECEIVER,
          "M256-D001-DYN-PAY-61", "tracedValue cache entry must store 1042")
    check(category_entry.get("category_probe_count") == 1,
          "M256-D001-DYN-PAY-62", "tracedValue cache entry must store the category probe count")
    check(category_entry.get("protocol_probe_count") == 0,
          "M256-D001-DYN-PAY-63", "tracedValue cache entry must store zero protocol probes")
    check(category_entry.get("resolved_class_name") == "Widget",
          "M256-D001-DYN-PAY-64", "tracedValue cache entry must point at Widget")
    check(category_entry.get("resolved_owner_identity") == EXPECTED_CATEGORY_OWNER,
          "M256-D001-DYN-PAY-65", "tracedValue cache entry must point at the category owner")

    check(known_class_entry.get("found") == 1 and known_class_entry.get("resolved") == 1,
          "M256-D001-DYN-PAY-66", "known-class classValue cache entry must be materialized and resolved")
    check(known_class_entry.get("dispatch_family_is_class") == 1,
          "M256-D001-DYN-PAY-67", "known-class classValue entry must be classified as class-family dispatch")
    check(known_class_entry.get("normalized_receiver_identity") == EXPECTED_METACLASS_RECEIVER,
          "M256-D001-DYN-PAY-68", "known-class classValue entry must normalize onto 1043")
    check(known_class_entry.get("resolved_class_name") == "Widget",
          "M256-D001-DYN-PAY-69", "known-class classValue entry must resolve against Widget")
    check(known_class_entry.get("resolved_owner_identity") == EXPECTED_CLASS_OWNER,
          "M256-D001-DYN-PAY-70", "known-class classValue entry must point at Widget::classValue")

    check(protocol_fallback_entry.get("found") == 1,
          "M256-D001-DYN-PAY-71", "ignoredValue negative cache entry must exist")
    check(protocol_fallback_entry.get("resolved") == 0,
          "M256-D001-DYN-PAY-72", "ignoredValue negative cache entry must remain unresolved")
    check(protocol_fallback_entry.get("dispatch_family_is_class") == 0,
          "M256-D001-DYN-PAY-73", "ignoredValue negative cache entry must remain instance-family")
    check(protocol_fallback_entry.get("normalized_receiver_identity") == EXPECTED_INSTANCE_RECEIVER,
          "M256-D001-DYN-PAY-74", "ignoredValue negative cache entry must store 1042")
    check(protocol_fallback_entry.get("category_probe_count") == 1,
          "M256-D001-DYN-PAY-75", "ignoredValue negative cache entry must preserve category probe count")
    check(protocol_fallback_entry.get("protocol_probe_count") == 2,
          "M256-D001-DYN-PAY-76", "ignoredValue negative cache entry must preserve protocol probe count")
    check(protocol_fallback_entry.get("selector") == "ignoredValue",
          "M256-D001-DYN-PAY-77", "ignoredValue negative cache entry must preserve selector spelling")

    return checks_passed, checks_total


def run_dynamic_case(args: argparse.Namespace,
                     failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    artifact = "dynamic_case"

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    check(args.native_exe.exists(), "M256-D001-DYN-01",
          f"missing native executable: {display_path(args.native_exe)}")
    check(args.runtime_library.exists(), "M256-D001-DYN-02",
          f"missing runtime library: {display_path(args.runtime_library)}")
    check(args.fixture.exists(), "M256-D001-DYN-03",
          f"missing fixture: {display_path(args.fixture)}")
    check(args.runtime_probe.exists(), "M256-D001-DYN-04",
          f"missing runtime probe: {display_path(args.runtime_probe)}")

    clangxx = resolve_tool(args.clangxx)
    check(clangxx is not None, "M256-D001-DYN-05",
          f"unable to resolve {args.clangxx}")
    if failures:
        return checks_passed, checks_total, {"skipped": False}

    probe_dir = args.probe_root.resolve() / f"probe-{uuid.uuid4().hex}"
    probe_dir.mkdir(parents=True, exist_ok=True)

    compile_result = run_command(
        [
            str(args.native_exe),
            str(args.fixture),
            "--out-dir",
            str(probe_dir),
            "--emit-prefix",
            "module",
        ],
        ROOT,
    )
    check(compile_result.returncode == 0, "M256-D001-DYN-06",
          f"fixture compile failed: {compile_result.stdout}{compile_result.stderr}")

    module_manifest_path = probe_dir / "module.manifest.json"
    runtime_manifest_path = probe_dir / "module.runtime-registration-manifest.json"
    module_ir = probe_dir / "module.ll"
    module_obj = probe_dir / "module.obj"
    backend_path = probe_dir / "module.object-backend.txt"
    check(module_manifest_path.exists(), "M256-D001-DYN-07",
          f"missing manifest: {display_path(module_manifest_path)}")
    check(runtime_manifest_path.exists(), "M256-D001-DYN-08",
          f"missing runtime registration manifest: {display_path(runtime_manifest_path)}")
    check(module_ir.exists(), "M256-D001-DYN-09",
          f"missing emitted IR: {display_path(module_ir)}")
    check(module_obj.exists(), "M256-D001-DYN-10",
          f"missing emitted object: {display_path(module_obj)}")
    check(backend_path.exists(), "M256-D001-DYN-11",
          f"missing backend marker: {display_path(backend_path)}")
    if failures:
        return checks_passed, checks_total, {
            "skipped": False,
            "compile_stdout": compile_result.stdout,
            "compile_stderr": compile_result.stderr,
        }

    module_manifest = json.loads(read_text(module_manifest_path))
    runtime_manifest = json.loads(read_text(runtime_manifest_path))
    backend_text = read_text(backend_path).strip()
    check(backend_text == "llvm-direct", "M256-D001-DYN-12",
          f"module.object-backend.txt must be llvm-direct, saw {backend_text!r}")

    ir_text = read_text(module_ir)
    executable_boundary_line = boundary_line(ir_text, REALIZATION_PREFIX)
    runtime_boundary_line = boundary_line(ir_text, BOUNDARY_PREFIX)
    check(bool(executable_boundary_line), "M256-D001-DYN-13",
          "IR must retain the executable realization-record summary")
    check(bool(runtime_boundary_line), "M256-D001-DYN-14",
          "IR must publish the runtime class realization summary")
    for check_id, token in (
        ("M256-D001-DYN-15", CONTRACT_ID),
        ("M256-D001-DYN-16", CLASS_REALIZATION_MODEL),
        ("M256-D001-DYN-17", METACLASS_GRAPH_MODEL),
        ("M256-D001-DYN-18", CATEGORY_ATTACHMENT_MODEL),
        ("M256-D001-DYN-19", PROTOCOL_CHECK_MODEL),
        ("M256-D001-DYN-20", FAIL_CLOSED_MODEL),
        ("M256-D001-DYN-21", "class_bundle_count=4"),
        ("M256-D001-DYN-22", "protocol_record_count=2"),
        ("M256-D001-DYN-23", "category_record_count=2"),
    ):
        check(token in runtime_boundary_line, check_id,
              f"runtime boundary line missing token: {token}")

    probe_exe = probe_dir / "m256_d001_class_realization_runtime_probe.exe"
    probe_compile = run_command(
        [
            str(clangxx),
            "-std=c++20",
            "-I",
            str(args.runtime_include_root),
            str(args.runtime_probe),
            str(module_obj),
            str(args.runtime_library),
            "-o",
            str(probe_exe),
        ],
        ROOT,
    )
    check(probe_compile.returncode == 0, "M256-D001-DYN-24",
          f"probe compile failed: {probe_compile.stdout}{probe_compile.stderr}")
    if probe_compile.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_compile_stdout": probe_compile.stdout,
            "probe_compile_stderr": probe_compile.stderr,
        }

    probe_run = run_command([str(probe_exe)], ROOT)
    check(probe_run.returncode == 0, "M256-D001-DYN-25",
          f"probe execution failed: {probe_run.stdout}{probe_run.stderr}")
    if probe_run.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_stdout": probe_run.stdout,
            "probe_stderr": probe_run.stderr,
        }

    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(artifact, "M256-D001-DYN-26",
                                f"probe output is not valid JSON: {exc}"))
        return checks_passed, checks_total + 1, {
            "skipped": False,
            "probe_stdout": probe_run.stdout,
        }
    checks_total += 1
    checks_passed += 1

    payload_passed, payload_total = payload_checks(
        probe_payload, module_manifest, runtime_manifest, failures
    )
    checks_passed += payload_passed
    checks_total += payload_total

    return checks_passed, checks_total, {
        "skipped": False,
        "probe_dir": display_path(probe_dir),
        "module_manifest": display_path(module_manifest_path),
        "runtime_manifest": display_path(runtime_manifest_path),
        "module_ir": display_path(module_ir),
        "module_obj": display_path(module_obj),
        "probe_exe": display_path(probe_exe),
        "executable_boundary_line": executable_boundary_line,
        "runtime_boundary_line": runtime_boundary_line,
        "probe_payload": probe_payload,
    }


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_groups = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.runtime_readme, RUNTIME_README_SNIPPETS),
        (args.tooling_runtime_readme, TOOLING_RUNTIME_README_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.parser_cpp, PARSER_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.runtime_header, RUNTIME_HEADER_SNIPPETS),
        (args.bootstrap_header, BOOTSTRAP_HEADER_SNIPPETS),
        (args.runtime_source, RUNTIME_CPP_SNIPPETS),
        (args.runtime_probe, PROBE_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_groups:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, Any]] = []
    if not args.skip_dynamic_probes:
        dynamic_passed, dynamic_total, dynamic_case = run_dynamic_case(
            args, failures
        )
        checks_passed += dynamic_passed
        checks_total += dynamic_total
        dynamic_cases.append(dynamic_case)

    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_cases": dynamic_cases,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        print(f"[fail] {MODE} ({checks_passed}/{checks_total} checks passed)", file=sys.stderr)
        for finding in failures:
            print(f"- {finding.check_id} [{finding.artifact}] {finding.detail}", file=sys.stderr)
        print(f"[info] summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print(f"[ok] {MODE} ({checks_passed}/{checks_total} checks passed)")
    print(f"[info] summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
