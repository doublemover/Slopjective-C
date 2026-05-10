"""Output rendering for runtime architecture proof packets."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel

from .contracts import PROOF_PACKET_PATH


def write_proof_packet(payload: dict[str, Any]) -> Path:
    PROOF_PACKET_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(PROOF_PACKET_PATH, payload)
    return PROOF_PACKET_PATH


def render_summary_path(summary_path: Path = PROOF_PACKET_PATH) -> None:
    print(f"summary_path: {repo_rel(summary_path)}")
