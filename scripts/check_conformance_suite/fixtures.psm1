Set-StrictMode -Version Latest

function Test-ConformancePayloadProperty {
  param(
    [object]$Object,
    [string]$Name
  )

  if ($null -eq $Object) {
    return $false
  }
  return $Object.PSObject.Properties.Name -contains $Name
}

function Test-ConformanceExecutableFixture {
  param([object]$Payload)

  return (Test-ConformancePayloadProperty -Object $Payload -Name "source") -and (Test-ConformancePayloadProperty -Object $Payload -Name "expect")
}

function Test-ConformanceMetadataFixtureRecord {
  param([object]$Payload)

  if ($null -eq $Payload) {
    return $false
  }
  if (-not (Test-ConformancePayloadProperty -Object $Payload -Name "fixture_id")) {
    return $false
  }
  if ($Payload.fixture_id -isnot [string]) {
    return $false
  }
  return $Payload.fixture_id.Trim().Length -gt 0
}

function Test-ConformanceExecutableReplaySmokeCandidate {
  param([object]$Payload)

  if (-not (Test-ConformanceExecutableFixture -Payload $Payload)) {
    return $false
  }
  return (Test-ConformancePayloadProperty -Object $Payload -Name "source") -and (-not [string]::IsNullOrWhiteSpace("$($Payload.source)"))
}

function Resolve-ConformanceFixtureId {
  param(
    [object]$Payload,
    [string]$RetiredRouteId
  )

  if ((Test-ConformancePayloadProperty -Object $Payload -Name "fixture_id") -and ($Payload.fixture_id -is [string]) -and ($Payload.fixture_id.Trim().Length -gt 0)) {
    return $Payload.fixture_id.Trim()
  }
  return $RetiredRouteId
}

Export-ModuleMember -Function @(
  "Resolve-ConformanceFixtureId",
  "Test-ConformanceExecutableFixture",
  "Test-ConformanceExecutableReplaySmokeCandidate",
  "Test-ConformanceMetadataFixtureRecord",
  "Test-ConformancePayloadProperty"
)
