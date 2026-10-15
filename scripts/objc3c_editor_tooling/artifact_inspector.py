from __future__ import annotations

import hashlib
import json
import re
import subprocess
from pathlib import Path
from typing import Any

from objc3c_tooling.llvm_discovery import find_llvm_tool_path
from objc3c_tooling.paths import ROOT, display_path, resolve_repo_path

from objc3c_editor_tooling.input_loading import EditorToolingInputs
from objc3c_editor_tooling.paths import EditorToolingPaths


ARTIFACT_EXPLANATIONS = {
    "summary": "compile summary with stage status and emitted artifact paths",
    "diagnostics": "structured diagnostics for editor squiggles and fix-it routing",
    "manifest": "manifest-backed declarations and source graph inputs for navigation",
    "ir": "lowered LLVM IR text emitted by the canonical frontend path",
    "object": "native object artifact exposed through runtime inspector commands",
    "runtime_metadata_binary": "runtime metadata import payload emitted by lowering",
}

TEXT_ARTIFACT_KINDS = {"summary", "diagnostics", "manifest", "ir"}
SOURCE_GRAPH_KEYS = (
    "source_graph",
    "sourceGraph",
    "source_graph_readiness",
    "frontend_source_graph",
)
RUNTIME_SYMBOL_MARKERS = ("objc3_", "objc_", "__objc", "runtime")
IMPORTED_SYMBOL_MARKERS = {
    "import",
    "imported",
    "undefined",
    "undef",
    "u",
}
EXPORTED_SYMBOL_MARKERS = {
    "export",
    "exported",
    "defined",
    "definition",
    "external",
    "extern",
    "global",
    "public",
    "t",
}
OBJECT_FORMATS_WITH_SYMBOL_TABLES = {"elf", "mach-o", "coff"}
OBJECT_MODEL_NATIVE_DEBUG_INFO_EVIDENCE_CONTRACT_ID = (
    "objc3c.object_model.production.native_debug_info_evidence.v1"
)
NATIVE_DEBUG_SECTION_PREFIXES = (".debug", ".zdebug", "__debug")
NATIVE_LINE_TABLE_SECTION_NAMES = {
    ".debug_line",
    ".zdebug_line",
    "__debug_line",
    ".debug$s",
}
DEFAULT_OBJC3_ABI_IDENTITY = "objc3-abi-2025Q4"
LLVM_DEBUG_LOCATION_ATTACHMENT_PATTERN = re.compile(
    r"(?:^|,\s*)!dbg\s+![0-9]+\b"
)
LLVM_NON_INSTRUCTION_DEBUG_LOCATION_PREFIXES = (
    "!",
    "@",
    "attributes ",
    "comdat ",
    "declare ",
    "define ",
    "module ",
    "source_filename",
    "target ",
    "uselistorder",
    "{",
    "}",
)


def _summary_paths(summary: dict[str, Any]) -> dict[str, Any]:
    paths = summary.get("paths", {})
    return paths if isinstance(paths, dict) else {}


def _runtime_inspector(summary: dict[str, Any]) -> dict[str, Any]:
    runtime_inspector = summary.get("runtime_inspector", {})
    return runtime_inspector if isinstance(runtime_inspector, dict) else {}


def _dump_commands(summary: dict[str, Any]) -> dict[str, Any]:
    observability = summary.get("observability", {})
    if not isinstance(observability, dict):
        return {}
    commands = observability.get("dump_commands", {})
    return commands if isinstance(commands, dict) else {}


def _runtime_dump_commands(summary: dict[str, Any]) -> dict[str, Any]:
    commands = _runtime_inspector(summary).get("dump_commands", {})
    return commands if isinstance(commands, dict) else {}


def _stable_digest_for_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _powershell_quote(value: str) -> str:
    return "'" + value.replace("'", "''") + "'"


def _resolve_tool(name: str) -> Path | None:
    discovered = find_llvm_tool_path(name)
    if discovered:
        return discovered
    exe_name = f"{name}.exe" if not name.endswith(".exe") else name
    candidate = ROOT / "artifacts" / "bin" / exe_name
    if candidate.is_file():
        return candidate
    return None


def _display_tool_command(tool: Path | None, args: list[str]) -> str:
    if tool is None:
        return ""
    return " ".join([_powershell_quote(str(tool)), *[_powershell_quote(arg) for arg in args]])


def _run_tool(tool_name: str, args: list[str]) -> tuple[str, list[str], str]:
    tool = _resolve_tool(tool_name)
    display_command = _display_tool_command(tool, args)
    if tool is None:
        return "", [], f"{tool_name} was not found on PATH or in the bundled LLVM tool paths"
    try:
        result = subprocess.run(
            [str(tool), *args],
            cwd=ROOT,
            check=False,
            text=True,
            capture_output=True,
        )
    except OSError as exc:
        return display_command, [], f"{tool_name} failed to start: {exc}"
    output = result.stdout if result.stdout else result.stderr
    if result.returncode != 0:
        detail = " ".join(output.split())[:240]
        return display_command, [], f"{tool_name} exited with {result.returncode}: {detail}"
    return display_command, output.splitlines(), ""


def _load_json_object_if_present(path_text: str | None) -> dict[str, Any]:
    if not path_text:
        return {}
    path = resolve_repo_path(path_text)
    if not path.is_file():
        return {}
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError, UnicodeDecodeError):
        return {}
    return payload if isinstance(payload, dict) else {}


def _as_list(value: Any) -> list[Any]:
    return value if isinstance(value, list) else []


def _as_dict(value: Any) -> dict[str, Any]:
    return value if isinstance(value, dict) else {}


def _stable_json_value(value: Any) -> Any:
    if isinstance(value, dict):
        return {
            str(key): _stable_json_value(value[key])
            for key in sorted(value, key=lambda item: str(item))
        }
    if isinstance(value, list):
        return [_stable_json_value(item) for item in value]
    if isinstance(value, str):
        return " ".join(value.split())
    if isinstance(value, (int, float, bool)) or value is None:
        return value
    return str(value)


def _stable_json_sort_key(value: Any) -> str:
    return json.dumps(_stable_json_value(value), sort_keys=True, separators=(",", ":"))


def _stable_inventory_digest(value: Any) -> str:
    return hashlib.sha256(
        json.dumps(
            _stable_json_value(value),
            sort_keys=True,
            separators=(",", ":"),
        ).encode("utf-8")
    ).hexdigest()


def _stable_inventory_list(value: Any) -> list[Any]:
    return sorted(
        [_stable_json_value(item) for item in _as_list(value)],
        key=_stable_json_sort_key,
    )


def _detect_object_format(path: Path) -> str:
    try:
        data = path.read_bytes()[:8]
    except OSError:
        return "unavailable"
    if data.startswith(b"\x7fELF"):
        return "elf"
    if data[:4] in {
        b"\xfe\xed\xfa\xce",
        b"\xce\xfa\xed\xfe",
        b"\xfe\xed\xfa\xcf",
        b"\xcf\xfa\xed\xfe",
        b"\xca\xfe\xba\xbe",
        b"\xbe\xba\xfe\xca",
    }:
        return "mach-o"
    if len(data) >= 2 and data[:2] in {b"\x64\x86", b"\x4c\x01"}:
        return "coff"
    if data.startswith(b"!<arch>\n"):
        return "archive"
    if data.startswith(b"BC\xc0\xde"):
        return "llvm-bitcode"
    return "unsupported"


