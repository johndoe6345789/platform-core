import { render, screen } from
  '@testing-library/react';
import IntegrityBadge from './IntegrityBadge';

describe('IntegrityBadge', () => {
  it('shows loading state', () => {
    render(
      <IntegrityBadge
        report={null}
        loading={true}
      />,
    );
    expect(
      screen.getByLabelText('verifying integrity'),
    ).toBeInTheDocument();
  });

  it('shows idle state', () => {
    render(
      <IntegrityBadge
        report={null}
        loading={false}
      />,
    );
    expect(
      screen.getByLabelText('integrity not checked'),
    ).toBeInTheDocument();
  });

  it('renders ok chain', () => {
    render(
      <IntegrityBadge
        loading={false}
        report={{
          ok: true, rows_checked: 5,
          message: '',
        }}
      />,
    );
    expect(
      screen.getByText(/Chain OK \(5 rows\)/),
    ).toBeInTheDocument();
  });

  it('renders broken chain', () => {
    render(
      <IntegrityBadge
        loading={false}
        report={{
          ok: false, rows_checked: 5,
          first_bad_id: 3, message: 'bad',
        }}
      />,
    );
    expect(
      screen.getByText(/broken @ 3/),
    ).toBeInTheDocument();
  });
});
