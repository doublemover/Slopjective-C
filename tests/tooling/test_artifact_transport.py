"""Artifact chunking preserves exact files while keeping transfers bounded."""
import hashlib
import io
import tarfile

import pytest

from scripts.objc3c_toolchain import artifact_transport as owner


def test_exact_reconstruction_including_metadata_and_empty_files(tmp_path):
    source=tmp_path/'source';source.mkdir();(source/'metadata').mkdir()
    data=bytes(range(256))*4000
    (source/'compiler.tar.zst').write_bytes(data)
    (source/'metadata/receipt.json').write_text('{"status":"FAIL"}\n')
    (source/'empty').touch()
    result=owner.split(source,tmp_path/'parts','123','unaccepted-compiler')
    parts=[(tmp_path/'parts'/row['name']).read_bytes() for row in result['parts']]
    assert all(hashlib.sha256(data).hexdigest()==row['sha256'] and len(data)==row['bytes']
               for data,row in zip(parts,result['parts'],strict=True))
    restored=b''.join(parts)
    assert len(restored)==result['payload_bytes']
    assert hashlib.sha256(restored).hexdigest()==result['payload_sha256']
    with tarfile.open(fileobj=io.BytesIO(restored)) as tar:
        assert tar.extractfile('compiler.tar.zst').read()==data
        assert tar.extractfile('metadata/receipt.json').read()==b'{"status":"FAIL"}\n'
        assert tar.extractfile('empty').read()==b''
    damaged=bytearray(restored);damaged[len(damaged)//2]^=1
    assert hashlib.sha256(damaged).hexdigest()!=result['payload_sha256']


@pytest.mark.parametrize('damage',['symlink','oversize','existing','nested'])
def test_transport_rejects_unowned_or_unbounded_payloads(tmp_path,monkeypatch,damage):
    source=tmp_path/'source';source.mkdir();(source/'data').write_bytes(b'input')
    output=tmp_path/'parts'
    if damage=='symlink':(source/'link').symlink_to(tmp_path/'outside')
    elif damage=='oversize':monkeypatch.setattr(owner,'MAX_PART_BYTES',1)
    elif damage=='existing':output.mkdir();(output/'keep').write_bytes(b'preserved')
    else:output=source/'nested'
    with pytest.raises(ValueError):owner.split(source,output,'123','unaccepted-compiler')
    if damage=='existing':assert (output/'keep').read_bytes()==b'preserved'
