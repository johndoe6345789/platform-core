import { render, screen } from
  '@testing-library/react';
import { ServiceCard } from './ServiceCard';

describe('ServiceCard', () => {
  it('shows Operational label when ok', () => {
    render(
      <ServiceCard
        service={{
          name: 'api',
          ok: true,
          latencyMs: 12,
        }}
      />,
    );
    expect(screen.getByText('api')).toBeTruthy();
    expect(
      screen.getByText('Operational'),
    ).toBeTruthy();
    expect(screen.getByText('12 ms')).toBeTruthy();
  });

  it('shows Down label when not ok', () => {
    render(
      <ServiceCard
        service={{ name: 'db', ok: false }}
      />,
    );
    expect(screen.getByText('Down')).toBeTruthy();
  });
});
