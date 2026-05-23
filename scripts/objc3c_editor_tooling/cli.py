from __future__ import annotations

import argparse
import sys

from objc3c_editor_tooling.input_loading import (
    load_editor_tooling_inputs,
    publish_diagnostics_only_summary,
    run_frontend_compile,
)
from objc3c_editor_tooling.model import build_editor_tooling_model
from objc3c_editor_tooling.paths import default_source_argument, paths_for_source, resolve_source
from objc3c_editor_tooling.publication import publish_editor_tooling_surface
from objc3c_editor_tooling.validation import compile_summary_exit_code


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--source-graph-only",
        action="store_true",
        help="publish the editor tooling surface and print only the source graph artifact path",
    )
    parser.add_argument(
        "--artifact-inspector-only",
        action="store_true",
        help="publish the editor tooling surface and print only the artifact inspector path",
    )
    parser.add_argument("source", nargs="?", default=default_source_argument())
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    paths = paths_for_source(resolve_source(args.source))
    compile_result = run_frontend_compile(paths)
    if compile_result.stdout:
        sys.stdout.write(compile_result.stdout)
    if compile_result.stderr:
        sys.stderr.write(compile_result.stderr)

    summary_exit_code = compile_summary_exit_code(compile_result)
    if summary_exit_code is not None:
        if not publish_diagnostics_only_summary(paths, compile_result):
            return summary_exit_code

    inputs = load_editor_tooling_inputs(paths)
    model = build_editor_tooling_model(paths, inputs)
    published = publish_editor_tooling_surface(paths=paths, inputs=inputs, model=model)

    if args.source_graph_only:
        print(f"summary_path: {published.summary_path}")
        print(f"source_graph_path: {published.source_graph_path}")
        return 0
    if args.artifact_inspector_only:
        artifact = model.artifact_inspector
        inventory = artifact.get("inventory_validation", {})
        object_inventory = artifact.get("object", {})
        runtime_inventory = artifact.get("runtime_inventory", {})
        package_inventory = artifact.get("package_inventory", {})
        fail_closed_reasons = inventory.get("fail_closed_reasons", [])
        print(f"summary_path: {published.summary_path}")
        print(f"artifact_inspector_path: {published.artifact_inspector_path}")
        print(f"inventory_ready: {str(inventory.get('inventory_ready') is True).lower()}")
        print(f"fail_closed: {str(inventory.get('fail_closed') is True).lower()}")
        fail_closed_reason_text = (
            ",".join(str(reason) for reason in fail_closed_reasons)
            if isinstance(fail_closed_reasons, list)
            else ""
        )
        print(f"fail_closed_reasons: {fail_closed_reason_text}")
        print(f"object_format: {object_inventory.get('object_format', '')}")
        print(f"symbol_count: {object_inventory.get('symbol_count', 0)}")
        print(f"object_inventory_digest: {object_inventory.get('inventory_digest', '')}")
        print(f"runtime_class_records: {runtime_inventory.get('class_record_count', 0)}")
        print(f"runtime_inventory_digest: {runtime_inventory.get('inventory_digest', '')}")
        print(f"package_identity: {package_inventory.get('package_identity', '')}")
        print(f"abi_identity: {package_inventory.get('abi_identity', '')}")
        return 0 if inventory.get("fail_closed") is not True else 2

    print(f"summary_path: {published.summary_path}")
    print(f"dump_path: {published.dump_path}")
    print(f"capabilities_path: {published.capabilities_path}")
    print(f"navigation_path: {published.navigation_path}")
    print(f"workspace_index_path: {published.workspace_index_path}")
    print(f"source_graph_path: {published.source_graph_path}")
    print(f"artifact_inspector_path: {published.artifact_inspector_path}")
    print(f"formatter_path: {published.formatter_path}")
    print(f"debug_path: {published.debug_path}")
    return 0
