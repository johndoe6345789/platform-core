import { renderHook, act, waitFor } from
  '@testing-library/react';
import { useCron } from './useCron';

type FetchMock = jest.Mock<Promise<Response>>;
const resp = (b: unknown): Response =>
  ({
    ok: true, status: 200,
    json: async () => b,
  }) as Response;

const row = {
  id: 1, name: 'nightly',
  cron: '0 0 * * *',
  handler: 'reindex',
  enabled: true,
  next_run_at: '', last_run_at: '',
  description: '',
};

describe('useCron', () => {
  let fm: FetchMock;
  beforeEach(() => {
    fm = jest.fn();
    global.fetch = fm as unknown as typeof fetch;
    jest.useFakeTimers();
  });
  afterEach(() => jest.useRealTimers());

  it('loads schedules on mount', async () => {
    fm.mockResolvedValue(resp({ items: [row] }));
    const { result } = renderHook(() => useCron());
    await waitFor(() =>
      expect(result.current.items).toHaveLength(1),
    );
    expect(fm.mock.calls[0][0]).toContain(
      '/api/cron/schedules',
    );
  });

  it('PUTs toggle with flipped enabled', async () => {
    fm.mockResolvedValue(resp({ items: [row] }));
    const { result } = renderHook(() => useCron());
    await waitFor(() =>
      expect(result.current.items).toHaveLength(1),
    );
    await act(async () => {
      await result.current.toggle(row);
    });
    const call = fm.mock.calls.find(
      c => (c[1] as RequestInit)?.method === 'PUT',
    );
    expect(call).toBeDefined();
    const body = JSON.parse(
      (call![1] as RequestInit).body as string,
    );
    expect(body.enabled).toBe(false);
  });

  it('POSTs fireNow', async () => {
    fm.mockResolvedValue(resp({ items: [] }));
    const { result } = renderHook(() => useCron());
    await act(async () => {
      await result.current.fireNow(7);
    });
    const call = fm.mock.calls.find(
      c => String(c[0]).endsWith('/7/fire'),
    );
    expect(call).toBeDefined();
    expect(
      (call![1] as RequestInit).method,
    ).toBe('POST');
  });
});