def _symbol_name(symbol: Any) -> str:
    if isinstance(symbol, str):
        parts = symbol.strip().split()
        return parts[-1] if parts else ""
    if isinstance(symbol, dict):
        for key in ("name", "symbol", "display_name"):
            value = symbol.get(key)
            if isinstance(value, str) and value:
                return value
    return ""


def _is_runtime_symbol(symbol: Any) -> bool:
    name = _symbol_name(symbol).lower()
    return any(marker in name for marker in RUNTIME_SYMBOL_MARKERS)


def _symbol_field_text(symbol: dict[str, Any], keys: tuple[str, ...]) -> str:
    values = []
    for key in keys:
        value = symbol.get(key)
        if value is not None:
            values.append(str(value).lower())
    return " ".join(values)


def _is_imported_symbol(symbol: Any) -> bool:
    if isinstance(symbol, str):
        text = symbol.strip()
        fields = text.split()
        return bool(
            fields
            and (
                fields[0].upper() == "U"
                or (len(fields) > 1 and fields[-2].upper() == "U")
            )
        )
    if not isinstance(symbol, dict):
        return False
    if symbol.get("defined") is False or symbol.get("undefined") is True:
        return True
    text = _symbol_field_text(
        symbol,
        ("binding", "linkage", "scope", "storage_class", "section", "kind", "type"),
    )
    return any(marker in text.split() for marker in IMPORTED_SYMBOL_MARKERS)


def _is_exported_symbol(symbol: Any) -> bool:
    if isinstance(symbol, str):
        return _is_runtime_symbol(symbol) and not _is_imported_symbol(symbol)
    if not isinstance(symbol, dict):
        return False
    if _is_imported_symbol(symbol):
        return False
    if symbol.get("defined") is True or symbol.get("exported") is True:
        return True
    text = _symbol_field_text(
        symbol,
        ("binding", "linkage", "scope", "storage_class", "visibility", "kind", "type"),
    )
    return not text or any(marker in text.split() for marker in EXPORTED_SYMBOL_MARKERS)


def _symbol_record_from_nm_line(line: str) -> dict[str, Any] | None:
    text = " ".join(line.strip().split())
    if not text or text.endswith(":") or text.startswith(("llvm-nm:", "Archive map")):
        return None
    parts = text.split()
    name = ""
    type_token = ""
    address = ""
    size = ""
    if len(parts) >= 4 and re.fullmatch(r"[A-Za-z?_-]", parts[1]):
        name, type_token, address, size = parts[0], parts[1], parts[2], parts[3]
    elif len(parts) >= 3 and re.fullmatch(r"[A-Za-z?_-]", parts[1]):
        name, type_token, address = parts[0], parts[1], parts[2]
    elif len(parts) == 2 and re.fullmatch(r"[A-Za-z?_-]", parts[1]):
        name, type_token = parts[0], parts[1]
    elif len(parts) >= 2 and re.fullmatch(r"[A-Za-z?_-]", parts[0]):
        type_token, name = parts[0], parts[-1]
    elif len(parts) >= 2 and re.fullmatch(r"[A-Za-z?_-]", parts[-2]):
        address, type_token, name = parts[-3] if len(parts) >= 3 else "", parts[-2], parts[-1]
    if not name or name.lower() in {"name", "symbol"}:
        return None
    type_upper = type_token.upper()
    imported = type_upper == "U"
    binding = "imported" if imported else "exported" if type_upper.isupper() else "local"
    record: dict[str, Any] = {
        "name": name,
        "type": type_token,
        "binding": binding,
        "defined": not imported,
        "undefined": imported,
    }
    if address and address != "-":
        record["address"] = address
    if size and size != "-":
        record["size"] = size
    return record


def _section_record_from_objdump_line(line: str) -> dict[str, Any] | None:
    parts = line.strip().split()
    if len(parts) < 3 or not parts[0].isdigit():
        return None
    name = parts[1]
    size = parts[2]
    if name.lower() == "name":
        return None
    record: dict[str, Any] = {"name": name, "size": size}
    if len(parts) >= 4:
        record["vma"] = parts[3]
    if len(parts) >= 5:
        record["lma"] = parts[4]
    if len(parts) >= 6:
        record["file_offset"] = parts[5]
    return record


def _inspect_object_inventory(record: dict[str, Any]) -> dict[str, Any]:
    if record.get("available") is not True:
        return {"symbols": [], "sections": [], "diagnostics": [], "commands": {}}
    path = str(record["path"])
    symbol_command, symbol_lines, symbol_error = _run_tool("llvm-nm", ["--format=posix", path])
    section_command, section_lines, section_error = _run_tool("llvm-objdump", ["-h", path])
    symbols = [
        symbol
        for line in symbol_lines
        for symbol in [_symbol_record_from_nm_line(line)]
        if symbol is not None
    ]
    sections = [
        section
        for line in section_lines
        for section in [_section_record_from_objdump_line(line)]
        if section is not None
    ]
    diagnostics = [error for error in (symbol_error, section_error) if error]
    return {
        "symbols": _stable_inventory_list(symbols),
        "sections": _stable_inventory_list(sections),
        "diagnostics": sorted(diagnostics),
        "commands": {
            "object_symbols": symbol_command,
            "object_sections": section_command,
        },
    }


def _runtime_inventory_entries(inventory: dict[str, Any], *keys: str) -> list[Any]:
    for key in keys:
        entries = _stable_inventory_list(inventory.get(key))
        if entries:
            return entries
    return []


def _unique_records(records: list[Any]) -> list[Any]:
    return sorted(
        {_stable_json_sort_key(record): _stable_json_value(record) for record in records}.values(),
        key=_stable_json_sort_key,
    )


