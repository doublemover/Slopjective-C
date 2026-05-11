"""Distribution-credibility source-surface validation entrypoint."""

from __future__ import annotations

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_report_json
from objc3c_tooling.paths import repo_rel

from .constants import EXPECTED_CONTRACT_IDS, SOURCE_SURFACE, SUMMARY_CONTRACT_ID, SUMMARY_PATH, SURFACE_CONTRACT_ID, ROOT
from .failures import fail
from .validators import validate_contract_payload


def main() -> int:
    if not SOURCE_SURFACE.is_file():
        return fail(f"missing source surface {repo_rel(SOURCE_SURFACE)}")
    source_surface = load_json(SOURCE_SURFACE)
    if source_surface.get("contract_id") != SURFACE_CONTRACT_ID:
        return fail("unexpected source surface contract_id")
    if source_surface.get("surface_kind") != "distribution-credibility-source-surface":
        return fail("unexpected source surface kind")
    if source_surface.get("schema_version") != 1:
        return fail("unexpected source surface schema_version")

    checked_paths: list[str] = [repo_rel(SOURCE_SURFACE)]
    for field_name, expected_contract_id in EXPECTED_CONTRACT_IDS.items():
        try:
            checked_paths.append(validate_referenced_contract(source_surface, field_name, expected_contract_id))
        except RuntimeError as exc:
            return fail(str(exc))

    runbook = source_surface.get("runbook")
    if not isinstance(runbook, str) or not runbook:
        return fail("runbook was missing from the source surface")
    runbook_path = ROOT / runbook
    if not runbook_path.is_file():
        return fail(f"runbook referenced missing file {runbook}")
    checked_paths.append(runbook)

    try:
        checked_paths.extend(validate_source_lists(source_surface))
    except RuntimeError as exc:
        return fail(str(exc))

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "source_surface": repo_rel(SOURCE_SURFACE),
        "checked_path_count": len(sorted(set(checked_paths))),
        "checked_paths": sorted(set(checked_paths)),
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("distribution-credibility-source-surface: OK")
    return 0


def validate_referenced_contract(source_surface: dict[str, object], field_name: str, expected_contract_id: str) -> str:
    raw_path = source_surface.get(field_name)
    if not isinstance(raw_path, str) or not raw_path:
        raise RuntimeError(f"{field_name} was missing from the source surface")
    target = ROOT / raw_path
    if not target.is_file():
        raise RuntimeError(f"{field_name} referenced missing file {raw_path}")
    payload = load_json(target)
    if payload.get("contract_id") != expected_contract_id:
        raise RuntimeError(f"{field_name} drifted from expected contract id {expected_contract_id}")
    validate_contract_payload(field_name, payload)
    return raw_path


def validate_source_lists(source_surface: dict[str, object]) -> list[str]:
    checked_paths: list[str] = []
    for list_name in ("checked_in_sources", "machine_owned_output_roots", "explicit_non_goals"):
        items = source_surface.get(list_name)
        if not isinstance(items, list) or not items:
            raise RuntimeError(f"{list_name} must be a non-empty list")
        if list_name != "checked_in_sources":
            continue
        for raw_path in items:
            if not isinstance(raw_path, str) or not raw_path:
                raise RuntimeError(f"{list_name} contained an invalid path entry")
            target = ROOT / raw_path
            if not target.exists():
                raise RuntimeError(f"{list_name} referenced missing path {raw_path}")
            checked_paths.append(raw_path)
    return checked_paths
