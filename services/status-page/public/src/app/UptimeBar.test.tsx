import { render, screen } from
  '@testing-library/react';
import { UptimeBar } from './UptimeBar';

describe('UptimeBar', () => {
  it('defaults to 90 cells when no days given', () => {
    render(<UptimeBar />);
    const bar = screen.getByRole('img');
    expect(bar.children.length).toBe(90);
    expect(bar.getAttribute('aria-label'))
      .toBe('Uptime for last 90 days');
  });

  it('uses provided days', () => {
    render(
      <UptimeBar days={['up', 'degraded', 'down']} />,
    );
    const bar = screen.getByRole('img');
    expect(bar.children.length).toBe(3);
  });
});
