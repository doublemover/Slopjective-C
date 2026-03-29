#!/usr/bin/env python3
"""Validate the checked-in stdlib boundary and canonical module inventory."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
WORKSPACE_PATH = ROOT / "stdlib" / "workspace.json"
MODULE_INVENTORY_PATH = ROOT / "stdlib" / "module_inventory.json"
STABILITY_POLICY_PATH = ROOT / "stdlib" / "stability_policy.json"
SPEC_CONTRACT_PATH = ROOT / "spec" / "STANDARD_LIBRARY_CONTRACT.md"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "stdlib" / "surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stdlib.surface.summary.v1"
TABLE_ROW_RE = re.compile(
    r"^\|\s*`(?P<module>[^`]+)`\s*\|\s*`(?P<capability>[^`]+)`\s*\|\s*(?P<profile>[^|]+?)\s*\|$"
)


def fail(message: str) -> int:
    print(f"stdlib-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, object]:
    return json.loads(path.read_text(encoding="utf-8"))


def parse_spec_canonical_modules(spec_text: str) -> list[dict[str, str]]:
    rows: list[dict[str, str]] = []
    for raw_line in spec_text.splitlines():
        match = TABLE_ROW_RE.match(raw_line.strip())
        if not match:
            continue
        module = match.group("module")
        capability = match.group("capability")
        if not module.startswith("objc3."):
            continue
        rows.append(
            {
                "module": module,
                "capability_id": capability,
                "required_profile": match.group("profile").strip(),
            }
        )
    return rows


def main() -> int:
    if not WORKSPACE_PATH.is_file():
        return fail(f"missing workspace contract: {repo_rel(WORKSPACE_PATH)}")
    if not MODULE_INVENTORY_PATH.is_file():
        return fail(f"missing module inventory: {repo_rel(MODULE_INVENTORY_PATH)}")
    if not STABILITY_POLICY_PATH.is_file():
        return fail(f"missing stability policy: {repo_rel(STABILITY_POLICY_PATH)}")
    if not SPEC_CONTRACT_PATH.is_file():
        return fail(f"missing spec contract: {repo_rel(SPEC_CONTRACT_PATH)}")

    workspace = load_json(WORKSPACE_PATH)
    inventory = load_json(MODULE_INVENTORY_PATH)
    stability_policy = load_json(STABILITY_POLICY_PATH)
    spec_text = SPEC_CONTRACT_PATH.read_text(encoding="utf-8")

    if workspace.get("contract_id") != "objc3c.stdlib.workspace.v1":
        return fail("workspace contract_id drifted")
    if workspace.get("schema_version") != 1:
        return fail("workspace schema_version drifted")
    if workspace.get("module_inventory") != "stdlib/module_inventory.json":
        return fail("workspace module_inventory path drifted")
    if workspace.get("stability_policy") != "stdlib/stability_policy.json":
        return fail("workspace stability_policy path drifted")
    if inventory.get("contract_id") != "objc3c.stdlib.module_inventory.v1":
        return fail("module inventory contract_id drifted")
    if inventory.get("schema_version") != 1:
        return fail("module inventory schema_version drifted")
    if inventory.get("spec_contract") != "spec/STANDARD_LIBRARY_CONTRACT.md":
        return fail("module inventory spec_contract drifted")
    if stability_policy.get("contract_id") != "objc3c.stdlib.stability_policy.v1":
        return fail("stability policy contract_id drifted")
    if stability_policy.get("schema_version") != 1:
        return fail("stability policy schema_version drifted")

    canonical_modules = inventory.get("canonical_modules")
    if not isinstance(canonical_modules, list) or not canonical_modules:
        return fail("module inventory missing canonical_modules")

    inventory_rows: list[dict[str, str]] = []
    for entry in canonical_modules:
        if not isinstance(entry, dict):
            return fail("module inventory entry must be an object")
        module = entry.get("module")
        capability_id = entry.get("capability_id")
        required_profile = entry.get("required_profile")
        if not all(isinstance(value, str) and value for value in (module, capability_id, required_profile)):
            return fail("module inventory entry is missing module/capability_id/required_profile")
        inventory_rows.append(
            {
                "module": module,
                "capability_id": capability_id,
                "required_profile": required_profile,
            }
        )

    spec_rows = parse_spec_canonical_modules(spec_text)
    if spec_rows != inventory_rows:
        return fail("module inventory drifted from spec canonical module table")

    layers = stability_policy.get("layers")
    if not isinstance(layers, list) or not layers:
        return fail("stability policy missing layers")
    inventory_module_names = {entry["module"] for entry in inventory_rows}
    covered_modules: set[str] = set()
    for layer in layers:
        if not isinstance(layer, dict):
            return fail("stability policy layer must be an object")
        layer_name = layer.get("name")
        modules = layer.get("modules")
        allowed_dependencies = layer.get("allowed_dependencies")
        if not isinstance(layer_name, str) or not layer_name:
            return fail("stability policy layer missing name")
        if not isinstance(modules, list) or not modules:
            return fail(f"stability policy layer {layer_name} missing modules")
        if not isinstance(allowed_dependencies, list):
            return fail(f"stability policy layer {layer_name} missing allowed_dependencies")
        for module_name in modules:
            if not isinstance(module_name, str) or not module_name:
                return fail(f"stability policy layer {layer_name} published an invalid module")
            if module_name not in inventory_module_names:
                return fail(f"stability policy layer {layer_name} referenced unknown module {module_name}")
            covered_modules.add(module_name)
        for dependency in allowed_dependencies:
            if not isinstance(dependency, str) or not dependency:
                return fail(f"stability policy layer {layer_name} published an invalid dependency")
            if dependency not in inventory_module_names:
                return fail(
                    f"stability policy layer {layer_name} referenced unknown dependency {dependency}"
                )
    if covered_modules != inventory_module_names:
        return fail("stability policy module coverage drifted from module inventory")

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(
        json.dumps(
            {
                "contract_id": SUMMARY_CONTRACT_ID,
                "schema_version": 1,
                "status": "PASS",
                "workspace_contract": repo_rel(WORKSPACE_PATH),
                "module_inventory": repo_rel(MODULE_INVENTORY_PATH),
                "stability_policy": repo_rel(STABILITY_POLICY_PATH),
                "spec_contract": repo_rel(SPEC_CONTRACT_PATH),
                "canonical_modules": inventory_rows,
                "layers": layers,
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
