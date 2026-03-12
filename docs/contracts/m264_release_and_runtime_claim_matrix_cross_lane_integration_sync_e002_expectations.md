# M264-E002 Expectations

Issue: `#7243`
Packet: `M264-E002`
Milestone: `M264`
Lane: `E`

The integrated M264 closeout must publish one truthful release/runtime claim
matrix for the currently runnable Objective-C 3 native subset.

Canonical identifiers:

- contract id: `objc3c-release-runtime-claim-matrix/m264-e002-v1`

Required release matrix:

- claimed profile remains `core`
- compatibility modes remain `canonical|legacy`
- migration assist remains live
- strict, strict-concurrency, and strict-system remain fail-closed and
  unclaimed
- feature-macro claims remain suppressed
- JSON is the only runnable emit/validate format
- native CLI matrix evidence consumes:
  - `module.objc3-conformance-report.json`
  - `module.objc3-conformance-publication.json`
  - `module.objc3-conformance-validation.json`
- frontend C API matrix evidence consumes:
  - `module.objc3-conformance-report.json`
  - `module.objc3-conformance-publication.json`
- the matrix consumes and preserves the already-validated A002, B003, C002,
  D002, and E001 milestone summaries
- the published evidence lands at:
  - `tmp/reports/m264/M264-E002/release_runtime_claim_matrix.json`
  - `tmp/reports/m264/M264-E002/release_runtime_claim_matrix.md`
- `M265-A001` is the next issue once this closeout lands
