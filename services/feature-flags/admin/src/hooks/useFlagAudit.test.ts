import { renderHook, waitFor } from
  '@testing-library/react';
import { useFlagAudit } from './useFlagAudit';

type FM = jest.Mock<Promise<Response>>;
const ok = (b: unknown): Response =>
  ({ ok: true, status: 200, json: async () => b })
    as Response;

describe('useFlagAudit', () => {
  let fm: FM;
  beforeEach(() => {
    fm = jest.fn();
    global.fetch = fm as unknown as typeof fetch;
  });

  it('no-ops when key null', async () => {
    const { result } = renderHook(() =>
      useFlagAudit(null),
    );
    expect(fm).not.toHaveBeenCalled();
    expect(result.current.audit).toEqual([]);
  });

  it('fetches audit rows', async () => {
    fm.mockResolvedValue(
      ok({
        audit: [
          {
            actor: 'u', prev: {}, next: {},
            at: 'now',
          },
        ],
      }),
    );
    const { result } = renderHook(() =>
      useFlagAudit('my key'),
    );
    await waitFor(() =>
      expect(result.current.audit.length).toBe(1),
    );
    expect(String(fm.mock.calls[0][0])).toContain(
      'my%20key/audit',
    );
  });

  it('handles non-ok', async () => {
    fm.mockResolvedValue(
      { ok: false, status: 404 } as Response,
    );
    const { result } = renderHook(() =>
      useFlagAudit('k'),
    );
    await waitFor(() =>
      expect(result.current.loading).toBe(false),
    );
    expect(result.current.audit).toEqual([]);
  });
});
