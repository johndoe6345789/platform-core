import { render, screen } from
  '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { RestoreDialog } from './RestoreDialog';

describe('RestoreDialog', () => {
  it('renders nothing when no key', () => {
    const { container } = render(
      <RestoreDialog
        s3Key={null}
        onConfirm={jest.fn()}
        onCancel={jest.fn()}
      />,
    );
    expect(container).toBeEmptyDOMElement();
  });

  it('confirms with the key', async () => {
    const cb = jest.fn();
    render(
      <RestoreDialog
        s3Key="k/1"
        onConfirm={cb}
        onCancel={jest.fn()}
      />,
    );
    await userEvent.click(
      screen.getByLabelText('Confirm restore'),
    );
    expect(cb).toHaveBeenCalledWith('k/1');
  });

  it('cancels', async () => {
    const cb = jest.fn();
    render(
      <RestoreDialog
        s3Key="k/1"
        onConfirm={jest.fn()}
        onCancel={cb}
      />,
    );
    await userEvent.click(
      screen.getByLabelText('Cancel restore'),
    );
    expect(cb).toHaveBeenCalled();
  });
});
