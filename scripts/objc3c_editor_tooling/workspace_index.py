from __future__ import annotations

import hashlib
import json
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import ROOT


STDLIB_MODULE_INVENTORY = ROOT / "stdlib" / "module_inventory.json"
SHOWCASE_PORTFOLIO = ROOT / "showcase" / "portfolio.json"
SHOWCASE_DEMO_PACKAGES = ROOT / "showcase" / "demo_packages.json"
PACKAGE_WORKSPACE_MIRROR_SEMANTICS = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "package_ecosystem"
    / "local_workspace_mirror_semantics.json"
)
PACKAGE_DEPENDENCY_LOCK_POLICY = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "package_ecosystem"
    / "dependency_lock_policy.json"
)
PACKAGE_REGISTRY_MIRROR_REPRODUCIBILITY = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "package_ecosystem"
    / "registry_mirror_reproducibility_contract.json"
)
PACKAGE_MIXED_IMAGE_METADATA = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "package_ecosystem"
    / "mixed_image_interop_loader_metadata.json"
)


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
        source_path = str(module.get("source", ""))
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
                "workspace_root": str(module.get("workspace_root", "")),
                "manifest_path": str(module.get("manifest", "")),
                "source_authority": "stdlib/module_inventory.json",
                "definition": package_location(source_path, module_name),
            }
        )
    return entries


def showcase_package_entries() -> list[dict[str, Any]]:
    portfolio = load_json(SHOWCASE_PORTFOLIO)
    entries: list[dict[str, Any]] = []
    for example in portfolio.get("examples", []):
        if not isinstance(example, dict):
            continue
        example_id = str(example.get("id", ""))
        source_path = str(example.get("source", ""))
        if not example_id or not source_path:
            continue
        module_name = str(example.get("title") or example_id)
        entries.append(
            {
                "package_id": f"showcase:{example_id}",
                "package_kind": "showcase-example",
                "module_name": module_name,
                "source_path": source_path,
                "workspace_manifest": str(example.get("workspace_manifest", "")),
                "source_authority": "showcase/portfolio.json",
                "story_capabilities": list(example.get("story_capabilities", [])),
                "stdlib_followup_modules": list(
                    example.get("stdlib_followup_modules", [])
                ),
                "definition": package_location(source_path, module_name),
            }
        )
    return entries


def cross_package_edges(packages: list[dict[str, Any]]) -> list[dict[str, str]]:
    package_ids = {str(package["package_id"]) for package in packages}
    edges: list[dict[str, str]] = []
    for package in packages:
        if package.get("package_kind") != "showcase-example":
            continue
        from_package = str(package["package_id"])
        for module_name in package.get("stdlib_followup_modules", []):
            to_package = f"stdlib:{module_name}"
            if to_package in package_ids:
                edges.append(
                    {
                        "from_package_id": from_package,
                        "to_package_id": to_package,
                        "edge_kind": "stdlib-followup-module",
                        "source_authority": str(package.get("source_authority", "")),
                    }
                )
    return sorted(
        edges,
        key=lambda edge: (
            edge["from_package_id"],
            edge["to_package_id"],
            edge["edge_kind"],
        ),
    )


def package_symbol_records(packages: list[dict[str, Any]]) -> list[dict[str, Any]]:
    records: list[dict[str, Any]] = []
    for package in packages:
        location = package["definition"]
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


def package_guardrails() -> dict[str, Any]:
    workspace_semantics = load_json(PACKAGE_WORKSPACE_MIRROR_SEMANTICS)
    dependency_lock = load_json(PACKAGE_DEPENDENCY_LOCK_POLICY)
    registry_mirror = load_json(PACKAGE_REGISTRY_MIRROR_REPRODUCIBILITY)
    mixed_image = load_json(PACKAGE_MIXED_IMAGE_METADATA)
    demo_packages = load_json(SHOWCASE_DEMO_PACKAGES)

    lockfile_semantics = workspace_semantics.get("lockfile_semantics", {})
    offline_mirror_semantics = workspace_semantics.get("offline_mirror_semantics", {})
    forbidden_sources = set(dependency_lock.get("forbidden_dependency_sources", []))
    blocker_conditions = set(registry_mirror.get("blocker_metadata", {}).get("blocking_conditions", []))
    tamper_diagnostics = [
        str(package.get("tamper_rejection", {}).get("diagnostic_code", ""))
        for package in mixed_image.get("packages", [])
        if isinstance(package, dict)
    ]
    demo_repro = demo_packages.get("reproducibility_contract", {})

    checks = {
        "lockfile_deterministic": lockfile_semantics.get("ordering")
        == "stable-by-package-id-then-source-path",
        "offline_mirror_lock_derived": offline_mirror_semantics.get("index_model")
        == "lock-derived-package-index",
        "offline_mirror_no_network": offline_mirror_semantics.get("network_policy")
        == "no-network-during-validation",
        "tamper_rejection_diagnostic_owned": "O3PKG8054" in tamper_diagnostics
        and "offline mirror metadata digest mismatch did not fail closed"
        in blocker_conditions,
        "mixed_version_policy_owned": demo_repro.get("mixed_version_policy")
        == "same-package-manifest-version-required",
        "tmp_not_source_authority": "tmp-only-source-of-truth" in forbidden_sources,
    }
    return {
        "contract_id": "objc3c.developer.tooling.workspace.package.guardrails.v1",
        "source_contracts": [
            "tests/tooling/fixtures/package_ecosystem/local_workspace_mirror_semantics.json",
            "tests/tooling/fixtures/package_ecosystem/dependency_lock_policy.json",
            "tests/tooling/fixtures/package_ecosystem/registry_mirror_reproducibility_contract.json",
            "tests/tooling/fixtures/package_ecosystem/mixed_image_interop_loader_metadata.json",
            "showcase/demo_packages.json",
        ],
        "checks": checks,
        "ok": all(checks.values()),
    }


def build_workspace_index(
    source_path: str,
    module_name: str,
    manifest_path: str | None,
    symbols: list[dict[str, Any]],
    source_index: dict[str, Any] | None = None,
) -> dict[str, Any]:
    packages = [
        local_source_package(source_path, module_name, manifest_path, symbols),
        *stdlib_package_entries(),
        *showcase_package_entries(),
    ]
    packages = sorted(packages, key=lambda package: str(package["package_id"]))
    edges = cross_package_edges(packages)
    package_symbols = package_symbol_records(packages)
    guardrails = package_guardrails()
    digest_inputs = {
        "package_ids": [package["package_id"] for package in packages],
        "source_paths": [package["source_path"] for package in packages],
        "edges": edges,
        "guardrail_checks": guardrails["checks"],
        "source_index_digest": source_index.get("source_index_digest", "")
        if isinstance(source_index, dict)
        else "",
    }
    evidence_roots = list(dict.fromkeys([
        "stdlib/module_inventory.json",
        "showcase/portfolio.json",
        "showcase/demo_packages.json",
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
