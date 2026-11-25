from __future__ import annotations

import hashlib
import json
from pathlib import Path
from typing import Any

from objc3c_toolchain.source_graph import load_graph
from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import ROOT


STDLIB_MODULE_INVENTORY = ROOT / "stdlib" / "module_inventory.json"
SHOWCASE_PORTFOLIO = ROOT / "showcase" / "portfolio.json"


def stable_digest(payload: object) -> str:
    encoded = json.dumps(payload, sort_keys=True, separators=(",", ":")).encode(
        "utf-8"
    )
    return hashlib.sha256(encoded).hexdigest()


def source_location(
    source_path: str,
    *,
    line: int,
    column: int,
    end_line: int,
    end_column: int,
) -> dict[str, Any]:
    lsp_start_line = max(line - 1, 0)
    lsp_start_column = max(column - 1, 0)
    lsp_end_line = max(end_line - 1, lsp_start_line)
    lsp_end_column = max(end_column - 1, lsp_start_column)
    return {
        "uri": source_path,
        "source_path": source_path,
        "compiler_range": {
            "line": line,
            "column": column,
            "end_line": end_line,
            "end_column": end_column,
        },
        "range": {
            "start": {"line": lsp_start_line, "character": lsp_start_column},
            "end": {"line": lsp_end_line, "character": lsp_end_column},
        },
        "range_model": "compiler-1-based-plus-lsp-zero-based",
    }


def symbol_location(source_path: str, symbol: dict[str, Any]) -> dict[str, Any]:
    line = int(symbol.get("line", 0) or 0)
    column = int(symbol.get("column", 0) or 0)
    end_line = int(symbol.get("end_line", line) or line)
    end_column = int(symbol.get("end_column", column) or column)
    return source_location(
        source_path,
        line=line,
        column=column,
        end_line=end_line,
        end_column=end_column,
    )


def document_symbol_records(
    source_path: str,
    module_name: str,
    symbols: list[dict[str, Any]],
) -> list[dict[str, Any]]:
    records: list[dict[str, Any]] = []
    for symbol in symbols:
        location = symbol_location(source_path, symbol)
        records.append(
            {
                "name": symbol["name"],
                "kind": symbol["kind"],
                "container_name": module_name,
                "source_path": source_path,
                "location": location,
                "definition": {
                    "target_uri": source_path,
                    "target_range": location["range"],
                    "target_compiler_range": location["compiler_range"],
                },
                "origin": "compile-manifest",
            }
        )
    return sorted(
        records,
        key=lambda record: (
            str(record["source_path"]),
            int(record["location"]["compiler_range"]["line"]),
            int(record["location"]["compiler_range"]["column"]),
            str(record["kind"]),
            str(record["name"]),
        ),
    )


def package_location(source_path: str, display_name: str) -> dict[str, Any]:
    return source_location(
        source_path,
        line=1,
        column=1,
        end_line=1,
        end_column=max(len(display_name), 1) + 1,
    )


def local_source_package(
    source_path: str,
    module_name: str,
    manifest_path: str | None,
    symbols: list[dict[str, Any]],
) -> dict[str, Any]:
    return {
        "package_id": f"source:{source_path}",
        "package_kind": "primary-source",
        "module_name": module_name,
        "source_path": source_path,
        "workspace_root": str(Path(source_path).parent).replace("\\", "/"),
        "manifest_path": manifest_path,
        "source_authority": "live-frontend-compile",
        "symbol_count": len(symbols),
        "definition": package_location(source_path, module_name),
    }


def stdlib_package_entries() -> list[dict[str, Any]]:
    inventory = load_json(STDLIB_MODULE_INVENTORY)
    entries: list[dict[str, Any]] = []
    for module in inventory.get("canonical_modules", []):
        if not isinstance(module, dict):
            continue
        module_name = str(module.get("module", ""))
        source_path = str(module.get("header", ""))
        if not module_name or not source_path:
            continue
        entries.append(
            {
                "package_id": f"stdlib:{module_name}",
                "package_kind": "stdlib-module",
                "module_name": module_name,
                "implementation_module": str(module.get("implementation_module", "")),
                "capability_id": str(module.get("capability_id", "")),
                "required_profile": str(module.get("required_profile", "")),
                "source_path": source_path,
                "workspace_root": Path(source_path).parent.as_posix(),
                "manifest_path": STDLIB_MODULE_INVENTORY.relative_to(ROOT).as_posix(),
                "source_authority": "stdlib/module_inventory.json",
                "definition": package_location(source_path, module_name),
            }
        )
    return entries


