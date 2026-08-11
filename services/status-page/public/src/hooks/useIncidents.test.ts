import { renderHook, waitFor } from
  '@testing-library/react';
import { useIncidents } from './useIncidents';

type FM = jest.Mock<Promise<Response>>;
const ok = (b: unknown): Response =>
  ({ ok: true, status: 200, json: async () => b })
    as Response;

describe('useIncidents', () => {
  let fm: FM;
  beforeEach(() => {
    fm = jest.fn();
    global.fetch = fm as unknown as typeof fetch;
  });

  it('loads active and history in parallel',
    async () => {
    fm
      .mockResolvedValueOnce(
        ok({ incidents: [{ id: 1 }] }),
      )
      .mockResolvedValueOnce(
        ok({ history: [{ id: 2 }] }),
      );
    const { result } =
      renderHook(() => useIncidents());
    await waitFor(() =>
      expect(result.current.loading).toBe(false),
    );
    expect(result.current.active.length).toBe(1);
    expect(result.current.history.length).toBe(1);
    expect(fm.mock.calls[0][0]).toContain(
      '/api/status/incidents',
    );
    expect(fm.mock.calls[1][0]).toContain(
      '/api/status/history',
    );
  });

  it('records error on failure', async () => {
    fm.mockRejectedValue(new Error('boom'));
    const { result } =
      renderHook(() => useIncidents());
    await waitFor(() =>
      expect(result.current.error).toMatch(/boom/),
    );
  });
});
