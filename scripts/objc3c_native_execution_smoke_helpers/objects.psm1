Set-StrictMode -Version Latest

function Resolve-NativeObjectPath {
  param(
    [Parameter(Mandatory = $true)][string]$CompileDir,
    [Parameter(Mandatory = $true)][string]$FixtureRel
  )

  $preferredNames = @("module.obj", "module.o")
  foreach ($name in $preferredNames) {
    $candidate = Join-Path $CompileDir $name
    if (Test-Path -LiteralPath $candidate -PathType Leaf) {
      return $candidate
    }
  }

  $moduleNamedCandidates = @(
    Get-ChildItem -LiteralPath $CompileDir -File -ErrorAction SilentlyContinue |
      Where-Object { $_.BaseName -eq "module" -and ($_.Extension -eq ".obj" -or $_.Extension -eq ".o") } |
      Sort-Object -Property Name
  )
  if ($moduleNamedCandidates.Count -eq 1) {
    return $moduleNamedCandidates[0].FullName
  }
  if ($moduleNamedCandidates.Count -gt 1) {
    throw "execution smoke FAIL: ambiguous native object artifacts for $fixtureRel in $CompileDir"
  }

  $allObjectCandidates = @(
    Get-ChildItem -LiteralPath $CompileDir -File -ErrorAction SilentlyContinue |
      Where-Object { $_.Extension -eq ".obj" -or $_.Extension -eq ".o" } |
      Sort-Object -Property Name
  )
  if ($allObjectCandidates.Count -eq 1) {
    return $allObjectCandidates[0].FullName
  }

  throw "execution smoke FAIL: missing native object artifact (.obj/.o) for $fixtureRel in $CompileDir"
}

Export-ModuleMember -Function @(
  "Resolve-NativeObjectPath"
)
