import { render, screen, fireEvent } from
  '@testing-library/react';
import { RolloutSlider } from './RolloutSlider';

describe('RolloutSlider', () => {
  it('shows current value label', () => {
    render(
      <RolloutSlider value={42} onChange={jest.fn()} />,
    );
    expect(
      screen.getByText(/42%/),
    ).toBeInTheDocument();
  });

  it('fires onChange with new number', () => {
    const cb = jest.fn();
    render(
      <RolloutSlider value={10} onChange={cb} />,
    );
    fireEvent.change(
      screen.getByLabelText('Rollout percentage'),
      { target: { value: '55' } },
    );
    expect(cb).toHaveBeenCalledWith(55);
  });

  it('respects disabled', () => {
    render(
      <RolloutSlider
        value={5}
        disabled
        onChange={jest.fn()}
      />,
    );
    expect(
      screen.getByLabelText('Rollout percentage'),
    ).toBeDisabled();
  });
});
