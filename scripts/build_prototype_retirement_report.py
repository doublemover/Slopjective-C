from __future__ import annotations

import json
import re
from pathlib import Path
from objc3c_tooling.json_io import write_text_file as write_text, write_json_file

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'workflow_simplification'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-prototype-retirement'
PACKAGE_JSON_PATH = ROOT / 'package.json'
PLAN_JSON_PATH = PLAN_DIR / 'prototype_retirement_report.json'
PLAN_MD_PATH = PLAN_DIR / 'prototype_retirement_report.md'
REPORT_JSON_PATH = REPORT_DIR / 'prototype_retirement_report.json'
REPORT_MD_PATH = REPORT_DIR / 'prototype_retirement_report.md'
NPM_RUN_PATTERN = re.compile(r'npm run ([A-Za-z0-9:_\-]+)')
DOC_ROOTS = [ROOT / 'README.md', ROOT / 'docs', ROOT / 'showcase', ROOT / 'stdlib']
TARGET_FILES = {
    'compiler_throughput_runbook': ROOT / 'docs' / 'runbooks' / 'objc3c_compiler_throughput.md',
    'external_validation_runbook': ROOT / 'docs' / 'runbooks' / 'objc3c_external_validation.md',
    'native_source_readme': ROOT / 'docs' / 'objc3c-native' / 'src' / 'README.md',
    'native_runtime_architecture_fragment': ROOT / 'docs' / 'objc3c-native' / 'src' / '35-runtime-architecture.md',
    'native_tests_fragment': ROOT / 'docs' / 'objc3c-native' / 'src' / '60-tests.md',
    'native_stitched_doc': ROOT / 'docs' / 'objc3c-native.md',
}
FORBIDDEN_SNIPPETS = {
    'docs/runbooks/objc3c_compiler_throughput.md': [
        'python scripts/render_objc3c_public_command_surface.py',
        'python scripts/build_objc3c_native_docs.py',
        'npm run objc3c -- benchmark-compiler-throughput',
    ],
    'docs/runbooks/objc3c_external_validation.md': [
        'python scripts/check_external_validation_source_surface.py',
        'python scripts/run_objc3c_external_validation_replay.py',
        'python scripts/publish_objc3c_external_repro_corpus.py',
    ],
    'docs/objc3c-native/src/35-runtime-architecture.md': [
        'python scripts/check_objc3c_dependency_boundaries.py --strict',
    ],
    'docs/objc3c-native/src/60-tests.md': [
        'python scripts/ci/check_task_hygiene.py',
        'python scripts/check_objc3c_dependency_boundaries.py --strict',
        'npm run objc3c -- test-smoke',
        'npm run objc3c -- test-recovery',
    ],
    'docs/objc3c-native.md': [
        'python scripts/ci/check_task_hygiene.py',
        'python scripts/check_objc3c_dependency_boundaries.py --strict',
        'npm run objc3c -- test-smoke',
        'npm run objc3c -- test-recovery',
    ],
}
REQUIRED_SNIPPETS = {
    'docs/runbooks/objc3c_compiler_throughput.md': [
        'npm run objc3c -- benchmark-compiler-throughput',
        'npm run objc3c -- build-native-docs',
        'npm run objc3c -- build-public-command-surface',
    ],
    'docs/runbooks/objc3c_external_validation.md': [
        'npm run objc3c -- check-external-validation-surface',
        'npm run objc3c -- test-external-validation-replay',
        'npm run objc3c -- publish-external-repro-corpus',
    ],
    'docs/objc3c-native/src/README.md': [
        'npm run objc3c -- build-native-docs',
        'npm run objc3c -- check-native-docs',
        'npm run objc3c -- build-site',
        'npm run objc3c -- check-site',
        'npm run objc3c -- build-public-command-surface',
        'npm run objc3c -- check-public-command-surface',
    ],
    'docs/objc3c-native/src/35-runtime-architecture.md': [
        'npm run objc3c -- check-dependency-boundaries',
    ],
    'docs/objc3c-native/src/60-tests.md': [
        'npm run objc3c -- test-smoke',
        'npm run objc3c -- test-recovery',
        'npm run objc3c -- test-execution-smoke',
        'npm run objc3c -- test-execution-replay',
        'npm run objc3c -- check-task-hygiene',
        'npm run objc3c -- check-dependency-boundaries',
    ],
    'docs/objc3c-native.md': [
        'npm run objc3c -- test-smoke',
        'npm run objc3c -- test-recovery',
        'npm run objc3c -- test-execution-smoke',
        'npm run objc3c -- test-execution-replay',
        'npm run objc3c -- check-task-hygiene',
        'npm run objc3c -- check-dependency-boundaries',
    ],
}



