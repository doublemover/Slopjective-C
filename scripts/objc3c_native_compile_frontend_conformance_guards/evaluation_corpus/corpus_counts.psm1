$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Assert-FrontendConformanceCorpusCounts {
  param(
    [object[]]$AcceptanceRows,
    [object[]]$RejectionRows,
    [object]$Payload,
    [string]$ArtifactPath
  )

  if ([int]$Payload.acceptance_corpus_count -ne $AcceptanceRows.Count) {
    Stop-FrontendConformanceGuard "frontend conformance corpus acceptance_corpus_count mismatch in $ArtifactPath"
  }
  if ([int]$Payload.rejection_corpus_count -ne $RejectionRows.Count) {
    Stop-FrontendConformanceGuard "frontend conformance corpus rejection_corpus_count mismatch in $ArtifactPath"
  }
  if ([int]$Payload.corpus_case_count -ne ($AcceptanceRows.Count + $RejectionRows.Count)) {
    Stop-FrontendConformanceGuard "frontend conformance corpus corpus_case_count mismatch in $ArtifactPath"
  }
  if ($AcceptanceRows.Count -le 0 -or $RejectionRows.Count -le 0) {
    Stop-FrontendConformanceGuard "frontend conformance corpus requires non-empty acceptance and rejection corpus in $ArtifactPath"
  }
}
