import { renderHook, act } from
  '@testing-library/react';
import { useJobActions } from './useJobActions';

type FM = jest.Mock<Promise<Response>>;
const ok = (): Response =>
  ({ ok: true, status: 200 }) as Response;

describe('useJobActions', () => {
  let fm: FM;
  let onChange: jest.Mock;
  beforeEach(() => {
    fm = jest.fn().mockResolvedValue(ok());
    global.fetch = fm as unknown as typeof fetch;
    onChange = jest.fn();
  });

  it('POSTs enqueue body', async () => {
    const { result } = renderHook(() =>
      useJobActions(onChange),
    );
    await act(async () => {
      await result.current.enqueue({
        name: 'n', handler: 'h',
      });
    });
    const [url, init] = fm.mock.calls[0];
    expect(String(url)).toContain('/enqueue');
    expect((init as RequestInit).method).toBe('POST');
    expect(onChange).toHaveBeenCalled();
  });

  it('POSTs retry_dead/:id', async () => {
    const { result } = renderHook(() =>
      useJobActions(onChange),
    );
    await act(async () => {
      await result.current.retry(12);
    });
    expect(String(fm.mock.calls[0][0])).toContain(
      '/retry_dead/12',
    );
  });

  it('DELETEs queue/:id', async () => {
    const { result } = renderHook(() =>
      useJobActions(onChange),
    );
    await act(async () => {
      await result.current.cancel(5);
    });
    const [url, init] = fm.mock.calls[0];
    expect(String(url)).toContain('/queue/5');
    expect((init as RequestInit).method).toBe('DELETE');
  });
});
