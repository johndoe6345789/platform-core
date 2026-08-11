import { renderHook, waitFor } from
  '@testing-library/react';
import { useAudit } from './useAudit';

type FM = jest.Mock<Promise<Response>>;
const ok = (b: unknown): Response =>
  ({ ok: true, status: 200, json: async () => b })
    as Response;

describe('useAudit', () => {
  let fm: FM;
  beforeEach(() => {
    fm = jest.fn();
    global.fetch = fm as unknown as typeof fetch;
  });

  it('GETs events with tenant params', async () => {
    fm.mockResolvedValue(ok([{ id: 1 }]));
    const { result } = renderHook(() =>
      useAudit({ tenant: 't1', limit: 5 }),
    );
    await waitFor(() =>
      expect(result.current.events).toHaveLength(1),
    );
    const url = String(fm.mock.calls[0][0]);
    expect(url).toContain('/audit/api/audit/events');
    expect(url).toContain('tenant=t1');
    expect(url).toContain('limit=5');
  });

  it('skips when tenant empty', () => {
    fm.mockResolvedValue(ok([]));
    renderHook(() => useAudit({ tenant: '' }));
    expect(fm).not.toHaveBeenCalled();
  });

  it('sets error on failure', async () => {
    fm.mockResolvedValue(
      { ok: false, status: 500 } as Response,
    );
    const { result } = renderHook(() =>
      useAudit({ tenant: 'x' }),
    );
    await waitFor(() =>
      expect(result.current.error).toMatch(/500/),
    );
    expect(result.current.events).toEqual([]);
  });
});
