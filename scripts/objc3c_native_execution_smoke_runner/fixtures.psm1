Set-StrictMode -Version Latest

$script:ScriptsRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:ScriptsRoot "objc3c_native_execution_smoke_helpers.psm1") -Force -DisableNameChecking

function New-ExecutionFixtureEntries {
  param(
    [Parameter(Mandatory = $true)][object[]]$PositiveFixtures,
    [Parameter(Mandatory = $true)][object[]]$NegativeFixtures,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  return @(
    $PositiveFixtures | ForEach-Object {
      [pscustomobject]@{
        kind = "positive"
        file = $_
        relative_path = Get-RepoRelativePath -Path $_.FullName -Root $RepoRoot
      }
    }
  ) + @(
    $NegativeFixtures | ForEach-Object {
      [pscustomobject]@{
        kind = "negative"
        file = $_
        relative_path = Get-RepoRelativePath -Path $_.FullName -Root $RepoRoot
      }
    }
  )
}

Export-ModuleMember -Function @(
  "New-ExecutionFixtureEntries"
)
