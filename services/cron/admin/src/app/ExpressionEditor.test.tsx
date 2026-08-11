import { render, screen } from
  '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { ExpressionEditor } from './ExpressionEditor';

jest.mock('@/hooks/useCronPreview', () => ({
  useCronPreview: () => ({
    fires: [], error: null,
  }),
}));

describe('ExpressionEditor', () => {
  beforeEach(() => {
    global.fetch = jest.fn(() =>
      Promise.resolve({
        ok: true, status: 200,
        json: () => Promise.resolve({}),
      }),
    ) as unknown as typeof fetch;
  });

  it('renders the labelled inputs', () => {
    render(
      <ExpressionEditor onCreated={jest.fn()} />,
    );
    expect(
      screen.getByLabelText('schedule name'),
    ).toBeInTheDocument();
    expect(
      screen.getByLabelText('cron expression'),
    ).toBeInTheDocument();
    expect(
      screen.getByLabelText('handler name'),
    ).toBeInTheDocument();
  });

  it('disables save until fields filled', async () => {
    render(
      <ExpressionEditor onCreated={jest.fn()} />,
    );
    const save = screen.getByRole('button', {
      name: 'Save',
    });
    expect(save).toBeDisabled();
    await userEvent.type(
      screen.getByLabelText('schedule name'),
      'n',
    );
    await userEvent.type(
      screen.getByLabelText('cron expression'),
      '* * * * *',
    );
    await userEvent.type(
      screen.getByLabelText('handler name'),
      'h',
    );
    expect(save).toBeEnabled();
  });

  it('POSTs to /api/cron/schedules and calls cb',
    async () => {
      const onCreated = jest.fn();
      render(
        <ExpressionEditor
          onCreated={onCreated}
        />,
      );
      await userEvent.type(
        screen.getByLabelText('schedule name'),
        'n',
      );
      await userEvent.type(
        screen.getByLabelText('cron expression'),
        '* * * * *',
      );
      await userEvent.type(
        screen.getByLabelText('handler name'),
        'h',
      );
      await userEvent.click(
        screen.getByRole('button', { name: 'Save' }),
      );
      const fm = global.fetch as jest.Mock;
      expect(fm).toHaveBeenCalledWith(
        '/api/cron/schedules',
        expect.objectContaining({ method: 'POST' }),
      );
      expect(onCreated).toHaveBeenCalled();
    });
});
