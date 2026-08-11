jest.mock('@/hooks/useFlagAudit', () => ({
  useFlagAudit: (key: string | null) => ({
    audit: key
      ? [
          {
            actor: 'u', prev: {}, next: { x: 1 },
            at: 't',
          },
        ]
      : [],
    loading: false,
    refresh: jest.fn(),
  }),
}));

import { render, screen } from
  '@testing-library/react';
import { AuditDrawer } from './AuditDrawer';

describe('AuditDrawer', () => {
  it('shows audit row when open', () => {
    render(
      <AuditDrawer
        flagKey="beta"
        open={true}
        onClose={jest.fn()}
      />,
    );
    expect(
      screen.getByText(/beta/),
    ).toBeInTheDocument();
    expect(
      screen.getByText(/"x": 1/),
    ).toBeInTheDocument();
  });

  it('shows empty when no rows', () => {
    render(
      <AuditDrawer
        flagKey={null}
        open={true}
        onClose={jest.fn()}
      />,
    );
    expect(
      screen.getByText(/No audit entries/),
    ).toBeInTheDocument();
  });
});
