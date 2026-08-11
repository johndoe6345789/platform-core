import { renderHook, act } from
  '@testing-library/react';
import { useRestore } from './useRestore';

type FM = jest.Mock<Promise<Response>>;
const ok = (b: unknown): Response =>
  ({ ok: true, status: 200, json: async () => b })
    as Response;

describe('useRestore', () => {
  let fm: FM;
  beforeEach(() => {
    fm = jest.fn().mockResolvedValue(ok({ id: 1 }));
    global.fetch = fm as unknown as typeof fetch;
  });

  it('triggers and calls onDone', async () => {
    const done = jest.fn();
    const { result } = renderHook(() =>
      useRestore(done),
    );
    await act(async () => {
      await result.current.trigger('full');
    });
    const [url, init] = fm.mock.calls[0];
    expect(String(url)).toContain('/trigger');
    expect((init as RequestInit).method).toBe('POST');
    expect(
      JSON.parse((init as RequestInit).body as string),
    ).toEqual({ kind: 'full' });
    expect(done).toHaveBeenCalled();
  });

  it('restore posts s3 key', async () => {
    const done = jest.fn();
    const { result } = renderHook(() =>
      useRestore(done),
    );
    await act(async () => {
      await result.current.restore('k/1');
    });
    const init =
      fm.mock.calls[0][1] as RequestInit;
    expect(
      JSON.parse(init.body as string),
    ).toEqual({ s3_key: 'k/1' });
    expect(done).toHaveBeenCalled();
  });
});
