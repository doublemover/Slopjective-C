#!/usr/bin/env python3
"""Build the machine-owned release manifest for the canonical runnable package."""

from __future__ import annotations

import hashlib
import json
import os
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
RELEASE_EVIDENCE_PY = ROOT / "scripts" / "check_release_evidence.py"
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_foundation" / "source_surface.json"
PAYLOAD_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_foundation" / "release_payload_policy.json"
REPRO_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_foundation" / "reproducibility_policy.json"
MANIFEST_PATH = ROOT / "tmp" / "artifacts" / "release-foundation" / "manifest" / "objc3c-release-manifest.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-foundation" / "release-manifest-summary.json"
EVIDENCE_INDEX_PATH = ROOT / "tmp" / "reports" / "release_evidence" / "evidence-index.json"


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace('\\', '/')


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open('rb') as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b''):
            digest.update(chunk)
    return digest.hexdigest()


def sha256_text(text: str) -> str:
    return hashlib.sha256(text.encode('utf-8')).hexdigest()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding='utf-8'))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


def run(command: list[str]) -> None:
    env = os.environ.copy()
    env['PYTHONDONTWRITEBYTECODE'] = '1'
    result = subprocess.run(command, cwd=ROOT, env=env, text=True, capture_output=True, check=False)
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    if result.returncode != 0:
        joined = ' '.join(command)
        raise RuntimeError(f"command failed with exit code {result.returncode}: {joined}")


def git_output(*args: str) -> str:
    result = subprocess.run(['git', *args], cwd=ROOT, text=True, capture_output=True, check=False)
    if result.returncode != 0:
        raise RuntimeError(f"git {' '.join(args)} failed with exit code {result.returncode}")
    return result.stdout.strip()


def component_group_for_path(rel_path: str) -> str:
    normalized = rel_path.replace('\\', '/')
    if normalized == 'package.json':
        return 'scripts-and-runbooks'
    top = normalized.split('/', 1)[0]
    return {
        'artifacts': 'native-binaries',
        'scripts': 'scripts-and-runbooks',
        'docs': 'scripts-and-runbooks',
        'showcase': 'showcase-and-tutorials',
        'site': 'showcase-and-tutorials',
        'stdlib': 'stdlib',
        'tests': 'conformance-and-evidence',
        'spec': 'conformance-and-evidence',
        'schemas': 'conformance-and-evidence',
        'native': 'runtime-library',
    }.get(top, top)


def package_once(package_root: Path, manifest_relative_path: str) -> dict[str, Any]:
    package_root.mkdir(parents=True, exist_ok=True)
    run([
        PWSH,
        '-NoProfile',
        '-ExecutionPolicy',
        'Bypass',
        '-File',
        str(PACKAGE_PS1),
        '-PackageRoot',
        str(package_root),
        '-ManifestRelativePath',
        manifest_relative_path,
    ])
    manifest_path = package_root / Path(manifest_relative_path.replace('/', os.sep))
    if not manifest_path.is_file():
        raise RuntimeError(f"missing runnable package manifest {manifest_path}")
    package_manifest = load_json(manifest_path)
    copied_files = package_manifest.get('copied_files')
    if not isinstance(copied_files, list) or not copied_files:
        raise RuntimeError('runnable package manifest did not contain copied_files')
    entries: list[dict[str, Any]] = []
    for raw_path in copied_files:
        if not isinstance(raw_path, str) or not raw_path:
            raise RuntimeError('runnable package manifest contained an invalid copied file entry')
        file_path = package_root / Path(raw_path.replace('/', os.sep))
        if not file_path.is_file():
            raise RuntimeError(f"runnable package referenced missing file {raw_path}")
        entries.append({
            'path': raw_path,
            'sha256': sha256_file(file_path),
            'byte_count': file_path.stat().st_size,
            'component_group': component_group_for_path(raw_path),
        })
    entries.sort(key=lambda item: item['path'])
    payload_digest = sha256_text(''.join(
        f"{entry['path']}|{entry['sha256']}|{entry['byte_count']}|{entry['component_group']}\n"
        for entry in entries
    ))
    return {
        'package_root': package_root,
        'manifest_path': manifest_path,
        'package_manifest': package_manifest,
        'entries': entries,
        'payload_digest': payload_digest,
    }