def iter_markdown_paths() -> list[Path]:
    paths: list[Path] = []
    for entry in DOC_ROOTS:
        if entry.is_file():
            paths.append(entry)
        elif entry.is_dir():
            paths.extend(sorted(entry.rglob('*.md')))
    return paths


def collect_invalid_npm_refs(package_bridge: str) -> list[dict[str, str]]:
    failures: list[dict[str, str]] = []
    for path in iter_markdown_paths():
        text = path.read_text(encoding='utf-8')
        for match in NPM_RUN_PATTERN.finditer(text):
            command = match.group(1)
            if command != package_bridge:
                failures.append({'path': path.relative_to(ROOT).as_posix(), 'command': command})
    return failures


def main() -> None:
    package_bridge = 'objc3c'
    package_scripts = json.loads(PACKAGE_JSON_PATH.read_text(encoding='utf-8'))['scripts']
    package_bridge_count = 1 if package_bridge in package_scripts else 0
    invalid_npm_refs = collect_invalid_npm_refs(package_bridge)
    forbidden_hits: dict[str, list[str]] = {}
    required_missing: dict[str, list[str]] = {}

    for relative_path, snippets in FORBIDDEN_SNIPPETS.items():
        text = (ROOT / relative_path).read_text(encoding='utf-8')
        hits = [snippet for snippet in snippets if snippet in text]
        if hits:
            forbidden_hits[relative_path] = hits

    for relative_path, snippets in REQUIRED_SNIPPETS.items():
        text = (ROOT / relative_path).read_text(encoding='utf-8')
        missing = [snippet for snippet in snippets if snippet not in text]
        if missing:
            required_missing[relative_path] = missing

    payload = {
        'issue': 'workflow-prototype-retirement',
        'package_bridge': package_bridge if package_bridge_count else '',
        'package_bridge_count': package_bridge_count,
        'invalid_npm_run_references': invalid_npm_refs,
        'forbidden_hits': forbidden_hits,
        'required_missing': required_missing,
        'checked_files': {key: path.relative_to(ROOT).as_posix() for key, path in TARGET_FILES.items()},
        'next_issue': 'workflow-closeout-gate',
        'status': 'PASS' if not invalid_npm_refs and not forbidden_hits and not required_missing else 'FAIL',
    }

    write_json_file(PLAN_JSON_PATH, payload)
    write_json_file(REPORT_JSON_PATH, payload)

    lines = [
        '# workflow-prototype-retirement Prototype Retirement Report',
        '',
        f"- status: `{payload['status']}`",
        f"- package_bridge_count: `{payload['package_bridge_count']}`",
        f"- package_bridge: `{payload['package_bridge']}`",
        f"- invalid_npm_run_references: `{len(invalid_npm_refs)}`",
        f"- forbidden_hits: `{sum(len(v) for v in forbidden_hits.values())}`",
        f"- required_missing: `{sum(len(v) for v in required_missing.values())}`",
        '',
        '## Checked files',
    ]
    for key, value in payload['checked_files'].items():
        lines.append(f'- `{key}`: `{value}`')
    lines.extend(['', '## Invalid npm run references'])
    if invalid_npm_refs:
        for entry in invalid_npm_refs:
            lines.append(f"- `{entry['path']}` -> `{entry['command']}`")
    else:
        lines.append('- none')
    lines.extend(['', '## Forbidden hits'])
    if forbidden_hits:
        for relative_path, hits in forbidden_hits.items():
            lines.append(f'- `{relative_path}`')
            for hit in hits:
                lines.append(f'  - `{hit}`')
    else:
        lines.append('- none')
    lines.extend(['', '## Required snippets missing'])
    if required_missing:
        for relative_path, misses in required_missing.items():
            lines.append(f'- `{relative_path}`')
            for miss in misses:
                lines.append(f'  - `{miss}`')
    else:
        lines.append('- none')
    lines.extend(['', 'Next issue: `workflow-closeout-gate`', ''])
    markdown = '\n'.join(lines)
    write_text(PLAN_MD_PATH, markdown)
    write_text(REPORT_MD_PATH, markdown)

    if payload['status'] != 'PASS':
        raise SystemExit(1)


if __name__ == '__main__':
    main()
