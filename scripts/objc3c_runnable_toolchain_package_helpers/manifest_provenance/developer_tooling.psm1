Set-StrictMode -Version Latest

function Get-ManifestProvenanceDeveloperToolingFiles {
  return @(
    "scripts/build_objc3c_editor_tooling_surface.py",
    "scripts/objc3c_editor_tooling/workspace_index.py",
    "scripts/format_objc3c_source.py",
    "scripts/rewrite_objc3c_source.py",
    "scripts/check_developer_tooling_language_server_navigation.py",
    "scripts/check_developer_tooling_formatter_debug_surface.py",
    "scripts/check_developer_tooling_formatter_rewrite_surface.py",
    "scripts/check_developer_tooling_diagnostic_quality.py",
    "scripts/check_developer_tooling_workspace_integration.py",
    "scripts/check_objc3c_developer_tooling_integration.py",
    "scripts/check_objc3c_runnable_developer_tooling_end_to_end.py"
  )
}

Export-ModuleMember -Function @("Get-ManifestProvenanceDeveloperToolingFiles")
