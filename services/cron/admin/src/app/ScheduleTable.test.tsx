import { render, screen } from
  '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { ScheduleTable } from './ScheduleTable';

const row = {
  id: 1, name: 'nightly',
  cron: '0 0 * * *',
  handler: 'reindex', enabled: true,
  next_run_at: 'soon', last_run_at: '',
  description: '',
};

describe('ScheduleTable', () => {
  it('shows empty state', () => {
    render(
      <ScheduleTable
        rows={[]}
        onToggle={jest.fn()}
        onFireNow={jest.fn()}
      />,
    );
    expect(
      screen.getByText(/No schedules/),
    ).toBeInTheDocument();
  });

  it('renders one row per schedule', () => {
    render(
      <ScheduleTable
        rows={[row]}
        onToggle={jest.fn()}
        onFireNow={jest.fn()}
      />,
    );
    expect(
      screen.getByText('nightly'),
    ).toBeInTheDocument();
    expect(
      screen.getByText('0 0 * * *'),
    ).toBeInTheDocument();
  });

  it('calls onToggle when checkbox clicked', async () => {
    const onToggle = jest.fn();
    render(
      <ScheduleTable
        rows={[row]}
        onToggle={onToggle}
        onFireNow={jest.fn()}
      />,
    );
    await userEvent.click(
      screen.getByLabelText('Enable nightly'),
    );
    expect(onToggle).toHaveBeenCalledWith(row);
  });

  it('calls onFireNow with id', async () => {
    const onFireNow = jest.fn();
    render(
      <ScheduleTable
        rows={[row]}
        onToggle={jest.fn()}
        onFireNow={onFireNow}
      />,
    );
    await userEvent.click(
      screen.getByLabelText('Fire nightly now'),
    );
    expect(onFireNow).toHaveBeenCalledWith(1);
  });
});
