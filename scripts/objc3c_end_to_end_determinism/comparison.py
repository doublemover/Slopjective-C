"""Deterministic digest comparison for replay runs."""

from __future__ import annotations

from typing import Sequence


def compare_runs(runs: Sequence[dict[str, object]]) -> list[dict[str, object]]:
    if not runs:
        return []
    baseline = runs[0]
    baseline_digests = baseline.get("_digest_by_path")
    if not isinstance(baseline_digests, dict):
        return [
            {
                "kind": "baseline-missing-artifacts",
                "run_id": str(baseline.get("run_id", "run01")),
                "message": "baseline run did not produce artifact digest evidence",
            }
        ]

    baseline_paths = sorted(str(path) for path in baseline_digests.keys())
    mismatches: list[dict[str, object]] = []
    for replay in runs[1:]:
        replay_id = str(replay.get("run_id", "unknown"))
        replay_digests = replay.get("_digest_by_path")
        if not isinstance(replay_digests, dict):
            mismatches.append(
                {
                    "kind": "replay-missing-artifacts",
                    "run_id": replay_id,
                    "message": "replay run did not produce artifact digest evidence",
                }
            )
            continue

        replay_paths = sorted(str(path) for path in replay_digests.keys())
        missing = sorted(set(baseline_paths) - set(replay_paths))
        unexpected = sorted(set(replay_paths) - set(baseline_paths))
        for path in missing:
            mismatches.append(
                {
                    "kind": "artifact-missing",
                    "run_id": replay_id,
                    "path": path,
                }
            )
        for path in unexpected:
            mismatches.append(
                {
                    "kind": "artifact-unexpected",
                    "run_id": replay_id,
                    "path": path,
                }
            )

        for path in sorted(set(baseline_paths) & set(replay_paths)):
            base_sha = str(baseline_digests[path])
            replay_sha = str(replay_digests[path])
            if base_sha != replay_sha:
                mismatches.append(
                    {
                        "kind": "artifact-digest-mismatch",
                        "run_id": replay_id,
                        "path": path,
                        "baseline_sha256": base_sha,
                        "replay_sha256": replay_sha,
                    }
                )
    return mismatches


__all__ = ["compare_runs"]
