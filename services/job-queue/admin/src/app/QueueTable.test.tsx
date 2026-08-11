import { render, screen } from
  '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { QueueTable } from './QueueTable';

const row = {
  id: 1, name: 'n', handler: 'h',
  priority: 50, status: 'pending',
  attempts: 0, max_attempts: 3,
  run_at: 'soon', locked_by: null,
};

describe('QueueTable', () => {
  it('shows empty', () => {
    render(
      <QueueTable rows={[]} onCancel={jest.fn()} />,
    );
    expect(
      screen.getByText(/No queued jobs/),
    ).toBeInTheDocument();
  });

  it('cancels pending row', async () => {
    const cb = jest.fn().mockResolvedValue(true);
    render(
      <QueueTable rows={[row]} onCancel={cb} />,
    );
    await userEvent.click(
      screen.getByLabelText('Cancel job 1'),
    );
    expect(cb).toHaveBeenCalledWith(1);
  });

  it('hides cancel for running', () => {
    render(
      <QueueTable
        rows={[{ ...row, status: 'running' }]}
        onCancel={jest.fn()}
      />,
    );
    expect(
      screen.queryByLabelText('Cancel job 1'),
    ).toBeNull();
  });
});
