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


def test_output_cannot_modify_source_workspace(workspace, tmp_path):
    sdk = tmp_path / 'sdk'
    sdk.mkdir()
    with pytest.raises(ValueError, match='separate'):
        build_graph(sdk, workspace, workspace.parent / 'build')
