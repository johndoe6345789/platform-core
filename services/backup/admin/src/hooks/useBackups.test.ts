import { renderHook, waitFor } from
  '@testing-library/react';
import { useBackups } from './useBackups';

type FM = jest.Mock<Promise<Response>>;
const ok = (b: unknown): Response =>
  ({ ok: true, status: 200, json: async () => b })
    as Response;

describe('useBackups', () => {
  let fm: FM;
  beforeEach(() => {
    fm = jest.fn();
    global.fetch = fm as unknown as typeof fetch;
  });

  it('fetches runs and policies', async () => {
    fm.mockResolvedValueOnce(
      ok({ items: [{ id: '1' }] }),
    ).mockResolvedValueOnce(
      ok({ items: [{ id: '2' }] }),
    );
    const { result } = renderHook(() => useBackups());
    await waitFor(() =>
      expect(result.current.runs.length).toBe(1),
    );
    expect(result.current.policies.length).toBe(1);
    expect(fm.mock.calls[0][0]).toContain('/runs');
    expect(fm.mock.calls[1][0]).toContain('/policy');
  });

  it('tolerates network errors', async () => {
    fm.mockRejectedValue(new Error('net'));
    const { result } = renderHook(() => useBackups());
    await waitFor(() =>
      expect(fm).toHaveBeenCalled(),
    );
    expect(result.current.runs).toEqual([]);
  });
});
