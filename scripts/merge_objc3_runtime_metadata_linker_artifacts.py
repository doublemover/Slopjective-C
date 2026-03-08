from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
SOURCE_CONTRACT_ID = "objc3c-runtime-linker-retention-and-dead-strip-resistance/m253-d002-v1"
MERGED_CONTRACT_ID = "objc3c-runtime-metadata-archive-and-static-link-discovery/m253-d003-v1"
TRANSLATION_UNIT_IDENTITY_MODEL = "input-path-plus-parse-and-lowering-replay"
MERGE_MODEL = "deduplicated-driver-flag-fan-in"
RESPONSE_SUFFIX = ".merged.runtime-metadata-linker-options.rsp"
DISCOVERY_SUFFIX = ".merged.runtime-metadata-discovery.json"


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Merge objc3 runtime metadata discovery artifacts into one canonical response/discovery pair."
    )
    parser.add_argument("discovery_files", nargs="+", type=Path)
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--emit-prefix", default="module")
    parser.add_argument("--response-out", type=Path)
    parser.add_argument("--discovery-out", type=Path)
    return parser.parse_args(argv)



def canonical_json(value: Any) -> str:
    return json.dumps(value, indent=2, sort_keys=True) + "\n"



def display_path(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT).as_posix()
    except ValueError:
        return path.resolve().as_posix()



def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))



def require_string(payload: dict[str, Any], key: str, source: Path) -> str:
    value = payload.get(key)
    if not isinstance(value, str) or not value:
        raise ValueError(f"{display_path(source)} missing non-empty string field {key!r}")
    return value



def require_string_list(payload: dict[str, Any], key: str, source: Path) -> list[str]:
    value = payload.get(key)
    if not isinstance(value, list) or not value or any(not isinstance(item, str) or not item for item in value):
        raise ValueError(f"{display_path(source)} missing non-empty string-list field {key!r}")
    return list(value)



def unique_in_order(values: list[str]) -> list[str]:
    seen: set[str] = set()
    ordered: list[str] = []
    for value in values:
        if value in seen:
            continue
        seen.add(value)
        ordered.append(value)
    return ordered



def normalize_record(source: Path) -> dict[str, Any]:
    payload = load_json(source)
    if not isinstance(payload, dict):
        raise ValueError(f"{display_path(source)} must contain a JSON object")
    contract_id = require_string(payload, "contract_id", source)
    if contract_id != SOURCE_CONTRACT_ID:
        raise ValueError(
            f"{display_path(source)} contract_id mismatch: expected {SOURCE_CONTRACT_ID!r}, got {contract_id!r}"
        )
    translation_model = require_string(payload, "translation_unit_identity_model", source)
    if translation_model != TRANSLATION_UNIT_IDENTITY_MODEL:
        raise ValueError(
            f"{display_path(source)} translation_unit_identity_model mismatch: "
            f"expected {TRANSLATION_UNIT_IDENTITY_MODEL!r}, got {translation_model!r}"
        )
    return {
        "source_path": display_path(source),
        "contract_id": contract_id,
        "object_format": require_string(payload, "object_format", source),
        "object_artifact": require_string(payload, "object_artifact", source),
        "linker_anchor_symbol": require_string(payload, "linker_anchor_symbol", source),
        "discovery_root_symbol": require_string(payload, "discovery_root_symbol", source),
        "linker_anchor_logical_section": require_string(payload, "linker_anchor_logical_section", source),
        "discovery_root_logical_section": require_string(payload, "discovery_root_logical_section", source),
        "linker_anchor_emitted_section": require_string(payload, "linker_anchor_emitted_section", source),
        "discovery_root_emitted_section": require_string(payload, "discovery_root_emitted_section", source),
        "translation_unit_identity_model": translation_model,
        "translation_unit_identity_key": require_string(payload, "translation_unit_identity_key", source),
        "driver_linker_flags": require_string_list(payload, "driver_linker_flags", source),
    }



def build_response_path(args: argparse.Namespace) -> Path:
    if args.response_out is not None:
        return args.response_out.resolve()
    return args.out_dir.resolve() / f"{args.emit_prefix}{RESPONSE_SUFFIX}"



def build_discovery_path(args: argparse.Namespace) -> Path:
    if args.discovery_out is not None:
        return args.discovery_out.resolve()
    return args.out_dir.resolve() / f"{args.emit_prefix}{DISCOVERY_SUFFIX}"



def run(argv: list[str]) -> int:
    args = parse_args(argv)
    args.out_dir.resolve().mkdir(parents=True, exist_ok=True)

    records = [normalize_record(path.resolve()) for path in args.discovery_files]
    object_formats = unique_in_order([record["object_format"] for record in records])
    if len(object_formats) != 1:
        raise ValueError(f"mixed object formats are not mergeable: {object_formats}")

    anchor_symbols = unique_in_order([record["linker_anchor_symbol"] for record in records])
    discovery_symbols = unique_in_order([record["discovery_root_symbol"] for record in records])
    translation_keys = unique_in_order([record["translation_unit_identity_key"] for record in records])
    response_flags = unique_in_order(
        [flag for record in records for flag in record["driver_linker_flags"]]
    )
    if len(anchor_symbols) != len(records):
        raise ValueError(
            "linker-anchor collision detected across discovery inputs; "
            "D003 merge requires translation-unit-stable public anchors"
        )
    if len(discovery_symbols) != len(records):
        raise ValueError(
            "discovery-root collision detected across discovery inputs; "
            "D003 merge requires translation-unit-stable public discovery roots"
        )
    if len(translation_keys) != len(records):
        raise ValueError(
            "translation-unit identity collision detected across discovery inputs"
        )

    response_path = build_response_path(args)
    discovery_path = build_discovery_path(args)
    response_path.parent.mkdir(parents=True, exist_ok=True)
    discovery_path.parent.mkdir(parents=True, exist_ok=True)

    response_path.write_text("\n".join(response_flags) + "\n", encoding="utf-8")
    payload = {
        "contract_id": MERGED_CONTRACT_ID,
        "source_contract_id": SOURCE_CONTRACT_ID,
        "merge_model": MERGE_MODEL,
        "translation_unit_identity_model": TRANSLATION_UNIT_IDENTITY_MODEL,
        "object_format": object_formats[0],
        "merged_input_count": len(records),
        "response_artifact": response_path.name,
        "discovery_artifact": discovery_path.name,
        "driver_linker_flags": response_flags,
        "linker_anchor_symbols": anchor_symbols,
        "discovery_root_symbols": discovery_symbols,
        "translation_unit_identity_keys": translation_keys,
        "inputs": records,
    }
    discovery_path.write_text(canonical_json(payload), encoding="utf-8")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(run(sys.argv[1:]))
    except ValueError as exc:
        print(f"merge_objc3_runtime_metadata_linker_artifacts: {exc}", file=sys.stderr)
        raise SystemExit(1)