def _manifest_runtime_inventory(manifest: dict[str, Any]) -> dict[str, Any]:
    classes: list[Any] = []
    selectors: list[Any] = []
    methods: list[Any] = []
    properties: list[Any] = []
    protocols: list[Any] = []
    categories: list[Any] = []

    for entry in _as_list(manifest.get("interfaces")):
        if isinstance(entry, dict) and isinstance(entry.get("name"), str) and entry["name"]:
            classes.append({"name": entry["name"], "kind": "interface"})
            properties.extend(_as_list(entry.get("properties")))
            methods.extend(_as_list(entry.get("methods")))
    for entry in _as_list(manifest.get("implementations")):
        if isinstance(entry, dict) and isinstance(entry.get("class_name"), str) and entry["class_name"]:
            classes.append({"name": entry["class_name"], "kind": "implementation"})
            methods.extend(_as_list(entry.get("methods")))
    for entry in _as_list(manifest.get("protocols")):
        if isinstance(entry, dict) and isinstance(entry.get("name"), str) and entry["name"]:
            protocols.append({"name": entry["name"]})
            methods.extend(_as_list(entry.get("methods")))
    for entry in _as_list(manifest.get("categories")):
        if not isinstance(entry, dict):
            continue
        class_name = str(entry.get("class_name", "") or "")
        category_name = str(entry.get("category_name", "") or entry.get("name", "") or "")
        if class_name or category_name:
            categories.append({"class_name": class_name, "name": category_name})
        methods.extend(_as_list(entry.get("methods")))
        properties.extend(_as_list(entry.get("properties")))

    for method in methods:
        if not isinstance(method, dict):
            continue
        selector = method.get("selector") or method.get("name")
        if isinstance(selector, str) and selector:
            selectors.append({"name": selector})

    source_records = _as_dict(manifest.get("runtime_metadata_source_records"))
    if source_records:
        classes.extend(_as_list(source_records.get("classes")))
        selectors.extend(_as_list(source_records.get("selectors")))
        methods.extend(_as_list(source_records.get("methods")))
        properties.extend(_as_list(source_records.get("properties")))
        protocols.extend(_as_list(source_records.get("protocols")))
        categories.extend(_as_list(source_records.get("categories")))

    inventory: dict[str, Any] = {}
    if classes:
        inventory["classes"] = _unique_records(classes)
    if selectors:
        inventory["selectors"] = _unique_records(selectors)
    if methods:
        inventory["methods"] = _unique_records(methods)
    if properties:
        inventory["properties"] = _unique_records(properties)
    if protocols:
        inventory["protocols"] = _unique_records(protocols)
    if categories:
        inventory["categories"] = _unique_records(categories)
    if inventory:
        inventory["reflection_abi_version"] = "manifest-derived-runtime-metadata"
    return inventory


def _runtime_metadata_file_inventory(record: dict[str, Any]) -> dict[str, Any]:
    if record.get("available") is not True:
        return {}
    path = resolve_repo_path(str(record["path"]))
    artifact = {
        "kind": "runtime_metadata_binary",
        "path": record["path"],
        "size_bytes": record["size_bytes"],
        "sha256": record["sha256"],
    }
    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        return {"runtime_artifacts": [artifact]}
    except OSError:
        return {}
    try:
        payload = json.loads(text)
    except json.JSONDecodeError:
        payload = {}
    if isinstance(payload, dict):
        inventory = dict(payload)
        artifacts = _as_list(inventory.get("runtime_artifacts"))
        inventory["runtime_artifacts"] = _unique_records([*artifacts, artifact])
        return inventory
    return {
        "reflection_abi_version": text.strip().splitlines()[0] if text.strip() else "",
        "runtime_artifacts": [artifact],
    }


def _summary_runtime_inventory(summary: dict[str, Any]) -> dict[str, Any]:
    runtime_inspector = _runtime_inspector(summary)
    for key in ("runtime_inventory", "runtime_object_metadata", "runtime_metadata"):
        value = runtime_inspector.get(key)
        if isinstance(value, dict):
            return value
    for key in ("runtime_inventory", "runtime_object_metadata", "runtime_metadata"):
        value = summary.get(key)
        if isinstance(value, dict):
            return value
    return {}


def _summary_object_inventory(summary: dict[str, Any]) -> dict[str, Any]:
    runtime_inspector = _runtime_inspector(summary)
    for key in ("object_inventory", "object_file_inventory"):
        value = runtime_inspector.get(key)
        if isinstance(value, dict):
            return value
    for key in ("object_inventory", "object_file_inventory"):
        value = summary.get(key)
        if isinstance(value, dict):
            return value
    return {}


def _digest_expectation(summary: dict[str, Any], record: dict[str, Any]) -> str:
    object_inventory = _summary_object_inventory(summary)
    for key in ("sha256", "object_sha256", "digest"):
        value = object_inventory.get(key)
        if isinstance(value, str) and value:
            return value.removeprefix("sha256:")
    summary_paths = _summary_paths(summary)
    for key in ("object_sha256", "object_digest"):
        value = summary_paths.get(key)
        if isinstance(value, str) and value:
            return value.removeprefix("sha256:")
    expected_digests = _as_dict(summary.get("expected_artifact_digests"))
    for key in ("object", record.get("path", "")):
        value = expected_digests.get(key)
        if isinstance(value, str) and value:
            return value.removeprefix("sha256:")
    return ""


def _identity_from_payload(payload: dict[str, Any], keys: tuple[str, ...]) -> str:
    for key in keys:
        value = payload.get(key)
        if isinstance(value, str) and value:
            return value
    package = payload.get("package")
    if isinstance(package, dict):
        for key in keys:
            value = package.get(key)
            if isinstance(value, str) and value:
                return value
    abi = payload.get("abi")
    if isinstance(abi, dict) and "abi_identity" in keys:
        value = abi.get("identity")
        if isinstance(value, str) and value:
            return value
    return ""


def _trust_status(payload: dict[str, Any]) -> str:
    trust = payload.get("trust")
    if isinstance(trust, dict):
        for key in ("status", "trust_status", "verification_status"):
            value = trust.get(key)
            if isinstance(value, str) and value:
                return value
        if trust.get("trusted") is True:
            return "trusted"
        if trust.get("trusted") is False:
            return "untrusted"
    for key in ("trust_status", "package_trust_status", "receipt_trust_status"):
        value = payload.get(key)
        if isinstance(value, str) and value:
            return value
    if payload.get("trusted") is True:
        return "trusted"
    if payload.get("trusted") is False:
        return "untrusted"
    return ""


def _artifact_record(kind: str, path_text: str | None) -> dict[str, Any]:
    raw_path = str(path_text or "")
    if not raw_path:
        return {
            "kind": kind,
            "path": "",
            "available": False,
            "size_bytes": 0,
            "sha256": "",
            "explanation": ARTIFACT_EXPLANATIONS[kind],
            "retired_route_reason": f"compile summary did not publish a {kind} artifact path",
        }

    resolved = resolve_repo_path(raw_path)
    available = resolved.is_file()
    return {
        "kind": kind,
        "path": display_path(resolved) if available else raw_path.replace("\\", "/"),
        "available": available,
        "size_bytes": resolved.stat().st_size if available else 0,
        "sha256": _stable_digest_for_file(resolved) if available else "",
        "explanation": ARTIFACT_EXPLANATIONS[kind],
        "retired_route_reason": ""
        if available
        else f"published {kind} artifact path is not present on disk",
    }


def _read_text(record: dict[str, Any]) -> str:
    if record.get("available") is not True:
        return ""
    return resolve_repo_path(str(record["path"])).read_text(encoding="utf-8", errors="replace")


def _has_instruction_debug_location(line: str) -> bool:
    stripped = line.strip()
    if not stripped or stripped.startswith(";"):
        return False
    stripped = stripped.split(";", 1)[0].rstrip()
    lowered = stripped.lower()
    if lowered.startswith(LLVM_NON_INSTRUCTION_DEBUG_LOCATION_PREFIXES):
        return False
    if "@llvm.dbg." in lowered:
        return False
    return LLVM_DEBUG_LOCATION_ATTACHMENT_PATTERN.search(stripped) is not None


