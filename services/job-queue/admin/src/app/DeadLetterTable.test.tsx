import { render, screen } from
  '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { DeadLetterTable } from './DeadLetterTable';

const row = {
  id: 9, original_id: 1, name: 'n',
  handler: 'h', attempts: 5,
  last_error: 'boom', failed_at: 'then',
};

describe('DeadLetterTable', () => {
  it('shows empty', () => {
    render(
      <DeadLetterTable
        rows={[]}
        onRetry={jest.fn()}
      />,
    );
    expect(
      screen.getByText(/empty/),
    ).toBeInTheDocument();
  });

  it('requeues by id', async () => {
    const cb = jest.fn().mockResolvedValue(true);
    render(
      <DeadLetterTable rows={[row]} onRetry={cb} />,
    );
    await userEvent.click(
      screen.getByLabelText('Requeue job 9'),
    );
    expect(cb).toHaveBeenCalledWith(9);
  });
});
