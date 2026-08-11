import { render, screen } from
  '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { FlagTable } from './FlagTable';

const f = {
  id: 1, key: 'beta',
  description: 'Beta flag',
  enabled: true, rollout_pct: 50,
  targeting: {}, updated_at: '',
};

describe('FlagTable', () => {
  it('shows empty message', () => {
    render(
      <FlagTable
        flags={[]}
        onToggle={jest.fn()}
        onEdit={jest.fn()}
        onAudit={jest.fn()}
      />,
    );
    expect(
      screen.getByText(/No feature flags/),
    ).toBeInTheDocument();
  });

  it('fires toggle on switch click', async () => {
    const onToggle = jest.fn();
    render(
      <FlagTable
        flags={[f]}
        onToggle={onToggle}
        onEdit={jest.fn()}
        onAudit={jest.fn()}
      />,
    );
    await userEvent.click(
      screen.getByLabelText('Toggle beta'),
    );
    expect(onToggle).toHaveBeenCalledWith(f);
  });

  it('fires edit and audit', async () => {
    const onEdit = jest.fn();
    const onAudit = jest.fn();
    render(
      <FlagTable
        flags={[f]}
        onToggle={jest.fn()}
        onEdit={onEdit}
        onAudit={onAudit}
      />,
    );
    await userEvent.click(
      screen.getByRole('button', { name: 'Edit' }),
    );
    await userEvent.click(
      screen.getByRole('button', { name: 'Audit' }),
    );
    expect(onEdit).toHaveBeenCalledWith(f);
    expect(onAudit).toHaveBeenCalledWith(f);
  });
});