def _severity_counts(entries: list[Any]) -> dict[str, int]:
    counts: dict[str, int] = {}
    for entry in entries:
        if not isinstance(entry, dict):
            continue
        severity = str(entry.get("severity", "unknown") or "unknown")
        counts[severity] = counts.get(severity, 0) + 1
    return dict(sorted(counts.items()))


def _diagnostic_codes(entries: list[Any]) -> list[str]:
    codes = {
        str(entry.get("code", ""))
        for entry in entries
        if isinstance(entry, dict) and entry.get("code")
    }
    return sorted(codes)


def _diagnostics_payload(
    record: dict[str, Any],
    diagnostics: dict[str, Any],
) -> dict[str, Any]:
    entries = diagnostics.get("diagnostics", [])
    entries = entries if isinstance(entries, list) else []
    return {
        "available": record["available"],
        "path": record["path"],
        "diagnostic_count": len(entries),
        "severity_counts": _severity_counts(entries),
        "diagnostic_codes": _diagnostic_codes(entries),
        "retired_route_reason": record["retired_route_reason"],
    }


def _symbol_kind_counts(symbols: list[dict[str, Any]]) -> dict[str, int]:
    counts: dict[str, int] = {}
    for symbol in symbols:
        kind = str(symbol.get("kind", "unknown") or "unknown")
        counts[kind] = counts.get(kind, 0) + 1
    return dict(sorted(counts.items()))


def _find_source_graph_fields(manifest: dict[str, Any]) -> dict[str, Any]:
    found: dict[str, Any] = {}
    for key in SOURCE_GRAPH_KEYS:
        if key in manifest:
            found[key] = manifest[key]
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {}) if isinstance(frontend, dict) else {}
    if isinstance(pipeline, dict):
        for key in SOURCE_GRAPH_KEYS:
            if key in pipeline:
                found[f"frontend.pipeline.{key}"] = pipeline[key]
    return found


def _manifest_payload(
    record: dict[str, Any],
    manifest: dict[str, Any],
    symbols: list[dict[str, Any]],
) -> dict[str, Any]:
    source_graph_fields = _find_source_graph_fields(manifest)
    module_name = str(manifest.get("module", "") or "")
    return {
        "available": record["available"],
        "path": record["path"],
        "module": module_name,
        "declaration_count": len(symbols),
        "symbol_kind_counts": _symbol_kind_counts(symbols),
        "source_graph_field_count": len(source_graph_fields),
        "source_graph_fields": sorted(source_graph_fields),
        "retired_route_reason": record["retired_route_reason"],
    }


def _ir_payload(record: dict[str, Any]) -> dict[str, Any]:
    text = _read_text(record)
    lines = text.splitlines()
    llvm_debug_location_count = sum(
        1 for line in lines if _has_instruction_debug_location(line)
    )
    llvm_debug_metadata_present = llvm_debug_location_count > 0
    return {
        "available": record["available"],
        "path": record["path"],
        "line_count": len(lines),
        "function_definition_count": sum(1 for line in lines if re.match(r"^\s*define\b", line)),
        "external_declaration_count": sum(1 for line in lines if re.match(r"^\s*declare\b", line)),
        "global_record_count": sum(1 for line in lines if re.match(r"^\s*@", line)),
        "llvm_debug_metadata_present": llvm_debug_metadata_present,
        "llvm_debug_location_count": llvm_debug_location_count,
        "debug_metadata_model": "llvm-di-metadata-present"
        if llvm_debug_metadata_present
        else "no-llvm-di-debug-locations",
        "retired_route_reason": record["retired_route_reason"],
    }


def _section_name(section: Any) -> str:
    if isinstance(section, dict):
        return str(section.get("name", "") or "")
    return str(section or "")


def _is_native_debug_section(section_name: str) -> bool:
    lowered = section_name.lower()
    return lowered.startswith(NATIVE_DEBUG_SECTION_PREFIXES)


def _is_native_line_table_section(section_name: str) -> bool:
    lowered = section_name.lower()
    return lowered in NATIVE_LINE_TABLE_SECTION_NAMES or lowered.startswith(".debug_line")


def _native_debug_info_evidence_payload(
    object_payload: dict[str, Any],
    ir_payload: dict[str, Any],
) -> dict[str, Any]:
    section_names = sorted(
        {
            name
            for section in _as_list(object_payload.get("sections"))
            for name in [_section_name(section)]
            if name
        }
    )
    native_debug_sections = [
        name for name in section_names if _is_native_debug_section(name)
    ]
    native_line_table_sections = [
        name for name in section_names if _is_native_line_table_section(name)
    ]
    llvm_debug_location_count = int(ir_payload.get("llvm_debug_location_count", 0) or 0)
    llvm_debug_metadata_present = (
        ir_payload.get("llvm_debug_metadata_present") is True
        and llvm_debug_location_count > 0
    )
    emitted_native_debug_info_supported = bool(
        native_debug_sections and llvm_debug_metadata_present
    )
    native_line_table_supported = bool(
        native_line_table_sections and llvm_debug_metadata_present
    )
    blocked_by: list[str] = []
    if object_payload.get("available") is not True:
        blocked_by.append("native-object-artifact-missing")
    if not native_debug_sections:
        blocked_by.append("native-object-lacks-debug-info-section")
    if not native_line_table_sections:
        blocked_by.append("native-object-lacks-debug-line-section")
    if not llvm_debug_metadata_present:
        blocked_by.append("compiler-ir-lacks-llvm-di-locations")
    statement_stepping_supported = (
        emitted_native_debug_info_supported and native_line_table_supported
    )
    if not statement_stepping_supported:
        blocked_by.append("runtime-debug-trace-statement-stepping-integration")
    fail_closed_reason = (
        ""
        if statement_stepping_supported
        else "native object lacks debug info and debug line-table sections"
        if not native_debug_sections and not native_line_table_sections
        else "native object lacks debug line-table sections"
        if not native_line_table_sections
        else "compiler IR lacks instruction-level LLVM DI locations"
        if not llvm_debug_metadata_present
        else "runtime debug trace is not integrated with emitted native debug info"
    )
    evidence_id = "object-model.native-debug-info.production-object-section-probe"
    return {
        "contract_id": OBJECT_MODEL_NATIVE_DEBUG_INFO_EVIDENCE_CONTRACT_ID,
        "evidence_id": evidence_id,
        "source_model": "emitted-object-section-inventory-and-ir-debug-metadata-probe",
        "object_artifact_present": object_payload.get("available") is True,
        "object_path": str(object_payload.get("path", "") or ""),
        "object_format": str(object_payload.get("object_format", "") or ""),
        "object_sha256": str(object_payload.get("sha256", "") or ""),
        "object_section_inventory_command": str(
            object_payload.get("object_section_inventory_command", "") or ""
        ),
        "object_section_names": section_names,
        "native_debug_sections": native_debug_sections,
        "native_line_table_sections": native_line_table_sections,
        "native_debug_section_count": len(native_debug_sections),
        "native_line_table_section_count": len(native_line_table_sections),
        "ir_path": str(ir_payload.get("path", "") or ""),
        "ir_debug_metadata_model": str(ir_payload.get("debug_metadata_model", "") or ""),
        "llvm_debug_metadata_present": llvm_debug_metadata_present,
        "llvm_debug_location_count": llvm_debug_location_count,
        "emitted_native_debug_info_supported": emitted_native_debug_info_supported,
        "native_line_table_supported": native_line_table_supported,
        "statement_stepping_supported": statement_stepping_supported,
        "statement_stepping_evidence_id": (
            "object-model.statement-stepping.production-source-line-table"
            if statement_stepping_supported
            else ""
        ),
        "fail_closed": not statement_stepping_supported,
        "fail_closed_reason": fail_closed_reason,
        "blocked_by": blocked_by,
    }


