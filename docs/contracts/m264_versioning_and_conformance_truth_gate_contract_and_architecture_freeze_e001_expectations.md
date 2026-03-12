# M264-E001 Expectations

Issue: `#7242`
Packet: `M264-E001`
Milestone: `M264`
Lane: `E`

The integrated M264 gate must freeze one truthful versioning/conformance
boundary.

Canonical identifiers:

- contract id: `objc3c-versioning-conformance-truth-gate/m264-e001-v1`

Required boundary:

- claimed profile remains `core`
- compatibility selection remains `canonical|legacy`
- migration assist remains live
- strictness and strict concurrency remain fail-closed
- feature-macro claims remain suppressed
- the emitted machine-readable surface consists of:
  - `module.objc3-conformance-report.json`
  - `module.objc3-conformance-publication.json`
  - `module.objc3-conformance-validation.json`
- the frontend C API runner publishes the same lowered/publication truth surface
  as the native CLI
- the D002 validation path consumes only the shipped JSON sidecars
- the gate consumes and preserves the already-validated A002, B003, C002, and
  D002 milestone summaries
- `M264-E002` is the next issue allowed to widen evidence/reporting beyond this
  freeze boundary
