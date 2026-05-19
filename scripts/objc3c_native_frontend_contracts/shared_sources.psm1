function Get-Objc3cNativeFrontendSharedSources {
  param([object[]]$Modules)

  $seen = @{}
  $flattened = New-Object System.Collections.Generic.List[string]
  foreach ($module in $Modules) {
    $name = [string]$module.name
    $sources = @($module.sources)
    if ([string]::IsNullOrWhiteSpace($name)) {
      throw "frontend module entry missing name"
    }
    if ($sources.Count -eq 0) {
      throw "frontend module '$name' must declare at least one source"
    }
    foreach ($source in $sources) {
      if ($seen.ContainsKey($source)) {
        throw "duplicate frontend shared source entry: $source"
      }
      $seen[$source] = $true
      $flattened.Add($source)
    }
  }
  return $flattened.ToArray()
}

Export-ModuleMember -Function @(
  "Get-Objc3cNativeFrontendSharedSources"
)