def showcase_workspace_entries() -> tuple[list[dict], list[dict], dict]:
    portfolio = load_json(SHOWCASE_PORTFOLIO)
    entries, edges, errors = [], [], []
    manifests = {}
    for example in portfolio["examples"]:
        example_id = example["id"]
        source_path = example["source"]
        package_id = f"showcase:{example_id}"
        manifest_path = example.get("workspace_manifest")
        module_name = example.get("title", example_id)
        entries.append({
            "package_id": package_id, "package_kind": "showcase-example",
            "module_name": module_name, "source_path": source_path,
            "workspace_manifest": manifest_path or "",
            "source_authority": SHOWCASE_PORTFOLIO.relative_to(ROOT).as_posix(),
            "definition": package_location(source_path, module_name) if (ROOT / source_path).is_file() else None,
        })
        if manifest_path is None:
            continue
        try:
            path = (ROOT / manifest_path).resolve(strict=True)
            if not path.is_relative_to(ROOT):
                raise ValueError("workspace manifest escaped the repository")
            graph = load_graph(path)
        except (OSError, ValueError) as error:
            errors.append({"manifest": manifest_path, "diagnostic": str(error)})
            continue
        manifests.update({path.relative_to(ROOT).as_posix(): digest
                          for path, digest in graph["manifests"].items()})
        application_source = graph["application"]["input"].relative_to(ROOT).as_posix()
        entries[-1].update(source_path=application_source, definition=package_location(application_source, module_name))
        providers = {name: f"workspace:{example_id}:{name}" for name in graph["order"]}
        consumers = [(package_id, graph["application"])]
        for name, provider_id in providers.items():
            provider = graph["packages"][name]
            location = provider["input"].relative_to(ROOT).as_posix()
            entries.append({
                "package_id": provider_id, "package_kind": "workspace-provider",
                "module_name": name, "source_path": location,
                "locked_identity": provider["package"],
                "manifest_path": provider["manifest"].relative_to(ROOT).as_posix(),
                "source_authority": manifest_path,
                "definition": package_location(location, name),
            })
            consumers.append((provider_id, provider))
        for consumer_id, consumer in consumers:
            edges.extend({
                "from_package_id": consumer_id, "to_package_id": providers[edge["name"]],
                "edge_kind": f'{edge["visibility"]}-package-import',
                "locked_identity": edge["package"], "source_authority": manifest_path,
            } for edge in consumer["dependencies"])
    guardrails = {
        "contract_id": "objc3c.developer.tooling.workspace.package.guardrails.v1",
        "source_contracts": sorted(manifests),
        "manifest_sha256": manifests,
        "checks": {"native_workspace_locks_valid": not errors},
        "errors": errors,
        "ok": not errors,
    }
    return entries, sorted(edges, key=lambda edge: (edge["from_package_id"], edge["to_package_id"])), guardrails


def package_symbol_records(packages: list[dict[str, Any]]) -> list[dict[str, Any]]:
    records: list[dict[str, Any]] = []
    for package in packages:
        location = package["definition"]
        if location is None:
            continue
        source_path = str(package["source_path"])
        records.append(
            {
                "name": package["module_name"],
                "kind": package["package_kind"],
                "container_name": package["package_id"],
                "package_id": package["package_id"],
                "source_path": source_path,
                "location": location,
                "definition": {
                    "target_uri": source_path,
                    "target_range": location["range"],
                    "target_compiler_range": location["compiler_range"],
                },
                "origin": package["source_authority"],
                "cross_package": package["package_kind"] != "primary-source",
            }
        )
    return sorted(
        records,
        key=lambda record: (
            str(record["package_id"]),
            str(record["name"]),
            str(record["source_path"]),
        ),
    )


def build_workspace_index(
    source_path: str,
    module_name: str,
    manifest_path: str | None,
    symbols: list[dict[str, Any]],
    source_index: dict[str, Any] | None = None,
) -> dict[str, Any]:
    showcase, edges, guardrails = showcase_workspace_entries()
    packages = [
        local_source_package(source_path, module_name, manifest_path, symbols),
        *stdlib_package_entries(),
        *showcase,
    ]
    packages = sorted(packages, key=lambda package: str(package["package_id"]))
    package_symbols = package_symbol_records(packages)
    digest_inputs = {
        "package_ids": [package["package_id"] for package in packages],
        "source_paths": [package["source_path"] for package in packages],
        "edges": edges,
        "guardrail_checks": guardrails["checks"],
        "workspace_manifests": guardrails["manifest_sha256"],
        "source_index_digest": source_index.get("source_index_digest", "")
        if isinstance(source_index, dict)
        else "",
    }
    evidence_roots = list(dict.fromkeys([
        "stdlib/module_inventory.json",
        "showcase/portfolio.json",
        *guardrails["source_contracts"],
    ]))
    if manifest_path:
        evidence_roots.append(manifest_path)
    available = manifest_path is not None and guardrails["ok"]
    return {
        "contract_id": "objc3c.developer.tooling.workspace.semantic.index.v1",
        "available": available,
        "index_model": "compile-manifest-plus-checked-in-package-workspace-surfaces",
        "source_truth_model": "checked-in-workspace-and-package-surfaces-plus-live-frontend-manifest",
        "evidence_roots": evidence_roots,
        "unsupported_surfaces": [
            "network-backed workspace restore",
            "tmp-only workspace source truth",
            "workspace symbols without manifest-backed source coordinates",
        ],
        "fail_closed": not available,
        "package_count": len(packages),
        "cross_package_edge_count": len(edges),
        "packages": packages,
        "cross_package_edges": edges,
        "package_symbols": package_symbols,
        "source_index": source_index or {},
        "source_declaration_count": int(source_index.get("declaration_count", 0) or 0)
        if isinstance(source_index, dict)
        else 0,
        "source_reference_count": int(source_index.get("reference_count", 0) or 0)
        if isinstance(source_index, dict)
        else 0,
        "source_import_count": int(source_index.get("import_count", 0) or 0)
        if isinstance(source_index, dict)
        else 0,
        "guardrails": guardrails,
        "workspace_index_digest": stable_digest(digest_inputs),
        "deterministic_ordering": "package-id-then-source-path",
        "retired_route_reason": ""
        if available
        else (
            "compile produced no manifest-backed declaration surface"
            if manifest_path is None
            else "workspace package guardrails failed closed"
        ),
    }
