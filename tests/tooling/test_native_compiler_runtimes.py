"""Compiler-rt delivery rejects substituted archives and foreign native objects."""
from pathlib import Path
import json
import shutil
import subprocess

import pytest

from scripts.objc3c_toolchain import compiler_runtimes as owner, linux_host


def elf(machine=62):
    header=bytearray(64);header[:6]=b'\x7fELF\x02\x01'
    header[16:18]=(1).to_bytes(2,'little');header[18:20]=machine.to_bytes(2,'little')
    return bytes(header)


def archive(path, objects):
    data=bytearray(b'!<arch>\n')
    for name, body in objects:
        data.extend(f'{name:<16}{0:<12}{0:<6}{0:<6}{100644:<8}{len(body):<10}`\n'.encode())
        data.extend(body)
        if len(body)%2:data.extend(b'\n')
    path.write_bytes(data)


@pytest.mark.parametrize('machine',[62,183])
def test_all_members_must_have_the_selected_native_elf_architecture(tmp_path,machine):
    path=tmp_path/'builtins.a'
    archive(path,[('first.o/',elf(machine)),('second.o/',elf(machine))])
    assert owner.archive_members(path,machine)==['first.o','second.o']
    archive(path,[('first.o/',elf(machine)),('second.o/',elf(183 if machine==62 else 62))])
    with pytest.raises(ValueError,match='foreign'):owner.archive_members(path,machine)


@pytest.mark.parametrize('mutation',['thin','bitcode','executable','32bit','big-endian','truncated-header','truncated-member','padding','empty'])
def test_archive_admission_rejects_unowned_or_incomplete_objects(tmp_path,mutation):
    path=tmp_path/'builtins.a';body=bytearray(elf())
    if mutation=='bitcode':body[:4]=b'BC\xc0\xde'
    elif mutation=='executable':body[16]=2
    elif mutation=='32bit':body[4]=1
    elif mutation=='big-endian':body[5]=2
    archive(path,[('native.o/',bytes(body)+b'X')])
    data=path.read_bytes()
    if mutation=='thin':data=data.replace(b'!<arch>',b'!<thin>',1)
    elif mutation=='truncated-header':data=data[:25]
    elif mutation=='truncated-member':data=data[:-2]
    elif mutation=='padding':data=data[:-1]+b'!'
    elif mutation=='empty':data=b'!<arch>\n'
    path.write_bytes(data)
    with pytest.raises(ValueError):owner.archive_members(path,62)


def test_gnu_and_bsd_archive_names_are_admitted_without_external_members(tmp_path):
    path=tmp_path/'builtins.a';long_name='native_long_function_object.c.o'
    archive(path,[('//',long_name.encode()+b'/\n'),('/0',elf()),('#1/12',b'bsd_object.o'+elf())])
    assert owner.archive_members(path,62)==[long_name,'bsd_object.o']
    archive(path,[('../escape.o/',elf())])
    with pytest.raises(ValueError,match='escaped'):owner.archive_members(path,62)


def test_actual_native_compiler_archive_can_be_admitted(tmp_path):
    cc,ar=shutil.which('cc'),shutil.which('ar')
    if not cc or not ar:pytest.skip('requires the native distribution C compiler and archiver')
    source=tmp_path/'source.c';source.write_text('int native_builtin_control(int value){return value+1;}\n')
    obj=tmp_path/'native.o';path=tmp_path/'builtins.a'
    subprocess.run([cc,'-c',source,'-o',obj],check=True,capture_output=True,timeout=30)
    subprocess.run([ar,'rc',path,obj],check=True,capture_output=True,timeout=30)
    assert owner.archive_members(path,linux_host.host_identity()['elf_machine'])==['native.o']


def test_staging_binds_architecture_and_archive_bytes_before_copy(tmp_path):
    resource,directory=runtime_tree(tmp_path)
    source=directory/'libclang_rt.builtins.a'
    record=owner.catalogue(resource,directory,linux_host.host_identity(),'a'*64)
    prefix=tmp_path/'sdk';result=owner.materialize(prefix,record)
    assert len(result['installed'])==len(owner.REQUIRED)
    assert owner.digest(prefix/result['installed'][0])==record['files'][0]['sha256']
    with pytest.raises(ValueError,match='already populated'):owner.materialize(prefix,record)
    source.write_bytes(source.read_bytes()+b'corruption')
    with pytest.raises(ValueError,match='changed'):owner.materialize(tmp_path/'another-sdk',record)


def runtime_tree(tmp_path):
    resource=tmp_path/'23';directory=resource/'lib'/linux_host.native_target()
    directory.mkdir(parents=True)
    for name in owner.REQUIRED:
        archive(directory/f'libclang_rt.{name}.a',[('native.o/',elf(linux_host.host_identity()['elf_machine']))])
    return resource,directory


