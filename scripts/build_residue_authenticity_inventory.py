from __future__ import annotations

import json
import re
import subprocess
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'source_hygiene'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm315' / 'M315-A001'
PLAN_JSON_PATH = PLAN_DIR / 'residue_authenticity_inventory.json'
PLAN_MD_PATH = PLAN_DIR / 'residue_authenticity_inventory.md'
REPORT_JSON_PATH = REPORT_DIR / 'residue_authenticity_inventory.json'
REPORT_MD_PATH = REPORT_DIR / 'residue_authenticity_inventory.md'
RESIDUE_PATTERN = re.compile(r'\bM\d{3}\b|\[M\d{3}\]')
PRODUCT_PREFIXES = (
    'README.md',
    'CONTRIBUTING.md',
    'site/src/',
    'site/index.md',
    'docs/tutorials/',
    'docs/runbooks/',
    'docs/objc3c-native/src/',
    'docs/objc3c-native.md',
    'showcase/',
    'stdlib/',
    'schemas/',
    'scripts/',
    'native/objc3c/src/',
)
GENERATED_TRUTH_FILES = {
    'site/index.md',
    'docs/objc3c-native.md',
    'docs/runbooks/objc3c_public_command_surface.md',
}
ARCHIVE_PREFIXES = ('docs/reference/',)


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def git_ls_files() -> list[str]:
    return subprocess.check_output(['git', 'ls-files'], cwd=ROOT, text=True, encoding='utf-8').splitlines()


def read_text_if_possible(path: Path) -> str | None:
    try:
        return path.read_text(encoding='utf-8')
    except Exception:
        return None


def top_bucket(path: str) -> str:
    if path in {'README.md', 'CONTRIBUTING.md'}:
        return path
    if '/' not in path:
        return path
    return path.split('/', 1)[0]


def main() -> None:
    files = git_ls_files()
    tracked_file_count = len(files)
    product_files = [path for path in files if path.startswith(PRODUCT_PREFIXES)]
    archive_files = [path for path in files if path.startswith(ARCHIVE_PREFIXES)]
    generated_truth_files = [path for path in files if path in GENERATED_TRUTH_FILES]

    product_residue_hits: list[dict[str, object]] = []
    generated_truth_residue_hits: list[dict[str, object]] = []
    for relative_path in product_files:
        text = read_text_if_possible(ROOT / relative_path)
        if text is None:
            continue
        matches = sorted(set(RESIDUE_PATTERN.findall(text)))
        if not matches:
            continue
        entry = {
            'path': relative_path,
            'matches': matches,
            'bucket': top_bucket(relative_path),
        }
        product_residue_hits.append(entry)
        if relative_path in GENERATED_TRUTH_FILES:
            generated_truth_residue_hits.append(entry)

    ll_files = [path for path in files if path.endswith('.ll')]
    ll_bucket_counts = Counter(top_bucket(path) for path in ll_files)
    ll_root_counts = Counter('/'.join(path.split('/')[:4]) for path in ll_files)

    json_files = [path for path in files if path.endswith('.json')]
    json_bucket_counts = Counter(top_bucket(path) for path in json_files)

    authenticity_classes = {
        'generated_truth': {
            'description': 'checked-in generated outputs that are canonical user or operator surfaces',
            'count': len(generated_truth_files),
            'sample': sorted(generated_truth_files),
        },
        'synthetic_or_replay_ll': {
            'description': 'tracked llvm ir artifacts checked into test fixtures or replay contracts',
            'count': len(ll_files),
            'sample': ll_files[:20],
            'root_counts': dict(sorted(ll_root_counts.items())),
        },
        'tracked_test_and_conformance_json': {
            'description': 'tracked json fixtures, corpora, and validation contracts rooted under tests/',
            'count': sum(1 for path in json_files if path.startswith('tests/')),
            'sample': [path for path in json_files if path.startswith('tests/')][:20],
        },
        'archive_or_planning': {
            'description': 'tracked draft, planning, publish, or redirect material that is explicitly non-product',
            'count': len(archive_files),
            'sample': archive_files[:20],
        },
    }

    residue_bucket_counts = Counter(entry['bucket'] for entry in product_residue_hits)
    payload = {
        'issue': 'M315-A001',
        'tracked_file_count': tracked_file_count,
        'product_file_count': len(product_files),
        'archive_file_count': len(archive_files),
        'generated_truth_file_count': len(generated_truth_files),
        'product_residue_hit_count': len(product_residue_hits),
        'generated_truth_residue_hit_count': len(generated_truth_residue_hits),
        'residue_pattern': RESIDUE_PATTERN.pattern,
        'residue_bucket_counts': dict(sorted(residue_bucket_counts.items())),
        'product_residue_hits': product_residue_hits,
        'generated_truth_residue_hits': generated_truth_residue_hits,
        'll_file_count': len(ll_files),
        'll_bucket_counts': dict(sorted(ll_bucket_counts.items())),
        'json_file_count': len(json_files),
        'json_bucket_counts': dict(sorted(json_bucket_counts.items())),
        'authenticity_classes': authenticity_classes,
        'follow_on_priorities': [
            'remove milestone-coded residue from generated truth surfaces first',
            'replace milestone-coded identifiers in live source comments and contract strings with stable feature-surface identifiers',
            'separate genuine generated outputs from synthetic fixtures and archive-only material in machine-readable provenance',
        ],
        'next_issue': 'M315-B001',
    }

    write_text(PLAN_JSON_PATH, json.dumps(payload, indent=2) + '\n')
    write_text(REPORT_JSON_PATH, json.dumps(payload, indent=2) + '\n')

    lines = [
        '# M315-A001 Residue Authenticity Inventory',
        '',
        f"- tracked_file_count: `{payload['tracked_file_count']}`",
        f"- product_file_count: `{payload['product_file_count']}`",
        f"- archive_file_count: `{payload['archive_file_count']}`",
        f"- generated_truth_file_count: `{payload['generated_truth_file_count']}`",
        f"- product_residue_hit_count: `{payload['product_residue_hit_count']}`",
        f"- generated_truth_residue_hit_count: `{payload['generated_truth_residue_hit_count']}`",
        f"- ll_file_count: `{payload['ll_file_count']}`",
        f"- json_file_count: `{payload['json_file_count']}`",
        '',
        '## Residue bucket counts',
    ]
    for bucket, count in payload['residue_bucket_counts'].items():
        lines.append(f'- `{bucket}`: `{count}`')
    lines.extend(['', '## Generated truth residue hits'])
    if generated_truth_residue_hits:
        for entry in generated_truth_residue_hits:
            lines.append(f"- `{entry['path']}` -> `{', '.join(entry['matches'])}`")
    else:
        lines.append('- none')
    lines.extend(['', '## Authenticity classes'])
    for name, info in authenticity_classes.items():
        lines.append(f"- `{name}`: `{info['count']}`")
        lines.append(f"  - {info['description']}")
    lines.extend(['', '## Follow-on priorities'])
    for item in payload['follow_on_priorities']:
        lines.append(f'- {item}')
    lines.extend(['', 'Next issue: `M315-B001`', ''])
    markdown = '\n'.join(lines)
    write_text(PLAN_MD_PATH, markdown)
    write_text(REPORT_MD_PATH, markdown)


if __name__ == '__main__':
    main()
