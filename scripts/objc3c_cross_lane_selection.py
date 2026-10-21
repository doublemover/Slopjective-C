"""Selection and run identity contracts for cross-lane E2E execution."""
from __future__ import annotations

import hashlib
import json
import re
from pathlib import Path
from typing import Any, Iterable

PHASES = ("provider", "consumer-compile", "link-run", "debugger", "reflection", "optimization", "package-producer", "clean-install-consumer", "distribution")


def _safe_manifest_path(root: Path, value: str, field: str) -> Path:
    path = Path(value)
    candidate = path if path.is_absolute() else root / path
    try:
        candidate.resolve().relative_to(root.resolve())
    except ValueError as error:
        raise ValueError(f"{field} escapes the repository root") from error
    return candidate

def validate_manifest_inventory(manifest: dict[str, Any], root: Path | None = None) -> list[dict[str, Any]]:
    if not isinstance(manifest, dict) or not isinstance(manifest.get("families"), list):
        raise ValueError("manifest families must be an array")
    root = root or Path.cwd()
    families: list[dict[str, Any]] = []
    seen: set[str] = set()
    for index, raw in enumerate(manifest["families"]):
        family = raw.get("family") if isinstance(raw, dict) and "family" in raw else raw
        if not isinstance(family, dict):
            raise ValueError(f"families[{index}] must be an object")
        family_id = family.get("family_id")
        if not isinstance(family_id, str) or not family_id.strip() or family_id in seen:
            raise ValueError(f"families[{index}].family_id must be unique and non-empty")
        seen.add(family_id)
        for key in ("source", "native_meta", "workspace_manifest", "expectation"):
            value = family.get(key)
            if not isinstance(value, str) or not value.strip():
                raise ValueError(f"{family_id}.{key} must be a non-empty path")
            _safe_manifest_path(root, value, f"{family_id}.{key}")
        families.append(family)
    return families


def _family_nodes(family: dict[str, Any], root: Path) -> list[dict[str, Any]]:
    family_id = str(family["family_id"])
    workspace_path = _safe_manifest_path(root, str(family["workspace_manifest"]), f"{family_id}.workspace_manifest")
    workspace = json.loads(workspace_path.read_text(encoding="utf-8"))
    expectation_path = _safe_manifest_path(root, str(family["expectation"]), f"{family_id}.expectation")
    expectation = json.loads(expectation_path.read_text(encoding="utf-8"))
    modules = workspace.get("package_modules", [])
    if not isinstance(modules, list):
        raise ValueError(f"{family_id}.package_modules must be an array")
    provider_ids: list[str] = []
    nodes: list[dict[str, Any]] = []
    for module in modules:
        if not isinstance(module, dict) or not isinstance(module.get("source"), str):
            raise ValueError(f"{family_id} has malformed provider module")
        provider_source = _safe_manifest_path(root, module["source"], f"{family_id}.package_modules.source")
        node_id = f"{family_id}:provider:{module.get('module_name', provider_source.stem)}"
        provider_ids.append(node_id)
        nodes.append({"node_id": node_id, "family_id": family_id, "phase": "provider", "source": module["source"], "identity_inputs": [module["source"], str(family["workspace_manifest"])], "depends_on": []})
    provider_by_package = {
        str(module.get("package_id")): f"{family_id}:provider:{module.get('module_name', Path(str(module['source'])).stem)}"
        for module in modules
        if isinstance(module, dict) and isinstance(module.get("package_id"), str)
    }
    provider_nodes = {node["node_id"]: node for node in nodes}
    for edge_index, edge in enumerate(workspace.get("package_edges", [])):
        if not isinstance(edge, dict):
            raise ValueError(f"{family_id}.package_edges[{edge_index}] must be an object")
        owner = provider_by_package.get(str(edge.get("from", "")))
        dependency = provider_by_package.get(str(edge.get("to", "")))
        if owner and dependency:
            provider_nodes[owner]["depends_on"].append(dependency)
    family_inputs = [str(family[key]) for key in ("source", "native_meta", "workspace_manifest", "expectation")]
    consumer = f"{family_id}:consumer"
    link_run = f"{family_id}:link-run"
    nodes.extend([
        {"node_id": consumer, "family_id": family_id, "phase": "consumer-compile", "source": family["source"], "identity_inputs": family_inputs, "depends_on": provider_ids},
        {"node_id": link_run, "family_id": family_id, "phase": "link-run", "source": family["source"], "identity_inputs": family_inputs, "depends_on": [consumer, *provider_ids]},
    ])
    debug_status = expectation.get("debug_source_map", {}).get("status")
    if family_id == "object_reflection_debugger" or debug_status == "expected-pass":
        nodes.append({"node_id": f"{family_id}:debugger", "family_id": family_id, "phase": "debugger", "source": family["source"], "identity_inputs": family_inputs, "depends_on": [link_run]})
        nodes.append({"node_id": f"{family_id}:reflection", "family_id": family_id, "phase": "reflection", "source": family["source"], "identity_inputs": family_inputs, "depends_on": [link_run]})
    if expectation.get("optimization_trace", {}).get("status") == "expected-pass":
        nodes.append({"node_id": f"{family_id}:optimization", "family_id": family_id, "phase": "optimization", "source": family["source"], "identity_inputs": family_inputs, "depends_on": [link_run]})
    if expectation.get("distribution_lifecycle", {}).get("status") == "expected-pass":
        producer = f"{family_id}:package-producer"
        clean_consumer = f"{family_id}:clean-install-consumer"
        nodes.append({"node_id": producer, "family_id": family_id, "phase": "package-producer", "source": family["source"], "identity_inputs": family_inputs, "depends_on": [link_run]})
        nodes.append({"node_id": clean_consumer, "family_id": family_id, "phase": "clean-install-consumer", "source": family["source"], "identity_inputs": family_inputs, "depends_on": [producer]})
        nodes.append({"node_id": f"{family_id}:distribution", "family_id": family_id, "phase": "distribution", "source": family["source"], "identity_inputs": family_inputs, "depends_on": [clean_consumer]})
    return nodes


