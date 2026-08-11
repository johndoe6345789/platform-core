import { renderHook, waitFor } from
  '@testing-library/react';
import { useJobs } from './useJobs';

type FM = jest.Mock<Promise<Response>>;
const ok = (b: unknown): Response =>
  ({ ok: true, status: 200, json: async () => b })
    as Response;

describe('useJobs', () => {
  let fm: FM;
  beforeEach(() => {
    fm = jest.fn();
    global.fetch = fm as unknown as typeof fetch;
  });

  it('fetches queue, runs and dead', async () => {
    fm
      .mockResolvedValueOnce(ok({ items: [{ id: 1 }] }))
      .mockResolvedValueOnce(ok({ items: [{ id: 2 }] }))
      .mockResolvedValueOnce(ok({ items: [{ id: 3 }] }));
    const { result } = renderHook(() => useJobs());
    await waitFor(() =>
      expect(result.current.dead.length).toBe(1),
    );
    expect(fm.mock.calls[0][0]).toContain('/queue');
    expect(fm.mock.calls[1][0]).toContain('/runs');
    expect(fm.mock.calls[2][0]).toContain('/dead_letter');
  });
});
