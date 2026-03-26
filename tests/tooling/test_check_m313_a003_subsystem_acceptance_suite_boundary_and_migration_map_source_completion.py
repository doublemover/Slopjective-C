from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
MAP_PATH = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a003_subsystem_acceptance_suite_boundary_and_migration_map_source_completion_map.json"


def test_boundary_map_freezes_suite_ids() -> None:
    payload = json.loads(MAP_PATH.read_text(encoding="utf-8"))
    assert [entry["suite_id"] for entry in payload["acceptance_suite_boundaries"]] == [
        "runtime_bootstrap_dispatch",
        "frontend_split_recovery",
        "module_parity_packaging",
        "native_fixture_corpus_and_runtime_probes",
    ]


def test_boundary_map_freezes_owner_sequence_and_next_issue() -> None:
    payload = json.loads(MAP_PATH.read_text(encoding="utf-8"))
    assert payload["migration_owner_sequence"] == [
        "M313-B001",
        "M313-B002",
        "M313-B003",
        "M313-B004",
        "M313-C002",
        "M313-C003",
        "M313-D002",
        "M313-E002",
    ]
    assert payload["next_issue"] == "M313-B001"