def _topological_nodes(nodes: list[dict[str, Any]]) -> list[dict[str, Any]]:
    order = {str(node["node_id"]): index for index, node in enumerate(nodes)}
    if len(order) != len(nodes):
        raise ValueError("execution graph contains duplicate node ids")
    pending = {node_id: set(node.get("depends_on", [])) for node_id, node in
               ((str(node["node_id"]), node) for node in nodes)}
    for node_id, dependencies in pending.items():
        missing = dependencies - set(order)
        if missing:
            raise ValueError(f"{node_id} has unknown dependencies: " + ", ".join(sorted(missing)))
    result: list[dict[str, Any]] = []
    emitted: set[str] = set()
    while len(result) != len(nodes):
        ready = [node_id for node_id in pending
                 if node_id not in emitted and pending[node_id] <= emitted]
        if not ready:
            raise ValueError("execution graph contains a dependency cycle")
        ready.sort(key=order.__getitem__)
        for node_id in ready:
            emitted.add(node_id)
            result.append(nodes[order[node_id]])
    return result


def resolve_execution_plan(
    manifest: dict[str, Any],
    root: Path,
    families: Iterable[str] = (),
    phase: str | None = None,
    node_ids: Iterable[str] = (),
) -> dict[str, Any]:
    rows = validate_manifest_inventory(manifest, root)
    selected_families = set(families)
    known = {str(row["family_id"]) for row in rows}
    unknown = selected_families - known
    if unknown:
        raise ValueError("unknown family selector: " + ", ".join(sorted(unknown)))
    if phase is not None and phase not in PHASES:
        raise ValueError("unknown phase selector: " + phase)
    nodes = _topological_nodes([node for family in rows for node in _family_nodes(family, root)])
    node_map = {node["node_id"]: node for node in nodes}
    requested_nodes = set(node_ids)
    unknown_nodes = requested_nodes - set(node_map)
    if unknown_nodes:
        raise ValueError("unknown node selector: " + ", ".join(sorted(unknown_nodes)))
    selected = {
        node_id for node_id, node in node_map.items()
        if (not selected_families or node["family_id"] in selected_families)
        and (phase is None or node["phase"] == phase)
    }
    if requested_nodes:
        incompatible = {
            node_id for node_id in requested_nodes
            if ((selected_families and node_map[node_id]["family_id"] not in selected_families)
                or (phase is not None and node_map[node_id]["phase"] != phase))
        }
        if incompatible:
            raise ValueError("node selector conflicts with family/phase filter: " +
                             ", ".join(sorted(incompatible)))
        selected = {
            node_id for node_id in requested_nodes
            if not selected_families or node_map[node_id]["family_id"] in selected_families
        }
    pending = list(selected)
    while pending:
        node = node_map[pending.pop()]
        for dependency in node["depends_on"]:
            if dependency not in selected:
                selected.add(dependency)
                pending.append(dependency)
    if not selected:
        raise ValueError("selectors do not match any execution node")
    ordered = [node for node in nodes if node["node_id"] in selected]
    return {
        "nodes": nodes,
        "selected_nodes": ordered,
        "selected_node_ids": [node["node_id"] for node in ordered],
        "unselected_node_ids": [node["node_id"] for node in nodes if node["node_id"] not in selected],
        "selected_families": sorted({node["family_id"] for node in ordered}),
        "unselected_families": sorted(known - {node["family_id"] for node in ordered}),
        "selection_complete": True,
    }


