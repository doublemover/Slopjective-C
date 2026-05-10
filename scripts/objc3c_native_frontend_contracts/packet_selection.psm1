function Get-Objc3cNativeSelectedFrontendPacketDefinitions {
  param(
    [Parameter(Mandatory = $true)][string]$Mode,
    [Parameter(Mandatory = $true)][object[]]$PacketDefinitions
  )

  switch ($Mode) {
    "contracts-source" {
      return @($PacketDefinitions | Where-Object { $_.Family -eq "source-derived" })
    }
    "contracts-binary" {
      return @($PacketDefinitions | Where-Object { $_.Family -in @("source-derived", "binary-derived") })
    }
    "contracts-closeout" {
      return @($PacketDefinitions | Where-Object { $_.Family -in @("source-derived", "binary-derived", "closeout-derived") })
    }
    "contracts-all" {
      return @($PacketDefinitions)
    }
    "full" {
      return @($PacketDefinitions)
    }
    default {
      return @()
    }
  }
}
