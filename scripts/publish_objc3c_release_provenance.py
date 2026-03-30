#!/usr/bin/env python3
"""Publish SBOM and attestation artifacts for the canonical objc3c release payload."""

from __future__ import annotations

import hashlib
import json
from collections import defaultdict
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
MANIFEST_PATH = ROOT / 'tmp' / 'artifacts' / 'release-foundation' / 'manifest' / 'objc3c-release-manifest.json'
SBOM_PATH = ROOT / 'tmp' / 'artifacts' / 'release-foundation' / 'sbom' / 'objc3c-release-sbom.json'
ATTESTATION_PATH = ROOT / 'tmp' / 'artifacts' / 'release-foundation' / 'attestation' / 'objc3c-release-attestation.json'
SUMMARY_PATH = ROOT / 'tmp' / 'reports' / 'release-foundation' / 'publication-summary.json'
PROVENANCE_POLICY = ROOT / 'tests' / 'tooling' / 'fixtures' / 'release_foundation' / 'provenance_policy.json'


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace('\\', '/')


def sha256_file(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding='utf-8'))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


def main() -> int:
    if not MANIFEST_PATH.is_file():
        raise RuntimeError(f"missing release manifest {repo_rel(MANIFEST_PATH)}")
    manifest = load_json(MANIFEST_PATH)
    provenance_policy = load_json(PROVENANCE_POLICY)

    grouped_entries: dict[str, list[dict[str, Any]]] = defaultdict(list)
    for entry in manifest['release_payload_entries']:
        grouped_entries[entry['component_group']].append({
            'path': entry['path'],
            'sha256': entry['sha256'],
            'byte_count': entry['byte_count'],
        })

    component_groups = []
    for group_id in sorted(grouped_entries):
        entries = sorted(grouped_entries[group_id], key=lambda item: item['path'])
        component_groups.append({
            'group_id': group_id,
            'file_count': len(entries),
            'entries': entries,
        })

    sbom = {
        'contract_id': provenance_policy['sbom_contract_id'],
        'schema_version': 1,
        'generated_at_utc': datetime.now(timezone.utc).isoformat().replace('+00:00', 'Z'),
        'release_manifest_path': repo_rel(MANIFEST_PATH),
        'release_payload_digest_sha256': manifest['release_payload_digest_sha256'],
        'component_groups': component_groups,
    }
    SBOM_PATH.parent.mkdir(parents=True, exist_ok=True)
    SBOM_PATH.write_text(json.dumps(sbom, indent=2) + '\n', encoding='utf-8')

    release_manifest_sha256 = sha256_file(MANIFEST_PATH)
    sbom_sha256 = sha256_file(SBOM_PATH)
    attestation = {
        'contract_id': provenance_policy['attestation_contract_id'],
        'schema_version': 1,
        'generated_at_utc': datetime.now(timezone.utc).isoformat().replace('+00:00', 'Z'),
        'release_manifest_path': repo_rel(MANIFEST_PATH),
        'sbom_path': repo_rel(SBOM_PATH),
        'attested_digests': {
            'release_manifest_sha256': release_manifest_sha256,
            'release_payload_digest_sha256': manifest['release_payload_digest_sha256'],
            'package_manifest_sha256': manifest['primary_package_manifest_sha256'],
            'sbom_sha256': sbom_sha256,
            'repo_superclean_surface_sha256': manifest['repo_superclean_surface_sha256'],
            'release_evidence_index_sha256': manifest['release_evidence_index_sha256'],
        },
        'source_stamps': {
            'git_commit': manifest['source_stamps']['git_commit'],
            'git_tree_dirty': manifest['source_stamps']['git_tree_dirty'],
            'package_root': manifest['primary_package_root'],
            'package_manifest_path': manifest['primary_package_manifest_path'],
            'release_manifest_path': repo_rel(MANIFEST_PATH),
            'sbom_path': repo_rel(SBOM_PATH),
            'attestation_path': repo_rel(ATTESTATION_PATH),
        },
    }
    ATTESTATION_PATH.parent.mkdir(parents=True, exist_ok=True)
    ATTESTATION_PATH.write_text(json.dumps(attestation, indent=2) + '\n', encoding='utf-8')

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        'contract_id': 'objc3c.release.foundation.publication.summary.v1',
        'status': 'PASS',
        'release_manifest_path': repo_rel(MANIFEST_PATH),
        'release_manifest_sha256': release_manifest_sha256,
        'sbom_path': repo_rel(SBOM_PATH),
        'sbom_sha256': sbom_sha256,
        'attestation_path': repo_rel(ATTESTATION_PATH),
        'attestation_sha256': sha256_file(ATTESTATION_PATH),
        'component_group_count': len(component_groups),
    }
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + '\n', encoding='utf-8')
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print(f"published_manifest: {repo_rel(MANIFEST_PATH)}")
    print(f"published_sbom: {repo_rel(SBOM_PATH)}")
    print(f"published_attestation: {repo_rel(ATTESTATION_PATH)}")
    print('objc3c-release-provenance: PASS')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
