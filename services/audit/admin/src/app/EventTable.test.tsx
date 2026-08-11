import { render, screen } from
  '@testing-library/react';
import EventTable from './EventTable';
import type { AuditEvent } from
  '@/hooks/useAudit';

const ev: AuditEvent = {
  id: 7, tenant_id: 't',
  actor_id: 'u', action: 'CREATE',
  target_type: 'post', target_id: '9',
  payload: {},
  prev_hash: '', hash: 'abcdef123456',
  created_at: '2024-01-01T00:00:00Z',
};

describe('EventTable', () => {
  it('renders empty state', () => {
    render(<EventTable events={[]} />);
    expect(
      screen.getByLabelText('no audit events'),
    ).toBeInTheDocument();
  });

  it('renders row cells', () => {
    render(<EventTable events={[ev]} />);
    expect(screen.getByText('CREATE')).toBeInTheDocument();
    expect(screen.getByText('u')).toBeInTheDocument();
    expect(
      screen.getByText('post:9'),
    ).toBeInTheDocument();
    expect(
      screen.getByText(/abcdef123456/),
    ).toBeInTheDocument();
  });
});
