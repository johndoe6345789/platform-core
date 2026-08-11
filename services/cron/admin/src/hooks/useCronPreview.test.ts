import { renderHook, act, waitFor } from
  '@testing-library/react';
import { useCronPreview } from './useCronPreview';

type FetchMock = jest.Mock<Promise<Response>>;
const ok = (b: unknown): Response =>
  ({ ok: true, status: 200, json: async () => b })
    as Response;
const bad = (b: unknown): Response =>
  ({ ok: false, status: 400, json: async () => b })
    as Response;

describe('useCronPreview', () => {
  let fm: FetchMock;
  beforeEach(() => {
    jest.useFakeTimers();
    fm = jest.fn();
    global.fetch = fm as unknown as typeof fetch;
  });
  afterEach(() => jest.useRealTimers());

  it('skips fetch on empty expression', () => {
    renderHook(() => useCronPreview(''));
    expect(fm).not.toHaveBeenCalled();
  });

  it('debounces and returns fire list', async () => {
    fm.mockResolvedValue(ok({ next: [1, 2, 3] }));
    const { result } = renderHook(() =>
      useCronPreview('* * * * *'),
    );
    await act(async () => {
      jest.advanceTimersByTime(400);
    });
    await waitFor(() =>
      expect(result.current.fires).toEqual([1, 2, 3]),
    );
    const body = JSON.parse(
      (fm.mock.calls[0][1] as RequestInit)
        .body as string,
    );
    expect(body.cron).toBe('* * * * *');
  });

  it('records error from backend', async () => {
    fm.mockResolvedValue(bad({ error: 'bad' }));
    const { result } = renderHook(() =>
      useCronPreview('zzz'),
    );
    await act(async () => {
      jest.advanceTimersByTime(400);
    });
    await waitFor(() =>
      expect(result.current.error).toBe('bad'),
    );
  });
});
