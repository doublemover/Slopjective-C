from __future__ import annotations

import json
import re
import shutil
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'workflow_simplification'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-prototype-retirement'
PACKAGE_JSON_PATH = ROOT / 'package.json'
NPM = shutil.which('npm.cmd') or shutil.which('npm') or 'npm'
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
        'python scripts/objc3c_public_workflow_runner.py benchmark-compiler-throughput',
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
        'python scripts/objc3c_public_workflow_runner.py test-fast',
        'python scripts/objc3c_public_workflow_runner.py test-recovery',
    ],
    'docs/objc3c-native.md': [
        'python scripts/ci/check_task_hygiene.py',
        'python scripts/check_objc3c_dependency_boundaries.py --strict',
        'python scripts/objc3c_public_workflow_runner.py test-fast',
        'python scripts/objc3c_public_workflow_runner.py test-recovery',
    ],
}
REQUIRED_SNIPPETS = {
    'docs/runbooks/objc3c_compiler_throughput.md': [
        'npm run inspect:objc3c:compiler-throughput',
        'npm run build:docs:native',
        'npm run build:docs:commands',
    ],
    'docs/runbooks/objc3c_external_validation.md': [
        'npm run check:external-validation:surface',
        'npm run test:objc3c:external-validation:replay',
        'npm run publish:objc3c:external-repro-corpus',
    ],
    'docs/objc3c-native/src/README.md': [
        'npm run build:docs:native',
        'npm run check:docs:native',
        'npm run build:site',
        'npm run check:site',
        'npm run build:docs:commands',
        'npm run check:docs:commands',
    ],
    'docs/objc3c-native/src/35-runtime-architecture.md': [
        'npm run check:objc3c:boundaries',
    ],
    'docs/objc3c-native/src/60-tests.md': [
        'npm run test:fast',
        'npm run test:objc3c',
        'npm run test:objc3c:execution-smoke',
        'npm run test:objc3c:execution-replay-proof',
        'npm run check:task-hygiene',
        'npm run check:objc3c:boundaries',
    ],
    'docs/objc3c-native.md': [
        'npm run test:fast',
        'npm run test:objc3c',
        'npm run test:objc3c:execution-smoke',
        'npm run test:objc3c:execution-replay-proof',
        'npm run check:task-hygiene',
        'npm run check:objc3c:boundaries',
    ],
}


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def iter_markdown_paths() -> list[Path]:
    paths: list[Path] = []
    for entry in DOC_ROOTS:
        if entry.is_file():
            paths.append(entry)
        elif entry.is_dir():
            paths.extend(sorted(entry.rglob('*.md')))
    return paths


def collect_invalid_npm_refs(package_scripts: set[str]) -> list[dict[str, str]]:
    failures: list[dict[str, str]] = []
    for path in iter_markdown_paths():
        text = path.read_text(encoding='utf-8')
        for match in NPM_RUN_PATTERN.finditer(text):
            command = match.group(1)
            if command not in package_scripts:
                failures.append({'path': path.relative_to(ROOT).as_posix(), 'command': command})
    return failures


def main() -> None:
    subprocess.run([NPM, 'run', 'build:docs:native'], cwd=ROOT, check=True)
    subprocess.run([NPM, 'run', 'check:docs:native'], cwd=ROOT, check=True)

    package_scripts = set(json.loads(PACKAGE_JSON_PATH.read_text(encoding='utf-8'))['scripts'])
    invalid_npm_refs = collect_invalid_npm_refs(package_scripts)
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
        'package_script_count': len(package_scripts),
        'invalid_npm_run_references': invalid_npm_refs,
        'forbidden_hits': forbidden_hits,
        'required_missing': required_missing,
        'checked_files': {key: path.relative_to(ROOT).as_posix() for key, path in TARGET_FILES.items()},
        'next_issue': 'workflow-closeout-gate',
        'status': 'PASS' if not invalid_npm_refs and not forbidden_hits and not required_missing else 'FAIL',
    }

    write_text(PLAN_JSON_PATH, json.dumps(payload, indent=2) + '\n')
    write_text(REPORT_JSON_PATH, json.dumps(payload, indent=2) + '\n')

    lines = [
        '# workflow-prototype-retirement Prototype Retirement Report',
        '',
        f"- status: `{payload['status']}`",
        f"- package_script_count: `{payload['package_script_count']}`",
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
