import { render, screen } from
  '@testing-library/react';
import { PolicyEditor } from './PolicyEditor';

const row = {
  id: '1', name: 'daily', kind: 'full',
  retain_days: '30',
  schedule_cron: '0 2 * * *',
};

describe('PolicyEditor', () => {
  it('shows empty state', () => {
    render(<PolicyEditor rows={[]} />);
    expect(
      screen.getByText(/No policies/),
    ).toBeInTheDocument();
  });

  it('renders policy row', () => {
    render(<PolicyEditor rows={[row]} />);
    expect(screen.getByText('daily')).toBeInTheDocument();
    expect(screen.getByText('0 2 * * *'))
      .toBeInTheDocument();
  });
});
