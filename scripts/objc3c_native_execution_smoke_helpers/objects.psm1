Set-StrictMode -Version Latest

function Resolve-NativeObjectPath {
  param(
    [Parameter(Mandatory = $true)][string]$CompileDir,
    [Parameter(Mandatory = $true)][string]$FixtureRel
  )

  $objectPath = Join-Path $CompileDir "module.obj"
  if (Test-Path -LiteralPath $objectPath -PathType Leaf) {
    return $objectPath
  }

  throw "execution smoke FAIL: missing native object artifact module.obj for $fixtureRel in $CompileDir"
}

Export-ModuleMember -Function @(
  "Resolve-NativeObjectPath"
)
