import { renderHook, act, waitFor } from
  '@testing-library/react';
import { useIntegrity } from './useIntegrity';

type FM = jest.Mock<Promise<Response>>;
const ok = (b: unknown): Response =>
  ({ ok: true, status: 200, json: async () => b })
    as Response;

describe('useIntegrity', () => {
  let fm: FM;
  beforeEach(() => {
    fm = jest.fn();
    global.fetch = fm as unknown as typeof fetch;
  });

  it('verifies via query param', async () => {
    fm.mockResolvedValue(
      ok({
        ok: true, rows_checked: 9,
        message: 'ok',
      }),
    );
    const { result } = renderHook(() =>
      useIntegrity('t 1'),
    );
    await act(async () => {
      await result.current.verify();
    });
    await waitFor(() =>
      expect(result.current.report?.ok).toBe(true),
    );
    const url = String(fm.mock.calls[0][0]);
    expect(url).toContain('/audit/api/audit/verify');
    expect(url).toContain('tenant=t%201');
  });

  it('no-ops on empty tenant', async () => {
    const { result } = renderHook(() =>
      useIntegrity(''),
    );
    await act(async () => {
      await result.current.verify();
    });
    expect(fm).not.toHaveBeenCalled();
  });

  it('records error on HTTP failure', async () => {
    fm.mockResolvedValue(
      { ok: false, status: 503 } as Response,
    );
    const { result } = renderHook(() =>
      useIntegrity('t'),
    );
    await act(async () => {
      await result.current.verify();
    });
    await waitFor(() =>
      expect(result.current.error).toMatch(/503/),
    );
  });
});