def digest_files(paths: Iterable[Path]) -> str:
    digest = hashlib.sha256()
    for path in sorted({path.resolve() for path in paths}, key=lambda value: value.as_posix()):
        digest.update(path.as_posix().encode())
        digest.update(b"\0")
        if not path.is_file():
            raise ValueError(f"identity input missing: {path}")
        with path.open("rb") as handle:
            for block in iter(lambda: handle.read(1024 * 1024), b""):
                digest.update(block)
        digest.update(b"\0")
    return digest.hexdigest()


def safe_run_id(value: str) -> str:
    if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9._-]{0,79}", value):
        raise ValueError("run id must contain 1-80 safe filename characters")
    return value



def execution_node_statuses(plan: dict[str, Any], family_summaries: dict[str, dict[str, Any]], failures: list[dict[str, str]]) -> list[dict[str, str]]:
    failed_by_family: dict[str, set[str]] = {}
    blocked_by_family: dict[str, set[str]] = {}
    failure_by_family = {row["family_id"]: row.get("error", "") for row in failures}
    for family_id, message in failure_by_family.items():
        nodes = [node for node in plan["selected_nodes"] if node["family_id"] == family_id]
        phase_tokens = {
            "provider": ("provider", "package module"),
            "consumer-compile": ("consumer compile", "consumer"),
            "link-run": ("link", "runtime status", "execution"),
            "debugger": ("debugger", "source-map/debug"),
            "reflection": ("reflection",),
            "optimization": ("optimization", "ir proof"),
            "package-producer": ("package producer", "mirror generator"),
            "clean-install-consumer": ("clean install", "package consumer"),
            "distribution": ("distribution", "package lifecycle"),
        }
        failed = {node["node_id"] for node in nodes if any(token in message.lower() for token in phase_tokens.get(node["phase"], ()))}
        if not failed and nodes:
            failed = {nodes[0]["node_id"]}
        blocked: set[str] = set()
        changed = True
        while changed:
            changed = False
            for node in nodes:
                if node["node_id"] not in failed and node["node_id"] not in blocked and any(dep in failed or dep in blocked for dep in node["depends_on"]):
                    blocked.add(node["node_id"])
                    changed = True
        failed_by_family[family_id], blocked_by_family[family_id] = failed, blocked
    selected = set(plan["selected_node_ids"])
    results = []
    for node in plan["nodes"]:
        family_id, node_id = node["family_id"], node["node_id"]
        if node_id not in selected:
            status = "unselected"
        elif family_id not in family_summaries:
            status = "dependency-blocked"
        elif node_id in failed_by_family.get(family_id, set()):
            status = "failed"
        elif node_id in blocked_by_family.get(family_id, set()):
            status = "dependency-blocked"
        else:
            status = "completed"
        results.append({"node_id": node_id, "family_id": family_id, "phase": node["phase"], "status": status})
    return results
