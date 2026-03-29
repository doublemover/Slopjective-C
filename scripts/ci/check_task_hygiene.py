#!/usr/bin/env python3
from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PACKAGE_JSON = ROOT / "package.json"
PACKAGE_SCRIPT_BUDGET = 67
REMOVED_FAMILY_PATTERNS = (
    r"^check:objc3c:m",
    r"^test:tooling:m",
    r"^check:compiler-closeout:m",
    r"^run:objc3c:",
    r"^plan:compiler-dispatch:",
    r"^refresh:compiler-dispatch:",
    r"^dev:objc3c:",
)
LIVE_SCAN_ROOTS = [
    ROOT / ".github",
    ROOT / "docs",
    ROOT / "showcase",
    ROOT / "scripts",
    ROOT / "tests",
    ROOT / "native",
    ROOT / "README.md",
    ROOT / "CONTRIBUTING.md",
    ROOT / "package.json",
]
LEGACY_ALIAS_RE = re.compile(r"npm run (check:objc3c:m|test:tooling:m|check:compiler-closeout:m|run:objc3c:|plan:compiler-dispatch:|refresh:compiler-dispatch:|dev:objc3c:)")
MILESTONE_WORKFLOW_RE = re.compile(r"^m\d+.*\.yml$")


def iter_live_files():
    for root in LIVE_SCAN_ROOTS:
        if not root.exists():
            continue
        if root.is_file():
            yield root
            continue
        for path in root.rglob('*'):
            if path.is_file() and 'tmp' not in path.parts and 'node_modules' not in path.parts:
                yield path


def main() -> int:
    errors: list[str] = []
    payload = json.loads(PACKAGE_JSON.read_text(encoding='utf-8'))
    scripts = payload.get('scripts', {})
    if not isinstance(scripts, dict):
        errors.append('package.json scripts field must be an object')
        scripts = {}

    removed_hits = sorted(name for name in scripts if any(re.match(p, name) for p in REMOVED_FAMILY_PATTERNS))
    if removed_hits:
        errors.append(f'removed script families still present: {removed_hits}')
    if len(scripts) > PACKAGE_SCRIPT_BUDGET:
        errors.append(
            f'package script count exceeds budget: {len(scripts)} > {PACKAGE_SCRIPT_BUDGET}'
        )
    if (ROOT / 'docs' / 'contracts').exists():
        errors.append('docs/contracts must not be live')
    if (ROOT / 'spec' / 'planning').exists():
        errors.append('spec/planning must not be live')
    if (ROOT / 'compiler').exists():
        errors.append('compiler prototype tree must not be live')
    if any((ROOT / '.github' / 'workflows').glob('m*.yml')):
        errors.append('milestone-named workflows must not be live')
    if any((ROOT / 'scripts').glob('check_m*.py')):
        errors.append('milestone checkers must not be live')
    if list((ROOT / 'tests' / 'tooling').glob('test_check_*.py')):
        errors.append('checker-wrapper pytest files must not be live')
    if any((ROOT / 'scripts').rglob('run_*_lane_*_readiness.py')):
        errors.append('readiness runners must not be live')
    live_pycache_dirs = [path for path in ROOT.rglob('__pycache__') if 'tmp' not in path.parts]
    if live_pycache_dirs:
        errors.append(f'__pycache__ directories must not be live: {[path.relative_to(ROOT).as_posix() for path in live_pycache_dirs[:5]]}')
    live_pyc_files = [path for path in ROOT.rglob('*.pyc') if 'tmp' not in path.parts]
    if live_pyc_files:
        errors.append(f'.pyc files must not be live: {[path.relative_to(ROOT).as_posix() for path in live_pyc_files[:5]]}')
    for path in iter_live_files():
        try:
            text = path.read_text(encoding='utf-8', errors='ignore')
        except Exception:
            continue
        if LEGACY_ALIAS_RE.search(text):
            errors.append(f'legacy npm alias reference still live: {path.relative_to(ROOT).as_posix()}')
            break
    ll_stubs = [
        ROOT / 'tests/tooling/fixtures/native/library_cli_parity/cli/module.ll',
        ROOT / 'tests/tooling/fixtures/native/library_cli_parity/library/module.ll',
    ]
    for stub in ll_stubs:
        if stub.exists():
            errors.append(f'stub ll fixture still live: {stub.relative_to(ROOT).as_posix()}')
    if errors:
        print('task-hygiene contract check failed:')
        for error in errors:
            print(f'- {error}')
        return 1
    print('task-hygiene contract check passed.')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
