"""Patch identities must survive Git index changes in a development checkout."""
from contextlib import redirect_stdout
from io import StringIO
import json
from pathlib import Path
import subprocess
from unittest.mock import patch

from scripts.objc3c_toolchain import export_development_patches as exporter


def test_export_identity_survives_intent_to_add_and_new_files(tmp_path):
    source = tmp_path / 'source'
    source.mkdir()
    def git(*args):
        return subprocess.check_output(['git', '-C', str(source), *args])
    git('init', '-q')
    git('config', 'user.name', 'Patch export test')
    git('config', 'user.email', 'patch-export@example.invalid')
    (source / 'tracked').write_text('upstream\n')
    git('add', 'tracked')
    git('commit', '-qm', 'upstream')
    (source / 'tracked').write_text('modified\n')
    (source / 'new').mkdir()
    (source / 'new/a').write_text('first')
    (source / 'new/b').write_text('second\n')
    (source / 'new/b').chmod(0o755)
    (source / 'new/empty').touch()
    table = source / 'clang/include/clang/Basic/ObjC3Features.def'
    table.parent.mkdir(parents=True)
    table.write_text('features\n')
    manifest = tmp_path / 'toolchains/llvm/manifest.json'
    manifest.parent.mkdir(parents=True)
    manifest.write_text(json.dumps({'upstream': {'commit': git('rev-parse', 'HEAD').decode().strip()}}))
    authority = tmp_path / 'authority.json'
    authority.write_text('{}')
    stack = tmp_path / 'patches'
    stack.mkdir()
    packet = stack / 'test.patch'
    with patch.multiple(exporter, ROOT=tmp_path, SOURCE=source, STACK=stack,
                        AUTHORITY=authority,
                        PACKETS={'test.patch': ['tracked', 'new', str(table.relative_to(source))]}), \
         patch.object(exporter, 'generated', return_value=('features\n', None)), \
         redirect_stdout(StringIO()):
        exporter.export()
        original = packet.read_bytes()
        original_manifest = manifest.read_bytes()
        git('add', '--intent-to-add', '.')
        exporter.export()
        assert packet.read_bytes() == original
        assert manifest.read_bytes() == original_manifest
        git('reset', '-q')
        exporter.export()
        assert packet.read_bytes() == original
        # A newly owned source extends the packet without moving old sections.
        (source / 'new/0').write_text('third\n')
        exporter.export()
        extended = packet.read_bytes()
        assert extended.startswith(original)
        git('add', '--intent-to-add', '.')
        exporter.export()
        assert packet.read_bytes() == extended
        # Also preserve old packets which omitted optional new-file metadata.
        packet.write_bytes(b''.join(line for line in extended.splitlines(keepends=True)
                                   if not line.startswith(b'index 000000000..')))
        without_index = packet.read_bytes()
        exporter.export()
        assert packet.read_bytes() == without_index
        (source / 'tracked').write_text('completed change\n')
        (source / 'new/a').write_text('unfinished work\n')
        (source / 'not-yet-owned').write_text('in progress\n')
        exporter.export(['tracked'])
        selective = packet.read_bytes()
        assert b'+completed change\n' in selective
        assert b'+unfinished work' not in selective
        assert b'+first\n' in selective
        assert b'not-yet-owned' not in selective
        (source / 'tracked').write_text('upstream\n')
        exporter.export(['tracked'])
        assert b'diff --git a/tracked b/tracked' not in packet.read_bytes()
        assert b'+first\n' in packet.read_bytes()