def _source_graph_payload(
    manifest_payload: dict[str, Any],
    symbols: list[dict[str, Any]],
    workspace_index: dict[str, Any],
    source_index: dict[str, Any] | None = None,
    source_graph: dict[str, Any] | None = None,
) -> dict[str, Any]:
    if isinstance(source_graph, dict) and source_graph.get("contract_id") == "objc3c.developer.tooling.source.graph.v1":
        return {
            "available": source_graph.get("available") is True,
            "graph_inputs": list(source_graph.get("evidence", {}).get("source_truth_inputs", []))
            if isinstance(source_graph.get("evidence"), dict)
            else [],
            "declaration_node_count": int(source_graph.get("declaration_node_count", 0) or 0),
            "workspace_package_count": int(source_graph.get("package_provenance", {}).get("package_count", 0) or 0)
            if isinstance(source_graph.get("package_provenance"), dict)
            else int(workspace_index.get("package_count", 0) or 0),
            "workspace_index_digest": str(workspace_index.get("workspace_index_digest", "") or ""),
            "source_declaration_count": int(source_index.get("declaration_count", 0) or 0)
            if isinstance(source_index, dict)
            else 0,
            "source_reference_count": int(source_index.get("reference_count", 0) or 0)
            if isinstance(source_index, dict)
            else 0,
            "source_index_digest": str(source_index.get("source_index_digest", "") or "")
            if isinstance(source_index, dict)
            else "",
            "source_graph_digest": str(source_graph.get("source_graph_digest", "") or ""),
            "retired_route_reason": str(source_graph.get("retired_route_reason", "") or ""),
        }
    workspace_available = workspace_index.get("available") is True
    source_index = source_index or {}
    source_index_available = source_index.get("available") is True
    available = manifest_payload["available"] and (
        bool(symbols)
        or int(manifest_payload.get("source_graph_field_count", 0) or 0) > 0
        or workspace_available
        or source_index_available
    )
    graph_inputs = ["manifest-declarations"] if symbols else []
    if int(manifest_payload.get("source_graph_field_count", 0) or 0) > 0:
        graph_inputs.append("manifest-source-graph-fields")
    if workspace_available:
        graph_inputs.append("workspace-index-packages")
    if source_index_available:
        graph_inputs.append("source-derived-editor-index")
    digest_input = {
        "symbols": [
            {
                "name": symbol.get("name"),
                "kind": symbol.get("kind"),
                "line": symbol.get("line"),
                "column": symbol.get("column"),
            }
            for symbol in symbols
        ],
        "workspace_index_digest": workspace_index.get("workspace_index_digest", ""),
        "source_index_digest": source_index.get("source_index_digest", ""),
        "source_graph_fields": manifest_payload.get("source_graph_fields", []),
    }
    digest = (
        hashlib.sha256(
            json.dumps(
                _stable_json_value(digest_input),
                sort_keys=True,
                separators=(",", ":"),
            ).encode("utf-8")
        ).hexdigest()
        if available
        else ""
    )
    return {
        "available": available,
        "graph_inputs": graph_inputs,
        "declaration_node_count": len(symbols),
        "workspace_package_count": int(workspace_index.get("package_count", 0) or 0),
        "workspace_index_digest": str(workspace_index.get("workspace_index_digest", "") or ""),
        "source_declaration_count": int(source_index.get("declaration_count", 0) or 0),
        "source_reference_count": int(source_index.get("reference_count", 0) or 0),
        "source_index_digest": str(source_index.get("source_index_digest", "") or ""),
        "source_graph_digest": digest,
        "retired_route_reason": ""
        if available
        else "manifest did not publish declarations, source graph fields, or workspace index inputs",
    }


def _object_payload(
    record: dict[str, Any],
    runtime_commands: dict[str, Any],
    summary: dict[str, Any] | None = None,
) -> dict[str, Any]:
    summary = summary or {}
    object_symbols = str(runtime_commands.get("object_symbols", "") or "")
    object_sections = str(runtime_commands.get("object_sections", "") or "")
    object_format = _detect_object_format(resolve_repo_path(str(record["path"]))) if record["available"] else ""
    inspected_inventory = _inspect_object_inventory(record)
    summary_inventory = _summary_object_inventory(summary)
    tool_symbols = _stable_inventory_list(inspected_inventory.get("symbols"))
    tool_sections = _stable_inventory_list(inspected_inventory.get("sections"))
    tool_inventory_available = bool(tool_symbols and tool_sections)
    inventory = {
        **summary_inventory,
        **inspected_inventory,
    }
    symbols = _stable_inventory_list(inventory.get("symbols"))
    sections = _stable_inventory_list(inventory.get("sections"))
    inventory_commands = _as_dict(inventory.get("commands"))
    exported_runtime_helpers = [
        symbol
        for symbol in symbols
        if _is_runtime_symbol(symbol) and _is_exported_symbol(symbol)
    ]
    imported_runtime_helpers = [
        symbol
        for symbol in symbols
        if _is_runtime_symbol(symbol) and _is_imported_symbol(symbol)
    ]
    expected_digest = _digest_expectation(summary, record) or str(record["sha256"])
    digest_matches = not expected_digest or expected_digest == str(record["sha256"])
    inventory_available = bool(tool_inventory_available and expected_digest)
    unsupported_format = bool(
        record["available"] and object_format not in OBJECT_FORMATS_WITH_SYMBOL_TABLES
    )
    inventory_source_model = (
        "emitted-object-tool-symbol-and-section-inventory"
        if tool_inventory_available
        else "fail-closed-no-tool-derived-object-inventory"
        if record["available"]
        else "unavailable-object-artifact"
    )
    inventory_digest = _stable_inventory_digest(
        {
            "object_format": object_format,
            "sha256": record["sha256"],
            "symbols": symbols,
            "sections": sections,
            "exported_runtime_helpers": exported_runtime_helpers,
            "imported_runtime_helpers": imported_runtime_helpers,
        }
    ) if inventory_available else ""
    return {
        "available": record["available"],
        "path": record["path"],
        "size_bytes": record["size_bytes"],
        "sha256": record["sha256"],
        "object_format": object_format,
        "expected_sha256": expected_digest,
        "digest_matches": digest_matches,
        "inventory_source_model": inventory_source_model,
        "tool_inventory_available": tool_inventory_available,
        "inventory_available": inventory_available,
        "inventory_digest": inventory_digest,
        "inventory_diagnostics": sorted(
            str(item) for item in _as_list(inventory.get("diagnostics")) if item
        ),
        "symbol_count": len(symbols),
        "symbols": symbols,
        "section_count": len(sections),
        "sections": sections,
        "exported_runtime_helper_count": len(exported_runtime_helpers),
        "exported_runtime_helpers": exported_runtime_helpers,
        "imported_runtime_helper_count": len(imported_runtime_helpers),
        "imported_runtime_helpers": imported_runtime_helpers,
        "object_symbol_inventory_command": str(
            inventory_commands.get("object_symbols", "") or object_symbols
        ),
        "object_section_inventory_command": str(
            inventory_commands.get("object_sections", "") or object_sections
        ),
        "inspection_ready": bool(
            record["available"]
            and digest_matches
            and not unsupported_format
            and inventory_available
            and (inventory_commands.get("object_symbols") or object_symbols)
            and (inventory_commands.get("object_sections") or object_sections)
        ),
        "retired_route_reason": (
            record["retired_route_reason"]
            if not record["available"]
            else "object format is unsupported"
            if unsupported_format
            else "object digest does not match published inventory expectation"
            if not digest_matches
            else "runtime inspector did not publish bound object symbol, section, and digest inventory"
            if not inventory_available
            else ""
        ),
    }


