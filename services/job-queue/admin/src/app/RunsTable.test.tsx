import { render, screen } from
  '@testing-library/react';
import { RunsTable } from './RunsTable';

const row = {
  id: 1, job_queue_id: 1, name: 'n',
  handler: 'h', attempt: 1,
  status: 'ok', started_at: 's',
  finished_at: 'f', duration_ms: 250,
  worker_id: 'w1',
};

describe('RunsTable', () => {
  it('shows empty', () => {
    render(<RunsTable rows={[]} />);
    expect(
      screen.getByText(/No run history/),
    ).toBeInTheDocument();
  });

  it('renders row with duration', () => {
    render(<RunsTable rows={[row]} />);
    expect(
      screen.getByText('250 ms'),
    ).toBeInTheDocument();
    expect(screen.getByText('w1')).toBeInTheDocument();
  });
});
