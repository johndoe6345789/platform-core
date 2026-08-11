import { renderHook, waitFor } from
  '@testing-library/react';
import { useStatus } from './useStatus';

type FM = jest.Mock<Promise<Response>>;
const ok = (b: unknown): Response =>
  ({ ok: true, status: 200, json: async () => b })
    as Response;

describe('useStatus', () => {
  let fm: FM;
  beforeEach(() => {
    fm = jest.fn();
    global.fetch = fm as unknown as typeof fetch;
  });

  it('loads service rows from /api/healthz',
    async () => {
    fm.mockResolvedValue(
      ok({
        services: [{ name: 'api', ok: true }],
      }),
    );
    const { result } = renderHook(() => useStatus());
    await waitFor(() =>
      expect(result.current.loading).toBe(false),
    );
    expect(result.current.services[0].name).toBe('api');
    expect(fm.mock.calls[0][0]).toBe('/api/healthz');
  });

  it('records error on fetch failure', async () => {
    fm.mockRejectedValue(new Error('x'));
    const { result } = renderHook(() => useStatus());
    await waitFor(() =>
      expect(result.current.error).toMatch(/x/),
    );
  });
});
