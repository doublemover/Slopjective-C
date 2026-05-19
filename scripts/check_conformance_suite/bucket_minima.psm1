Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "fixtures.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "state.psm1") -Force -DisableNameChecking

function Invoke-ConformanceBucketMinimaCheck {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][hashtable]$State
  )

  $bucketMinima = [ordered]@{
    parser = 15
    semantic = 25
    lowering_abi = 10
    module_roundtrip = 12
    diagnostics = 20
  }

  Write-Output "Conformance bucket minima check:"
  foreach ($bucket in $bucketMinima.Keys) {
    $bucketPath = Join-Path $RepoRoot "tests/conformance/$bucket"
    if (-not (Test-Path -LiteralPath $bucketPath -PathType Container)) {
      Add-ConformanceSuiteFailure -State $State -Message "Missing required bucket directory: tests/conformance/$bucket"
      continue
    }

    $bucketFiles = Get-ChildItem -LiteralPath $bucketPath -File -Filter "*.json" |
      Sort-Object -Property Name |
      Where-Object { $_.Name -ne "manifest.json" }
    $executableCount = 0
    $metadataCount = 0
    foreach ($file in $bucketFiles) {
      $raw = $null
      try {
        $raw = Get-Content -LiteralPath $file.FullName -Raw -Encoding utf8
        $payload = $raw | ConvertFrom-Json
      } catch {
        Add-ConformanceSuiteFailure -State $State -Message ("Bucket '{0}' fixture parse failure: {1} ({2})" -f $bucket, $file.FullName, $_.Exception.Message)
        continue
      }

      if ($null -eq $payload -or ($payload -isnot [pscustomobject])) {
        Add-ConformanceSuiteFailure -State $State -Message ("Bucket '{0}' fixture shape failure: {1} must be a JSON object" -f $bucket, $file.FullName)
        continue
      }

      $fixtureId = Resolve-ConformanceFixtureId -Payload $payload -RetiredRouteId $file.BaseName
      $State["AllFixtureIds"].Add($fixtureId) | Out-Null

      if (Test-ConformanceExecutableFixture -Payload $payload) {
        $executableCount += 1
        if ((-not $State["SmokeBucketSelection"].Contains($bucket)) -and (Test-ConformanceExecutableReplaySmokeCandidate -Payload $payload)) {
          $State["SmokeBucketSelection"].Add($bucket) | Out-Null
          $State["ExecutableSmokeCandidates"].Add([pscustomobject]@{
              bucket = $bucket
              fixture_id = $fixtureId
              source = "$($payload.source)"
              source_file = $file.FullName
            }) | Out-Null
        }
        continue
      }

      if (Test-ConformanceMetadataFixtureRecord -Payload $payload) {
        $metadataCount += 1
        $State["MetadataOnlyFixtureCount"] = [int]$State["MetadataOnlyFixtureCount"] + 1
        continue
      }

      Add-ConformanceSuiteFailure -State $State -Message ("Bucket '{0}' fixture shape failure: {1} must include ('source' and 'expect') or include a non-empty string 'fixture_id'" -f $bucket, $file.FullName)
    }

    Write-Output ("- {0}: executable={1} metadata_only={2} (minimum executable {3})" -f $bucket, $executableCount, $metadataCount, $bucketMinima[$bucket])
    if ($executableCount -lt $bucketMinima[$bucket]) {
      Add-ConformanceSuiteFailure -State $State -Message ("Bucket '{0}' has {1} executable fixtures, below minimum {2}" -f $bucket, $executableCount, $bucketMinima[$bucket])
    }
  }
}

Export-ModuleMember -Function "Invoke-ConformanceBucketMinimaCheck"
