      result.parity_surface.deterministic_closeout_surface_readiness_record &&
      IsReadyObjc3SemaCloseoutSurfaceReadinessRecord(
          result.parity_surface.closeout_surface_readiness_record);
  result.closeout_signoff_record = BuildObjc3SemaCloseoutSignoffRecord(
      input, result.parity_surface);
  result.deterministic_closeout_signoff_record =
      IsReadyObjc3SemaCloseoutSignoffRecord(result.closeout_signoff_record);
  result.parity_surface.closeout_signoff_record = result.closeout_signoff_record;
  result.parity_surface.deterministic_closeout_signoff_record =
      result.deterministic_closeout_signoff_record;
  result.parity_surface.ready =
      result.parity_surface.ready &&
      result.parity_surface.deterministic_closeout_signoff_record &&
      IsReadyObjc3SemaCloseoutSignoffRecord(
          result.parity_surface.closeout_signoff_record);
  return result;
}