def main() -> int:
    load_json(SOURCE_SURFACE)
    payload_policy = load_json(PAYLOAD_POLICY)
    reproducibility_policy = load_json(REPRO_POLICY)

    run([sys.executable, str(RELEASE_EVIDENCE_PY)])
    if not EVIDENCE_INDEX_PATH.is_file():
        raise RuntimeError(f"missing release evidence index {repo_rel(EVIDENCE_INDEX_PATH)}")

    run_root = ROOT / 'tmp' / 'pkg' / 'objc3c-release-foundation' / datetime.now(timezone.utc).strftime('%Y%m%d_%H%M%S_%f')
    manifest_relative_path = 'artifacts/package/objc3c-runnable-toolchain-package.json'
    first = package_once(run_root / 'run-1', manifest_relative_path)
    second = package_once(run_root / 'run-2', manifest_relative_path)

    required_prefixes = payload_policy['required_payload_prefixes']
    for prefix in required_prefixes:
        if not any(entry['path'].startswith(prefix) for entry in first['entries']):
            raise RuntimeError(f"release payload did not include required prefix {prefix}")

    repo_superclean_rel = first['package_manifest']['repo_superclean_surface']
    repo_superclean_path = first['package_root'] / Path(repo_superclean_rel.replace('/', os.sep))
    if not repo_superclean_path.is_file():
        raise RuntimeError(f"missing packaged repo superclean surface {repo_superclean_rel}")

    reproducibility_match = (
        first['payload_digest'] == second['payload_digest']
        and first['entries'] == second['entries']
        and first['package_manifest']['copied_file_count'] == second['package_manifest']['copied_file_count']
    )
    if not reproducibility_match:
        raise RuntimeError('repeated runnable package assembly drifted across release payload digests')

    git_commit = git_output('rev-parse', 'HEAD')
    git_tree_dirty = bool(git_output('status', '--porcelain'))

    payload = {
        'contract_id': 'objc3c.release.foundation.manifest.v1',
        'schema_version': 1,
        'package_model': first['package_manifest']['package_model'],
        'reproducibility_scope': reproducibility_policy['reproducibility_scope'],
        'build_run_count': 2,
        'reproducibility_match': reproducibility_match,
        'source_surface': repo_rel(SOURCE_SURFACE),
        'package_runs': [
            {
                'run_id': 'run-1',
                'package_root': repo_rel(first['package_root']),
                'package_manifest_path': repo_rel(first['manifest_path']),
                'package_manifest_sha256': sha256_file(first['manifest_path']),
                'copied_file_count': first['package_manifest']['copied_file_count'],
                'release_payload_digest_sha256': first['payload_digest'],
            },
            {
                'run_id': 'run-2',
                'package_root': repo_rel(second['package_root']),
                'package_manifest_path': repo_rel(second['manifest_path']),
                'package_manifest_sha256': sha256_file(second['manifest_path']),
                'copied_file_count': second['package_manifest']['copied_file_count'],
                'release_payload_digest_sha256': second['payload_digest'],
            },
        ],
        'primary_package_root': repo_rel(first['package_root']),
        'primary_package_manifest_path': repo_rel(first['manifest_path']),
        'primary_package_manifest_sha256': sha256_file(first['manifest_path']),
        'repo_superclean_surface_path': repo_rel(repo_superclean_path),
        'repo_superclean_surface_sha256': sha256_file(repo_superclean_path),
        'release_evidence_index_path': repo_rel(EVIDENCE_INDEX_PATH),
        'release_evidence_index_sha256': sha256_file(EVIDENCE_INDEX_PATH),
        'release_payload_entries': first['entries'],
        'release_payload_digest_sha256': first['payload_digest'],
        'source_stamps': {
            'generated_at_utc': datetime.now(timezone.utc).isoformat().replace('+00:00', 'Z'),
            'git_commit': git_commit,
            'git_tree_dirty': git_tree_dirty,
        },
    }

    MANIFEST_PATH.parent.mkdir(parents=True, exist_ok=True)
    MANIFEST_PATH.write_text(json.dumps(payload, indent=2) + '\n', encoding='utf-8')

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        'contract_id': 'objc3c.release.foundation.manifest.summary.v1',
        'status': 'PASS',
        'source_surface': repo_rel(SOURCE_SURFACE),
        'release_manifest_path': repo_rel(MANIFEST_PATH),
        'reproducibility_match': reproducibility_match,
        'primary_package_root': payload['primary_package_root'],
        'primary_package_manifest_path': payload['primary_package_manifest_path'],
        'release_payload_file_count': len(first['entries']),
        'release_payload_digest_sha256': payload['release_payload_digest_sha256'],
    }
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + '\n', encoding='utf-8')
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print('objc3c-release-manifest: PASS')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
