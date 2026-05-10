Set-StrictMode -Version Latest

function Get-EntrypointLlSurface {
  param(
    [string]$LlText
  )

  $lines = $LlText -split "`r?`n"
  $capturing = $false
  $braceDepth = 0
  $captured = New-Object System.Collections.Generic.List[string]

  foreach ($line in $lines) {
    if (-not $capturing -and $line -match '^\s*define\s+i32\s+@(main|objc3c_entry)\(') {
      $capturing = $true
    }

    if (-not $capturing) {
      continue
    }

    $captured.Add($line)
    $braceDepth += ([regex]::Matches($line, '\{')).Count
    $braceDepth -= ([regex]::Matches($line, '\}')).Count

    if ($braceDepth -le 0 -and $line -match '^\s*\}') {
      $capturing = $false
      $braceDepth = 0
    }
  }

  return ($captured -join "`n")
}

Export-ModuleMember -Function @(
  "Get-EntrypointLlSurface"
)
