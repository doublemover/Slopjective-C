from __future__ import annotations

import json
from pathlib import Path
import shutil

import pytest

from scripts.objc3c_toolchain.source_graph import build_graph, load_graph, source_inputs

FIXTURE = Path(__file__).resolve().parents[1] / 'clang_foundation/source_graph'


@pytest.fixture
def workspace(tmp_path):
    root = tmp_path / "sources with spaces 'and quotes"
    shutil.copytree(FIXTURE, root)
    return root / 'workspace.json'


def edit(path, action):
    data = json.loads(path.read_text())
    action(data)
    path.write_text(json.dumps(data))


def test_resolves_locked_diamond_once_in_dependency_order(workspace):
    graph = load_graph(workspace)
    assert graph['order'] == ['fixture.Leaf', 'fixture.Left', 'fixture.Right', 'fixture.Top']
    assert graph['packages']['fixture.Left']['dependencies'][0]['visibility'] == 'public'
    assert graph['packages']['fixture.Right']['dependencies'][0]['visibility'] == 'private'


@pytest.mark.parametrize('case,match', [
    ('unknown', 'identity lock'), ('identity', 'identity lock'), ('ownership', 'ownership lock'),
    ('cycle', 'cyclic'), ('duplicate', 'duplicate'), ('unknown-field', 'expected fields'),
    ('visibility', 'dependency lock'), ('app-overlap', 'source roots'),
    ('root-overlap', 'source roots'), ('escape', 'inside'),
])
def test_graph_rejects_invalid_locks_and_source_boundaries(workspace, tmp_path, case, match):
    top = workspace.parent / 'top/package.json'
    if case == 'unknown':
        edit(top, lambda data: data['dependencies'][0].update(name='fixture.Absent'))
    elif case == 'identity':
        edit(top, lambda data: data['dependencies'][0].update(package='wrong'))
    elif case == 'ownership':
        edit(top, lambda data: data.update(ownership='mrc'))
    elif case == 'cycle':
        edit(workspace.parent / 'leaf/package.json', lambda data: data['dependencies'].append(
            {'name':'fixture.Top', 'package':'top-v1', 'visibility':'private'}))
    elif case == 'duplicate':
        edit(workspace, lambda data: data['packages'].append('top/package.json'))
    elif case == 'unknown-field':
        edit(top, lambda data: data.update(fetch='https://example.invalid'))
    elif case == 'visibility':
        edit(top, lambda data: data['dependencies'][0].update(visibility='automatic'))
    elif case == 'app-overlap':
        edit(workspace, lambda data: data['application'].update(root='top'))
    elif case == 'root-overlap':
        nested = top.parent / 'nested'
        nested.mkdir()
        shutil.copyfile(top, nested / 'package.json')
        shutil.copyfile(top.parent / 'module.m', nested / 'module.m')
        edit(nested / 'package.json', lambda data: data.update(name='fixture.Nested'))
        edit(workspace, lambda data: data['packages'].append('top/nested/package.json'))
    elif case == 'escape':
        external = tmp_path / 'external.m'
        external.write_text('int escaped;')
        (top.parent / 'module.m').unlink()
        (top.parent / 'module.m').symlink_to(external)
    with pytest.raises(ValueError, match=match):
        load_graph(workspace)


def test_only_application_closure_is_built(workspace):
    edit(workspace, lambda data: data['application']['dependencies'].__setitem__(0,
         {'name':'fixture.Leaf', 'package':'leaf-v1', 'visibility':'private'}))
    assert load_graph(workspace)['order'] == ['fixture.Leaf']


def test_declaration_order_does_not_change_dependency_order(workspace):
    expected = load_graph(workspace)['order']
    edit(workspace, lambda data: data['packages'].reverse())
    assert load_graph(workspace)['order'] == expected


def test_unused_package_lock_is_still_validated(workspace):
    edit(workspace, lambda data: data['application']['dependencies'].__setitem__(0,
         {'name':'fixture.Leaf', 'package':'leaf-v1', 'visibility':'private'}))
    edit(workspace.parent / 'top/package.json', lambda data: data['dependencies'][0].update(package='wrong'))
    with pytest.raises(ValueError, match='identity lock'):
        load_graph(workspace)


