Objc3AwaitSuspensionProfile BuildAwaitSuspensionProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3AwaitSuspensionSiteCounts counts =
      CountAwaitSuspensionSitesInBody(fn.body);
  return BuildAwaitSuspensionProfileFromCounts(
      counts.await_keyword_sites,
      counts.await_suspension_point_sites,
      counts.await_resume_sites,
      counts.await_state_machine_sites,
      counts.await_continuation_sites);
}