def _runtime_imports_payload(
    record: dict[str, Any],
    summary: dict[str, Any],
) -> dict[str, Any]:
    runtime_inspector = _runtime_inspector(summary)
    runtime_commands = _runtime_dump_commands(summary)
    runtime_imports = summary.get("runtime_imports", [])
    runtime_imports = runtime_imports if isinstance(runtime_imports, list) else []
    available = record["available"] or bool(runtime_imports) or runtime_inspector.get("available") is True
    return {
        "available": available,
        "runtime_metadata_binary_path": record["path"],
        "runtime_metadata_binary_present": record["available"],
        "runtime_import_count": len(runtime_imports),
        "runtime_imports": runtime_imports,
        "runtime_inspector_available": runtime_inspector.get("available") is True,
        "runtime_inspector_contract_id": str(runtime_inspector.get("contract_id", "") or ""),
        "runtime_dump_commands": {
            key: str(value)
            for key, value in sorted(runtime_commands.items())
            if value
        },
        "retired_route_reason": ""
        if available
        else "compile emitted no runtime metadata import artifact or runtime inspector surface",
    }


def _runtime_inventory_payload(
    record: dict[str, Any],
    summary: dict[str, Any],
    manifest: dict[str, Any],
) -> dict[str, Any]:
    inventory = {
        **_runtime_metadata_file_inventory(record),
        **_manifest_runtime_inventory(manifest),
        **_summary_runtime_inventory(summary),
    }
    runtime_imports = summary.get("runtime_imports", [])
    runtime_imports = runtime_imports if isinstance(runtime_imports, list) else []
    class_records = _runtime_inventory_entries(inventory, "class_records", "classes")
    selector_records = _runtime_inventory_entries(inventory, "selector_records", "selectors")
    method_records = _runtime_inventory_entries(inventory, "method_records", "methods")
    property_records = _runtime_inventory_entries(inventory, "property_records", "properties")
    protocol_records = _runtime_inventory_entries(inventory, "protocol_records", "protocols")
    category_records = _runtime_inventory_entries(inventory, "category_records", "categories")
    helper_references = _runtime_inventory_entries(
        inventory,
        "stdlib_helper_references",
        "runtime_helper_references",
    )
    import_package_records = (
        _runtime_inventory_entries(
            inventory,
            "runtime_import_package_records",
            "runtime_imports",
        )
        or _stable_inventory_list(runtime_imports)
    )
    runtime_artifacts = _runtime_inventory_entries(
        inventory,
        "runtime_artifacts",
        "artifacts",
        "artifact_records",
    )
    available = bool(
        inventory
        or class_records
        or selector_records
        or method_records
        or property_records
        or protocol_records
        or category_records
        or runtime_artifacts
    )
    inventory_digest = _stable_inventory_digest(
        {
            "reflection_abi_version": str(inventory.get("reflection_abi_version", "") or ""),
            "class_records": class_records,
            "selector_records": selector_records,
            "method_records": method_records,
            "property_records": property_records,
            "protocol_records": protocol_records,
            "category_records": category_records,
            "stdlib_helper_references": helper_references,
            "runtime_import_package_records": import_package_records,
            "runtime_artifacts": runtime_artifacts,
        }
    ) if available else ""

    return {
        "available": available,
        "reflection_abi_version": str(inventory.get("reflection_abi_version", "") or ""),
        "runtime_metadata_link": record["path"],
        "inventory_digest": inventory_digest,
        "class_record_count": len(class_records),
        "class_records": class_records,
        "selector_record_count": len(selector_records),
        "selector_records": selector_records,
        "method_record_count": len(method_records),
        "method_records": method_records,
        "property_record_count": len(property_records),
        "property_records": property_records,
        "protocol_record_count": len(protocol_records),
        "protocol_records": protocol_records,
        "category_record_count": len(category_records),
        "category_records": category_records,
        "stdlib_helper_references": helper_references,
        "runtime_import_package_records": import_package_records,
        "runtime_artifact_count": len(runtime_artifacts),
        "runtime_artifacts": runtime_artifacts,
        "retired_route_reason": ""
        if available
        else "runtime inspector did not publish runtime object metadata inventory",
    }


def _source_graph_debug_links_payload(
    records: dict[str, dict[str, Any]],
    paths: EditorToolingPaths,
    inputs: EditorToolingInputs,
    source_graph: dict[str, Any] | None,
) -> dict[str, Any]:
    summary_paths = _summary_paths(inputs.summary)
    published_debug_map_path = (
        str(summary_paths.get("debug_map", "") or inputs.summary.get("debug_map_path", "") or "")
    )
    debug_map_path = str(
        published_debug_map_path
        or (display_path(paths.debug_map) if paths.debug_map.is_file() else "")
    )
    optimization_trace_path = str(
        summary_paths.get("optimization_trace", "")
        or inputs.summary.get("optimization_trace_path", "")
        or ""
    )
    source_graph_digest = (
        str(source_graph.get("source_graph_digest", "") or "")
        if isinstance(source_graph, dict)
        else ""
    )
    source_graph_link = (
        display_path(paths.source_graph)
        if paths.source_graph.is_file()
        or (isinstance(source_graph, dict) and source_graph.get("available") is True)
        else ""
    )
    debug_map_link = debug_map_path or (
        display_path(paths.debug_map)
        if records["object"].get("available") is True or paths.debug_map.is_file()
        else ""
    )
    return {
        "manifest_link": records["manifest"]["path"],
        "ir_link": records["ir"]["path"],
        "diagnostics_link": records["diagnostics"]["path"],
        "runtime_metadata_link": records["runtime_metadata_binary"]["path"],
        "source_graph_link": source_graph_link,
        "source_graph_digest": source_graph_digest,
        "debug_map_link": debug_map_link,
        "optimization_trace_link": optimization_trace_path,
    }


