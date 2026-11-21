"""Bound prepared analysis work and durably harvest completed native actions."""
from concurrent.futures import FIRST_COMPLETED, wait


def execute_jobs(jobs, *, submit, completed, max_pending):
    if type(max_pending) is not int or not 1 <= max_pending <= 8:
        raise ValueError('analysis scheduling requires a bounded pending window')
    pending = set()

    def harvest(futures):
        failure = None
        for future in futures:
            pending.remove(future)
            if future.cancelled():
                continue
            try:
                completed(future.result())
            except BaseException as error:
                if failure is None:
                    failure = error
        if failure is not None:
            raise failure

    iterator = iter(jobs)
    try:
        while True:
            # Harvest before asking the producer to prepare another PCH/TU.
            # This bounds both queued work and completed-but-unpublished proof.
            if len(pending) >= max_pending:
                harvest(wait(pending, return_when=FIRST_COMPLETED).done)
            try:
                job = next(iterator)
            except StopIteration:
                break
            pending.add(submit(job))
            harvest({future for future in pending if future.done()})
        while pending:
            harvest(wait(pending, return_when=FIRST_COMPLETED).done)
    except BaseException:
        for future in pending:
            future.cancel()
        # A preparation/worker error must not discard successful actions that
        # are already running. Preserve the original failure after draining.
        while pending:
            try:
                harvest(wait(pending, return_when=FIRST_COMPLETED).done)
            except BaseException:
                pass
        raise
