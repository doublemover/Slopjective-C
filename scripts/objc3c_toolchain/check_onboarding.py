"""Check the current Linux reader routes against CLI and SDK source owners."""
from __future__ import annotations

from contextlib import redirect_stdout
import io
import json
from pathlib import Path
import re
from urllib.parse import unquote, urlsplit

from scripts.build_pages import load_contract, render_markdown_source
from . import cli

RETIRED = re.compile(
    r'\b(?:build-native-binaries|test-smoke|compile-objc3c|check-showcase-surface|'
    r'validate-showcase|inspect-compile-observability|materialize-project-template|'
    r'validate-getting-started|build-site|check-site|action_catalog\.py|'
    r'auroraBoard|signalMesh|patchKit|applicationFrameworkSamples)\b|'
    r'docs/support/(?:capability_matrix|evidence_map)|\b(?:async\s+)?fn\s+\w+'
)


def check(repo: Path) -> dict:
    paths = [repo / 'README.md', *sorted((repo / 'docs/tutorials').glob('*.md')),
             *sorted((repo / 'showcase').rglob('*')),
             *sorted((repo / 'site/src').glob('*')), repo / 'site/index.md']
    errors = []
    help_text = io.StringIO()
    with redirect_stdout(help_text):
        try:
            cli.main(['--help'])
        except SystemExit as exit:
            if exit.code: raise
    checked = []
    for path in paths:
        if not path.is_file():
            continue
        relative = path.relative_to(repo).as_posix()
        content = path.read_text()
        checked.append(relative)
        # The root README can describe the cutover; reader entrypoints must
        # never route back through the removed workflows.
        if relative != 'README.md' and RETIRED.search(content):
            errors.append(f'{relative}: retired compiler workflow')
        for command in re.findall(r'(?:npm run objc3c --|python -m scripts\.objc3c_toolchain)\s+([\w-]+)', content):
            if not re.search(r'\b' + re.escape(command) + r'\b', help_text.getvalue()):
                errors.append(f'{relative}: unknown repository command {command}')
        if path.suffix != '.md':
            continue
        base = repo / 'site' if relative == 'site/src/index.body.md' else path.parent
        for link in re.findall(r'\[[^\]]*\]\(([^\s)]+)(?:\s+[^)]*)?\)', content):
            target = urlsplit(link.strip('<>'))
            if target.scheme or target.netloc or not target.path:
                continue
            if not (base / unquote(target.path)).exists():
                errors.append(f'{relative}: broken navigation {link}')
    portfolio = json.loads((repo / 'showcase/portfolio.json').read_text())
    if portfolio.get('schema') != 'objc3c.linux-sdk-examples.v1':
        errors.append('showcase: wrong catalog schema')
    expected = {
        'native-application': ('tests/clang_foundation/installed_sdk/main.objc3', 'installed_qualification'),
        'managed-modules': ('tests/clang_foundation/native_package', 'installed_source_modules'),
        'package-graph': ('tests/clang_foundation/source_graph/workspace.json', 'installed_source_graph'),
    }
    actual = {}
    for example in portfolio.get('examples', []):
        actual[example['id']] = (example['source'], example['qualification'])
        for field in ('source', 'tutorial'):
            if not (repo / example[field]).exists():
                errors.append(f'showcase: missing {field} {example[field]}')
    if actual != expected or len(portfolio.get('examples', [])) != len(expected):
        errors.append('showcase: examples must use the installed SDK qualification owners')
    authority = 'schemas/objc3-language-capabilities.json'
    contract = json.loads((repo / 'site/src/index.contract.json').read_text())
    if any(owner.get('capability_authority') != authority for owner in (portfolio, contract)):
        errors.append('current capability authority must be the language schema')
    states = {f['name']: f['state'] for f in json.loads((repo / authority).read_text())['features']}
    for relative in ('docs/tutorials/getting_started.md', 'showcase/README.md', 'site/src/index.body.md'):
        text = (repo / relative).read_text()
        if authority not in text or not re.search(r'Public\s+`?async/await`? and actors remain reserved', text):
            errors.append(f'{relative}: missing capability authority or reserved concurrency boundary')
    if states['async_await'] != 'reserved' or states['actors'] != 'reserved':
        errors.append('onboarding concurrency claims need review against the language schema')
    config = load_contract(repo)
    if config.front_matter + render_markdown_source(config.body_path) != config.output_path.read_text():
        errors.append('site/index.md: generated output drift')
    if errors:
        raise ValueError('\n'.join(errors))
    return {'status': 'PASS', 'entrypoints': checked, 'examples': sorted(expected),
            'capability_authority': authority}


def main() -> int:
    print(json.dumps(check(Path(__file__).resolve().parents[2]), indent=2))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