def _package_receipt_records(summary: dict[str, Any]) -> list[dict[str, Any]]:
    receipt_inputs = _stable_inventory_list(summary.get("package_operation_receipts"))
    if not receipt_inputs:
        receipt_inputs = _stable_inventory_list(summary.get("package_receipts"))
    records: list[dict[str, Any]] = []
    for entry in receipt_inputs:
        if isinstance(entry, str):
            entry_payload: dict[str, Any] = {"path": entry}
        elif isinstance(entry, dict):
            entry_payload = dict(entry)
        else:
            continue
        path_text = str(entry_payload.get("path", "") or "")
        receipt_payload = _load_json_object_if_present(path_text)
        digest = _stable_digest_for_file(resolve_repo_path(path_text)) if path_text and resolve_repo_path(path_text).is_file() else ""
        expected_digest = str(
            entry_payload.get("sha256", "")
            or entry_payload.get("digest", "")
            or receipt_payload.get("sha256", "")
            or receipt_payload.get("digest", "")
            or ""
        ).removeprefix("sha256:")
        package_id = (
            _identity_from_payload(entry_payload, ("package_id", "package_identity"))
            or _identity_from_payload(receipt_payload, ("package_id", "package_identity"))
        )
        trust_status = _trust_status(entry_payload) or _trust_status(receipt_payload)
        available = bool(path_text and receipt_payload)
        digest_matches = not expected_digest or expected_digest == digest
        trusted = trust_status in ("", "trusted", "verified", "not-revoked")
        records.append(
            {
                "path": path_text.replace("\\", "/"),
                "available": available,
                "operation": str(
                    entry_payload.get("operation", "")
                    or receipt_payload.get("operation", "")
                    or receipt_payload.get("receipt_kind", "")
                    or ""
                ),
                "package_id": package_id,
                "sha256": digest,
                "expected_sha256": expected_digest,
                "digest_matches": digest_matches,
                "trust_status": trust_status,
                "trusted": trusted,
                "retired_route_reason": ""
                if available and digest_matches and trusted
                else "package operation receipt is missing"
                if not available
                else "package operation receipt digest does not match published expectation"
                if not digest_matches
                else "package operation receipt trust status is not trusted",
            }
        )
    return records


def _package_inventory_payload(
    manifest: dict[str, Any],
    summary: dict[str, Any],
) -> dict[str, Any]:
    package_summary = _as_dict(summary.get("package"))
    registry = _as_dict(summary.get("registry"))
    manifest_package_id = _identity_from_payload(
        manifest,
        ("package_id", "package_identity", "module_package_id"),
    )
    summary_package_id = _identity_from_payload(
        package_summary,
        ("package_id", "package_identity", "module_package_id"),
    )
    registry_package_id = _identity_from_payload(
        registry,
        ("package_id", "package_identity", "module_package_id"),
    )
    manifest_abi_identity = _identity_from_payload(manifest, ("abi_identity",))
    summary_abi_identity = _identity_from_payload(package_summary, ("abi_identity",))
    registry_abi_identity = _identity_from_payload(registry, ("abi_identity",))
    receipt_records = _package_receipt_records(summary)
    receipt_package_ids = sorted({
        str(record.get("package_id", ""))
        for record in receipt_records
        if record.get("package_id")
    })
    source_path = str(manifest.get("source", "") or summary.get("input_path", "") or "")
    source_package_id = f"source:{source_path}" if source_path else ""
    explicit_package_ids = [
        value
        for value in (
            manifest_package_id,
            summary_package_id,
            registry_package_id,
            *receipt_package_ids,
        )
        if value
    ]
    derived_package_id = source_package_id if not explicit_package_ids else ""
    derived_abi_identity = DEFAULT_OBJC3_ABI_IDENTITY if not any(
        (manifest_abi_identity, summary_abi_identity, registry_abi_identity)
    ) else ""
    package_ids = [
        value
        for value in (
            *explicit_package_ids,
            derived_package_id,
        )
        if value
    ]
    abi_identities = [
        value
        for value in (
            manifest_abi_identity,
            summary_abi_identity,
            registry_abi_identity,
            derived_abi_identity,
        )
        if value
    ]
    identity_mismatch = len(set(package_ids)) > 1 or len(set(abi_identities)) > 1
    receipt_untrusted = any(record.get("trusted") is False for record in receipt_records)
    package_identity_source = (
        "manifest"
        if manifest_package_id
        else "summary-package"
        if summary_package_id
        else "registry"
        if registry_package_id
        else "package-receipt"
        if receipt_package_ids
        else "source-path"
        if derived_package_id
        else ""
    )
    abi_identity_source = (
        "manifest"
        if manifest_abi_identity
        else "summary-package"
        if summary_abi_identity
        else "registry"
        if registry_abi_identity
        else "default-objc3-abi-schema"
        if derived_abi_identity
        else ""
    )
    return {
        "available": bool(package_ids or abi_identities or registry or receipt_records),
        "module_identity": str(manifest.get("module", "") or ""),
        "package_identity": package_ids[0] if package_ids else "",
        "abi_identity": abi_identities[0] if abi_identities else "",
        "package_identity_source": package_identity_source,
        "abi_identity_source": abi_identity_source,
        "manifest_package_identity": manifest_package_id,
        "registry_package_identity": registry_package_id,
        "receipt_package_identities": receipt_package_ids,
        "identity_mismatch": identity_mismatch,
        "registry_identity": str(
            registry.get("registry_id", "")
            or registry.get("registry_identity", "")
            or ""
        ),
        "manifest_trust_status": _trust_status(manifest),
        "registry_trust_status": _trust_status(registry),
        "package_operation_receipts": sorted(receipt_records, key=_stable_json_sort_key),
        "package_operation_receipt_count": len(receipt_records),
        "untrusted_receipt_count": sum(
            1 for record in receipt_records if record.get("trusted") is False
        ),
        "retired_route_reason": "package identity mismatch across manifest, registry, or receipts"
        if identity_mismatch
        else "package operation receipt trust status is not trusted"
        if receipt_untrusted
        else "",
    }


def _provenance_payload(summary: dict[str, Any], paths: EditorToolingPaths) -> dict[str, Any]:
    provenance = _as_dict(summary.get("artifact_provenance"))
    generated_artifacts = _stable_inventory_list(summary.get("generated_artifacts"))
    generated = provenance.get("generated") is True or bool(generated_artifacts)
    source_truth_inputs = sorted(str(item) for item in _as_list(provenance.get("source_truth_inputs")))
    if not generated and not source_truth_inputs and paths.source.display_path:
        source_truth_inputs = [paths.source.display_path]
    available = not generated or bool(source_truth_inputs)
    return {
        "available": available,
        "generated": generated,
        "source_truth_inputs": source_truth_inputs,
        "generated_artifacts": generated_artifacts,
        "retired_route_reason": ""
        if available
        else "generated artifact inventory did not publish provenance source truth inputs",
    }


