Set-StrictMode -Version Latest

function Copy-Objc3cNativePerfProofFixture {
  param(
    [object]$Fixture,
    [string]$Directory
  )

  New-Item -ItemType Directory -Force -Path $Directory | Out-Null
  $fixtureSource = Join-Path $Directory $Fixture.Name
  Copy-Item -LiteralPath $Fixture.FullName -Destination $fixtureSource -Force
  return $fixtureSource
}
