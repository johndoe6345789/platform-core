import { renderHook, act } from
  '@testing-library/react';
import { useFlagMutations } from
  './useFlagMutations';

type FM = jest.Mock<Promise<Response>>;
const ok = (b: unknown): Response =>
  ({ ok: true, status: 200, json: async () => b })
    as Response;

describe('useFlagMutations', () => {
  let fm: FM;
  beforeEach(() => {
    fm = jest.fn();
    global.fetch = fm as unknown as typeof fetch;
  });

  it('PATCHes with JSON body', async () => {
    fm.mockResolvedValue(ok({ ok: true }));
    const { result } = renderHook(() =>
      useFlagMutations(),
    );
    await act(async () => {
      await result.current.patchFlag('a', {
        enabled: true, rollout_pct: 50,
      });
    });
    const [url, init] = fm.mock.calls[0];
    expect(String(url)).toContain('/a');
    expect((init as RequestInit).method).toBe('PATCH');
    const body = JSON.parse(
      (init as RequestInit).body as string,
    );
    expect(body.enabled).toBe(true);
  });

  it('records error on failure', async () => {
    fm.mockResolvedValue(
      { ok: false, status: 400 } as Response,
    );
    const { result } = renderHook(() =>
      useFlagMutations(),
    );
    await act(async () => {
      await expect(
        result.current.patchFlag('k', {}),
      ).rejects.toThrow(/400/);
    });
    expect(result.current.error).toMatch(/400/);
  });
});
