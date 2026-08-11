import { renderHook, waitFor } from
  '@testing-library/react';
import { useFlags } from './useFlags';

type FM = jest.Mock<Promise<Response>>;
const ok = (b: unknown): Response =>
  ({ ok: true, status: 200, json: async () => b })
    as Response;

describe('useFlags', () => {
  let fm: FM;
  beforeEach(() => {
    fm = jest.fn();
    global.fetch = fm as unknown as typeof fetch;
  });

  it('loads flag list', async () => {
    fm.mockResolvedValue(
      ok({ flags: [{ key: 'a' }] }),
    );
    const { result } = renderHook(() => useFlags());
    await waitFor(() =>
      expect(result.current.flags.length).toBe(1),
    );
    expect(fm.mock.calls[0][0]).toBe('/flags/api/flags');
  });

  it('sends credentials', async () => {
    fm.mockResolvedValue(ok({ flags: [] }));
    renderHook(() => useFlags());
    await waitFor(() =>
      expect(fm).toHaveBeenCalled(),
    );
    const init = fm.mock.calls[0][1] as RequestInit;
    expect(init.credentials).toBe('include');
  });

  it('captures error', async () => {
    fm.mockResolvedValue(
      { ok: false, status: 500 } as Response,
    );
    const { result } = renderHook(() => useFlags());
    await waitFor(() =>
      expect(result.current.error).toMatch(/500/),
    );
  });
});
