Set-StrictMode -Version Latest

function Get-NormalizedReadobjSectionText {
  param([Parameter(Mandatory = $true)][string]$Text)

  $normalizedLines = foreach ($line in ($Text -split "`r?`n")) {
    if ($line.StartsWith("File: ")) {
      "File: <canonical-object>"
    }
    else {
      $line
    }
  }
  return (($normalizedLines -join "`n").TrimEnd() + "`n")
}

function Get-ReadobjSectionNames {
  param([Parameter(Mandatory = $true)][string]$Text)

  $names = [System.Collections.Generic.List[string]]::new()
  foreach ($line in ($Text -split "`r?`n")) {
    $trimmed = $line.Trim()
    if ($trimmed.StartsWith("Name: ")) {
      $name = $trimmed.Substring(6)
      $parenIndex = $name.IndexOf(" (", [System.StringComparison]::Ordinal)
      if ($parenIndex -ge 0) {
        $name = $name.Substring(0, $parenIndex)
      }
      $names.Add($name.Trim())
    }
  }
  return @($names)
}

function Assert-RequiredTextTokens {
  param(
    [Parameter(Mandatory = $true)][string]$Text,
    [Parameter(Mandatory = $true)][string[]]$Tokens,
    [Parameter(Mandatory = $true)][string]$CaseId
  )

  foreach ($token in $Tokens) {
    if ([string]::IsNullOrWhiteSpace($token)) {
      continue
    }
    if ($Text.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
      throw "execution replay proof FAIL: missing required token '$token' for $CaseId"
    }
  }
}

function Compare-ExecutionReplayProofRuns {
  param(
    [Parameter(Mandatory = $true)][object]$Case,
    [Parameter(Mandatory = $true)][object]$Run1,
    [Parameter(Mandatory = $true)][object]$Run2
  )

  foreach ($field in @("manifest_sha256", "registration_manifest_sha256", "provenance_sha256", "diagnostics_sha256", "ir_sha256", "object_sha256", "artifact_set_digest_sha256")) {
    if ([string]$Run1[$field] -ne [string]$Run2[$field]) {
      throw "execution replay proof FAIL: $field drift across replay for $($Case.case_id) (run1=$($Run1[$field]) run2=$($Run2[$field]))"
    }
  }
  if ((@($Case.required_runtime_sections)).Count -gt 0) {
    if ([string]$Run1.section_inspection_sha256 -ne [string]$Run2.section_inspection_sha256) {
      throw "execution replay proof FAIL: section inspection drift across replay for $($Case.case_id)"
    }
    if (($Run1.section_names -join "|") -ne ($Run2.section_names -join "|")) {
      throw "execution replay proof FAIL: section inventory drift across replay for $($Case.case_id)"
    }
  }
}
