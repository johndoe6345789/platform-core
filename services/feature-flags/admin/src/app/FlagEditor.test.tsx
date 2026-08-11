import { render, screen } from
  '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { FlagEditor } from './FlagEditor';

const f = {
  id: 1, key: 'beta', description: 'old',
  enabled: true, rollout_pct: 10,
  targeting: {}, updated_at: '',
};

describe('FlagEditor', () => {
  it('renders null when flag null', () => {
    const { container } = render(
      <FlagEditor
        flag={null}
        open={true}
        onClose={jest.fn()}
        onSave={jest.fn()}
      />,
    );
    expect(container).toBeEmptyDOMElement();
  });

  it('passes current values to save', async () => {
    const onSave = jest.fn();
    render(
      <FlagEditor
        flag={f}
        open={true}
        onClose={jest.fn()}
        onSave={onSave}
      />,
    );
    await userEvent.click(
      screen.getByRole('button', { name: 'Save' }),
    );
    expect(onSave).toHaveBeenCalledWith({
      description: 'old',
      enabled: true,
      rollout_pct: 10,
    });
  });

  it('cancel fires onClose', async () => {
    const onClose = jest.fn();
    render(
      <FlagEditor
        flag={f}
        open={true}
        onClose={onClose}
        onSave={jest.fn()}
      />,
    );
    await userEvent.click(
      screen.getByRole('button', { name: 'Cancel' }),
    );
    expect(onClose).toHaveBeenCalled();
  });
});