@pytest.mark.parametrize('mutation',['missing-asan','foreign-asan','symlink','unrecognized','orphan-symbols','startup-type','shared-type'])
def test_complete_runtime_catalogue_rejects_incomplete_or_foreign_products(tmp_path,mutation):
    resource,directory=runtime_tree(tmp_path)
    if mutation=='missing-asan':(directory/'libclang_rt.asan.a').unlink()
    elif mutation=='foreign-asan':archive(directory/'libclang_rt.asan.a',[('foreign.o/',elf(183 if linux_host.host_identity()['elf_machine']==62 else 62))])
    elif mutation=='symlink':(directory/'libclang_rt.foreign.a').symlink_to(directory/'libclang_rt.builtins.a')
    elif mutation=='unrecognized':(directory/'libclang_rt.unrecognized.txt').write_text('unowned product')
    elif mutation=='orphan-symbols':(directory/'libclang_rt.unknown.a.syms').write_text('missing_archive')
    elif mutation=='startup-type':(directory/'clang_rt.crtbegin.o').write_bytes(elf(linux_host.host_identity()['elf_machine'])[:16]+b'\x03\x00'+elf(linux_host.host_identity()['elf_machine'])[18:])
    elif mutation=='shared-type':(directory/'libclang_rt.asan.so').write_bytes(elf(linux_host.host_identity()['elf_machine']))
    with pytest.raises(ValueError):owner.catalogue(resource,directory,linux_host.host_identity(),'a'*64)


def test_catalogue_stages_shared_runtime_and_symbol_lists(tmp_path):
    resource,directory=runtime_tree(tmp_path)
    (directory/'libclang_rt.asan.a.syms').write_text('__asan_init\n')
    body=bytearray(elf(linux_host.host_identity()['elf_machine']));body[16:18]=(3).to_bytes(2,'little')
    (directory/'libclang_rt.asan.so').write_bytes(body)
    (directory/'clang_rt.crtbegin.o').write_bytes(elf(linux_host.host_identity()['elf_machine']))
    record=owner.catalogue(resource,directory,linux_host.host_identity(),'a'*64)
    result=owner.materialize(tmp_path/'sdk',record)
    assert len(result['installed'])==len(owner.REQUIRED)+3
    record['files'][0]['resource_relative']='../../escape'
    with pytest.raises(ValueError,match='inventory changed'):owner.materialize(tmp_path/'bad',record)


def test_runtime_directory_and_destination_cannot_escape_their_roots(tmp_path):
    resource,directory=runtime_tree(tmp_path)
    with pytest.raises(ValueError,match='escaped'):owner.catalogue(resource,tmp_path,linux_host.host_identity(),'a'*64)
    record=owner.catalogue(resource,directory,linux_host.host_identity(),'a'*64)
    prefix=tmp_path/'sdk';(prefix/'lib').mkdir(parents=True)
    outside=tmp_path/'outside';outside.mkdir()
    (prefix/'lib/clang').symlink_to(outside,target_is_directory=True)
    with pytest.raises(ValueError,match='escaped'):owner.materialize(prefix,record)


@pytest.mark.parametrize('mutation',['none','wrong-version','external-resource'])
def test_discovery_binds_pinned_compiler_version_and_owned_resource_root(tmp_path,monkeypatch,mutation):
    bootstrap=tmp_path/'bootstrap';(bootstrap/'bin').mkdir(parents=True)
    (bootstrap/'bin/clang').write_bytes(elf(linux_host.host_identity()['elf_machine']))
    repo=tmp_path/'repo';(repo/'toolchains/llvm').mkdir(parents=True)
    (repo/'toolchains/llvm/manifest.json').write_text(json.dumps({'upstream':{'tag':'llvmorg-23.1.5'}}))
    resource,directory=runtime_tree(bootstrap/'lib/clang' if mutation!='external-resource' else tmp_path/'foreign')
    def output(arguments,**kwargs):
        assert kwargs['env']['LD_LIBRARY_PATH']==str(bootstrap/'lib')
        assert 'LD_PRELOAD' not in kwargs['env'] and 'LD_AUDIT' not in kwargs['env']
        if '--version' in arguments:return 'clang version 23.1.3' if mutation=='wrong-version' else 'clang version 23.1.5'
        if '-print-resource-dir' in arguments:return str(resource)
        assert '--target='+linux_host.native_target() in arguments
        return str(directory/'libclang_rt.builtins.a')
    monkeypatch.setenv('LD_PRELOAD','unowned');monkeypatch.setenv('LD_AUDIT','unowned')
    monkeypatch.setattr(owner.subprocess,'check_output',output)
    if mutation=='none':
        record=owner.discover(repo,bootstrap)
        assert len(record['files'])==len(owner.REQUIRED)
        assert record['bootstrap_compiler_sha256']==owner.digest(bootstrap/'bin/clang')
    else:
        with pytest.raises(ValueError,match='pinned'):owner.discover(repo,bootstrap)
