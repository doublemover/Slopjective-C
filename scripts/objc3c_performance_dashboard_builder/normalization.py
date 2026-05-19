from __future__ import annotations

from typing import Any


def normalize_packet_path(path: str) -> str:
    return path.replace("\\", "/")


def collect_packet_paths(*summaries: dict[str, Any]) -> list[str]:
    packet_paths: list[str] = []
    for summary in summaries:
        for relative_path in summary.get("telemetry_packets", summary.get("packet_paths", [])):
            if isinstance(relative_path, str):
                packet_paths.append(normalize_packet_path(relative_path))
    return packet_paths


def collect_comparative_packets(
    comparative_summary: dict[str, Any],
    *,
    marker: str,
) -> list[str]:
    return [
        raw_path
        for raw_path in comparative_summary.get("telemetry_packets", [])
        if isinstance(raw_path, str) and marker in normalize_packet_path(raw_path)
    ]