def _inventory_validation_payload(
    object_payload: dict[str, Any],
    runtime_inventory: dict[str, Any],
    package_inventory: dict[str, Any],
    provenance: dict[str, Any],
) -> dict[str, Any]:
    reasons: list[str] = []
    unsupported_notes: list[str] = []
    if object_payload.get("available") is not True:
        reasons.append("missing object artifact")
        unsupported_notes.append("object path is absent from the compile summary")
    if object_payload.get("object_format") == "unsupported":
        reasons.append("unsupported object format")
        unsupported_notes.append("object bytes do not use a supported symbol-table format")
    if object_payload.get("digest_matches") is False:
        reasons.append("stale object digest")
        unsupported_notes.append("object bytes differ from the published inventory digest")
    if (
        object_payload.get("available") is True
        and object_payload.get("object_format") != "unsupported"
        and object_payload.get("inventory_available") is not True
    ):
        reasons.append("missing object symbol inventory")
        unsupported_notes.append("object inventory lacks bound symbol and section rows")
    if runtime_inventory.get("available") is not True:
        reasons.append("missing runtime inventory")
        unsupported_notes.append("runtime metadata inventory is absent")
    if package_inventory.get("identity_mismatch") is True:
        reasons.append("package identity mismatch")
        unsupported_notes.append("manifest, registry, and receipt package identities diverge")
    if int(package_inventory.get("untrusted_receipt_count", 0) or 0) > 0:
        reasons.append("untrusted package receipt")
        unsupported_notes.append("one or more package receipts are not trusted")
    if not package_inventory.get("package_identity"):
        reasons.append("missing package identity")
        unsupported_notes.append("object inventory lacks module package identity")
    if not package_inventory.get("abi_identity"):
        reasons.append("missing ABI identity")
        unsupported_notes.append("object inventory lacks ABI identity")
    if provenance.get("available") is not True:
        reasons.append("generated artifact missing provenance")
        unsupported_notes.append("generated artifacts do not cite source-truth inputs")
    ready = not reasons
    return {
        "inventory_ready": ready,
        "fail_closed": not ready,
        "fail_closed_reasons": sorted(reasons),
        "unsupported_inventory_notes": sorted(set(unsupported_notes)),
    }


def _inspection_commands(
    records: dict[str, dict[str, Any]],
    summary: dict[str, Any],
    object_payload: dict[str, Any],
) -> dict[str, str]:
    commands: dict[str, str] = {}
    summary_commands = _dump_commands(summary)
    runtime_commands = _runtime_dump_commands(summary)
    for kind, record in records.items():
        if record.get("available") is not True:
            continue
        if kind in summary_commands and summary_commands[kind]:
            commands[kind] = str(summary_commands[kind])
        elif kind in TEXT_ARTIFACT_KINDS:
            commands[kind] = f"Get-Content -Raw '{record['path']}'"
    if records["object"].get("available") is True:
        for command_name in ("object_symbols", "object_sections"):
            command = (
                object_payload.get("object_symbol_inventory_command")
                if command_name == "object_symbols"
                else object_payload.get("object_section_inventory_command")
            )
            if not command:
                command = runtime_commands.get(command_name)
            if command:
                commands[command_name] = str(command)
    return dict(sorted(commands.items()))


def build_artifact_inspector_payload(
    paths: EditorToolingPaths,
    inputs: EditorToolingInputs,
    symbols: list[dict[str, Any]],
    workspace_index: dict[str, Any],
    source_index: dict[str, Any] | None = None,
    source_graph: dict[str, Any] | None = None,
) -> dict[str, Any]:
    summary_paths = _summary_paths(inputs.summary)
    records = {
        "summary": _artifact_record("summary", display_path(paths.compile_summary)),
        "diagnostics": _artifact_record("diagnostics", inputs.diagnostics_path_text),
        "manifest": _artifact_record("manifest", inputs.manifest_path_text),
        "ir": _artifact_record("ir", str(summary_paths.get("ir", "") or "")),
        "object": _artifact_record("object", inputs.object_path_text),
        "runtime_metadata_binary": _artifact_record(
            "runtime_metadata_binary",
            str(summary_paths.get("runtime_metadata_binary", "") or ""),
        ),
    }
    available_kinds = [kind for kind, record in records.items() if record["available"]]
    unsupported_kinds = [kind for kind, record in records.items() if not record["available"]]
    manifest = _manifest_payload(records["manifest"], inputs.manifest, symbols)
    runtime_commands = _runtime_dump_commands(inputs.summary)
    object_payload = _object_payload(records["object"], runtime_commands, inputs.summary)
    ir_payload = _ir_payload(records["ir"])
    native_debug_info_evidence = _native_debug_info_evidence_payload(
        object_payload,
        ir_payload,
    )
    runtime_inventory = _runtime_inventory_payload(
        records["runtime_metadata_binary"],
        inputs.summary,
        inputs.manifest,
    )
    package_inventory = _package_inventory_payload(inputs.manifest, inputs.summary)
    provenance = _provenance_payload(inputs.summary, paths)
    inventory_validation = _inventory_validation_payload(
        object_payload,
        runtime_inventory,
        package_inventory,
        provenance,
    )
    supported = bool(available_kinds)
    return {
        "contract_id": "objc3c.developer.tooling.artifact.inspector.v1",
        "source_path": paths.source.display_path,
        "supported": supported,
        "support_class": "compile-artifact-inspector"
        if supported and not inventory_validation["fail_closed"]
        else "compile-artifact-inspector-with-fail-closed-inventory"
        if supported
        else "fail-closed",
        "artifact_records": [records[kind] for kind in sorted(records)],
        "inspected_artifact_kinds": sorted(available_kinds),
        "unsupported_artifact_kinds": sorted(unsupported_kinds),
        "diagnostics": _diagnostics_payload(records["diagnostics"], inputs.diagnostics),
        "manifest": manifest,
        "ir": ir_payload,
        "object": object_payload,
        "native_debug_info_evidence": native_debug_info_evidence,
        "runtime_imports": _runtime_imports_payload(
            records["runtime_metadata_binary"],
            inputs.summary,
        ),
        "runtime_inventory": runtime_inventory,
        "package_inventory": package_inventory,
        "source_graph": _source_graph_payload(
            manifest,
            symbols,
            workspace_index,
            source_index,
            source_graph,
        ),
        "artifact_links": _source_graph_debug_links_payload(
            records,
            paths,
            inputs,
            source_graph,
        ),
        "provenance": provenance,
        "inventory_validation": inventory_validation,
        "source_index": source_index or {},
        "inspection_commands": _inspection_commands(records, inputs.summary, object_payload),
        "retired_route_reason": ""
        if supported
        else "compile produced no inspectable editor-facing artifacts",
    }
