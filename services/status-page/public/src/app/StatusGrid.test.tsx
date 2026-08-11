const useStatus = jest.fn();
jest.mock('@/hooks/useStatus', () => ({
  useStatus: () => useStatus(),
}));

import { render, screen } from
  '@testing-library/react';
import { StatusGrid } from './StatusGrid';

describe('StatusGrid', () => {
  it('renders empty state', () => {
    useStatus.mockReturnValue({
      services: [], loading: false, error: null,
    });
    render(<StatusGrid />);
    expect(
      screen.getByText('No services reporting yet.'),
    ).toBeTruthy();
  });

  it('renders service rows', () => {
    useStatus.mockReturnValue({
      services: [{ name: 'api', ok: true }],
      loading: false,
      error: null,
    });
    render(<StatusGrid />);
    expect(screen.getByText('api')).toBeTruthy();
  });
});
