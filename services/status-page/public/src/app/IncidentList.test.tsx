const useIncidents = jest.fn();
jest.mock('@/hooks/useIncidents', () => ({
  useIncidents: () => useIncidents(),
}));

import { render, screen } from
  '@testing-library/react';
import { IncidentList } from './IncidentList';

describe('IncidentList', () => {
  it('renders the empty message when no rows', () => {
    useIncidents.mockReturnValue({
      active: [], history: [], error: null,
    });
    render(<IncidentList />);
    expect(
      screen.getByText('No incidents reported.'),
    ).toBeTruthy();
  });

  it('renders active incidents', () => {
    useIncidents.mockReturnValue({
      active: [{
        id: 1, title: 'DB down', body: 'pg oom',
        severity: 'critical', status: 'investigating',
        started_at: '',
      }],
      history: [],
      error: null,
    });
    render(<IncidentList />);
    expect(screen.getByText('DB down')).toBeTruthy();
    expect(screen.getByText('critical')).toBeTruthy();
  });

  it('falls back to history when no active', () => {
    useIncidents.mockReturnValue({
      active: [],
      history: [{
        id: 9, title: 'Old', body: '',
        severity: 'minor', status: 'resolved',
        started_at: '',
      }],
      error: null,
    });
    render(<IncidentList />);
    expect(screen.getByText('Old')).toBeTruthy();
  });
});