def test_package_sources_keep_the_qualified_objective_c_boundary(workspace):
    root = workspace.parent / 'leaf'
    (root / 'other.c').write_text('int value;')
    edit(root / 'package.json', lambda data: data.update(source='other.c'))
    with pytest.raises(ValueError, match='Objective-C'):
        load_graph(workspace)


def test_source_content_and_membership_invalidate_preserved_timestamps(workspace):
    import os
    root = workspace.parent / 'leaf'
    path = root / 'module.m'
    original = source_inputs(root)
    stamp = path.stat()
    path.write_text(path.read_text() + '\n// changed\n')
    os.utime(path, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
    assert source_inputs(root) != original
    (root / 'shadow.h').write_text('')
    assert str(root / 'shadow.h') in source_inputs(root)


def test_source_directory_links_are_rejected(workspace):
    root = workspace.parent / 'leaf'
    (root / 'alias').symlink_to(root, target_is_directory=True)
    with pytest.raises(ValueError, match='directory links'):
        source_inputs(root)


def test_output_cannot_modify_a_source_root(workspace, tmp_path):
    sdk = tmp_path / 'sdk'
    sdk.mkdir()
    with pytest.raises(ValueError, match='separate'):
        build_graph(sdk, workspace, workspace.parent / 'leaf/build')


def test_output_also_protects_unselected_source_roots(workspace, tmp_path):
    sdk = tmp_path / 'sdk'
    sdk.mkdir()
    edit(workspace, lambda data: data['application']['dependencies'].__setitem__(0,
         {'name':'fixture.Leaf', 'package':'leaf-v1', 'visibility':'private'}))
    with pytest.raises(ValueError, match='separate'):
        build_graph(sdk, workspace, workspace.parent / 'right/build')


def test_standalone_builder_does_not_write_bytecode_into_sdk(tmp_path):
    import os
    import subprocess
    import sys
    helper_root = Path(__file__).resolve().parents[2] / 'scripts/objc3c_toolchain'
    for source, destination in [('source_graph.py', 'build.py'), ('sdk_example.py', 'sdk_example.py')]:
        shutil.copyfile(helper_root / source, tmp_path / destination)
    environment = dict(os.environ)
    environment.pop('PYTHONDONTWRITEBYTECODE', None)
    result = subprocess.run([sys.executable, str(tmp_path / 'build.py'), '--help'],
                            env=environment, capture_output=True, text=True, timeout=30)
    assert result.returncode == 0, result.stderr
    assert not (tmp_path / '__pycache__').exists()


def set_mode(workspace, language, standard, ownership):
    suffix = {'objective-c': '.m', 'objective-c++': '.mm', 'c++': '.cpp'}[language]
    for manifest in workspace.parent.glob('*/package.json'):
        data = json.loads(manifest.read_text())
        old = manifest.parent / data['source']
        new = old.with_suffix(suffix)
        if old != new:
            old.rename(new)
        data.update(language=language, standard=standard, ownership=ownership, provider_ownership=ownership, source=new.name)
        manifest.write_text(json.dumps(data))
    data = json.loads(workspace.read_text())
    old = workspace.parent / data['application']['root'] / data['application']['source']
    new = old.with_suffix(suffix)
    if old != new:
        old.rename(new)
    data['application'].update(language=language, standard=standard, ownership=ownership, source=new.name)
    workspace.write_text(json.dumps(data))


@pytest.mark.parametrize('language,standard,ownership', [
    ('objective-c', 'c17', 'arc'), ('objective-c', 'c17', 'mrc'),
    ('objective-c++', 'c++20', 'arc'), ('objective-c++', 'c++20', 'mrc'),
    ('objective-c++', 'c++23', 'arc'), ('objective-c++', 'c++23', 'mrc'),
    ('c++', 'c++20', 'none'), ('c++', 'c++23', 'none'),
])
def test_explicit_language_and_standard_modes_resolve_the_diamond(workspace, language, standard, ownership):
    set_mode(workspace, language, standard, ownership)
    graph = load_graph(workspace)
    assert len(graph['order']) == 4
    assert all(graph['packages'][name]['language'] == language and
               graph['packages'][name]['standard'] == standard for name in graph['order'])
    assert graph['application']['ownership'] == ownership


@pytest.mark.parametrize('case,match', [
    ('old-workspace', 'schema'), ('old-package', 'identity'), ('missing-mode', 'expected fields'),
    ('unknown-language', 'source language'), ('malformed-language', 'source language'),
    ('old-standard', 'standard'), ('cpp-arc', 'ownership'),
    ('language-edge', 'language lock'), ('standard-edge', 'standard lock'),
])
def test_mode_cutover_and_incompatible_edges_fail_before_compilation(workspace, case, match):
    leaf = workspace.parent / 'leaf/package.json'
    if case == 'old-workspace':
        edit(workspace, lambda data: data.update(schema='objc3c.source-workspace.v2'))
    elif case == 'old-package':
        edit(leaf, lambda data: data.update(schema='objc3c.source-package.v2'))
    elif case == 'missing-mode':
        edit(leaf, lambda data: data.pop('language'))
    elif case == 'unknown-language':
        edit(leaf, lambda data: data.update(language='automatic'))
    elif case == 'malformed-language':
        edit(leaf, lambda data: data.update(language=[]))
    elif case == 'old-standard':
        set_mode(workspace, 'c++', 'c++17', 'none')
    elif case == 'cpp-arc':
        set_mode(workspace, 'c++', 'c++20', 'arc')
    elif case == 'language-edge':
        source = leaf.parent / 'module.m'
        source.rename(source.with_suffix('.mm'))
        edit(leaf, lambda data: data.update(source='module.mm', language='objective-c++', standard='c++20'))
    elif case == 'standard-edge':
        set_mode(workspace, 'objective-c++', 'c++20', 'arc')
        edit(leaf, lambda data: data.update(standard='c++23'))
    with pytest.raises(ValueError, match=match):
        load_graph(workspace)


def test_parallel_graph_failure_replaces_a_stale_receipt_and_joins_cases(tmp_path, monkeypatch):
    from scripts.objc3c_toolchain import qualify_source_graph

    sdk = tmp_path / 'sdk'
    sdk.mkdir()
    output = tmp_path / 'evidence'
    output.mkdir()
    (output / 'qualification.json').write_text('{"status":"PASS"}')
    completed = []

    def case(sdk, output, language, standard, ownership, optimization):
        completed.append((language, standard, ownership, optimization))
        if language == 'objective-c' and ownership == 'arc' and optimization == 'O0':
            raise ValueError('real compiler gate failed; see compile.log')
        return [], []

    monkeypatch.setattr(qualify_source_graph, 'qualify_case', case)
    with pytest.raises(ValueError, match='qualification failed'):
        qualify_source_graph.qualify(sdk, output, jobs=2)
    report = json.loads((output / 'qualification.json').read_text())
    assert report['status'] == 'FAIL' and len(report['failures']) == 1
    assert len(completed) == 16
    assert 'compile.log' in report['failures'][0]['error']


def test_explicit_projection_keeps_consumer_mode_and_records_provider_mode(workspace):
    edit(workspace.parent / 'leaf/package.json', lambda d: d.update(provider_ownership='mrc'))
    graph = load_graph(workspace)
    leaf = graph['packages']['fixture.Leaf']
    assert leaf['ownership'] == 'arc' and leaf['provider_ownership'] == 'mrc'
    assert graph['application']['ownership'] == 'arc'


@pytest.mark.parametrize('provider', ['automatic', None, True, 'none'])
def test_provider_ownership_requires_a_supported_explicit_mode(workspace, provider):
    edit(workspace.parent / 'leaf/package.json', lambda d: d.update(provider_ownership=provider))
    with pytest.raises(ValueError, match='provider ownership'):
        load_graph(workspace)


def test_projected_provider_rejects_a_dependency_closure(workspace):
    edit(workspace.parent / 'top/package.json', lambda d: d.update(provider_ownership='mrc'))
    with pytest.raises(ValueError, match='closed provider'):
        load_graph(workspace)
