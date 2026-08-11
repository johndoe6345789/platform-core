import { render, screen } from
  '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { RunsTable } from './RunsTable';

const row = {
  id: 'r1', started_at: 's',
  finished_at: null, status: 'ok',
  bytes: '10', s3_key: 'k/1',
  error: null, kind: 'full',
};

describe('RunsTable', () => {
  it('renders empty', () => {
    render(
      <RunsTable rows={[]} onRestore={jest.fn()} />,
    );
    expect(
      screen.getByText(/No backup runs/),
    ).toBeInTheDocument();
  });

  it('fires restore with key', async () => {
    const cb = jest.fn();
    render(
      <RunsTable rows={[row]} onRestore={cb} />,
    );
    await userEvent.click(
      screen.getByLabelText('Restore backup r1'),
    );
    expect(cb).toHaveBeenCalledWith('k/1');
  });
});
