$ErrorActionPreference = "Stop"

function New-RecoveryContractExpectationRecord {
  param(
    [string]$Source,
    [string]$CaseName,
    [switch]$RequireLl,
    [switch]$RequireCompileProvenance,
    [switch]$UseCompileWrapper,
    [string[]]$ExtraArgs,
    [string[]]$RequiredLlTokens,
    [string[]]$ForbiddenLlTokens,
    [string[]]$RequiredManifestTokens,
    [switch]$RequireObjc3ManifestSurface
  )

  $record = @{
    Source = $Source
    CaseName = $CaseName
  }

  if ($RequireLl) { $record.RequireLl = $true }
  if ($RequireCompileProvenance) { $record.RequireCompileProvenance = $true }
  if ($UseCompileWrapper) { $record.UseCompileWrapper = $true }
  if ($PSBoundParameters.ContainsKey("ExtraArgs")) { $record.ExtraArgs = @($ExtraArgs) }
  if ($PSBoundParameters.ContainsKey("RequiredLlTokens")) { $record.RequiredLlTokens = @($RequiredLlTokens) }
  if ($PSBoundParameters.ContainsKey("ForbiddenLlTokens")) { $record.ForbiddenLlTokens = @($ForbiddenLlTokens) }
  if ($PSBoundParameters.ContainsKey("RequiredManifestTokens")) { $record.RequiredManifestTokens = @($RequiredManifestTokens) }
  if ($RequireObjc3ManifestSurface) { $record.RequireObjc3ManifestSurface = $true }

  Write-Output -NoEnumerate $record
}

Export-ModuleMember -Function @(
  "New-RecoveryContractExpectationRecord"
)
