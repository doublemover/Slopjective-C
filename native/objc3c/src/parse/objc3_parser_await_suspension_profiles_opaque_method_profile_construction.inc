Objc3AwaitSuspensionProfile BuildAwaitSuspensionProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3AwaitSuspensionSiteCounts counts;
  if (method.has_body) {
    CollectAwaitSuspensionSitesFromSymbol(method.selector, counts);
  }
  return BuildAwaitSuspensionProfileFromCounts(
      counts.await_keyword_sites,
      counts.await_suspension_point_sites,
      counts.await_resume_sites,
      counts.await_state_machine_sites,
      counts.await_continuation_sites);
}
