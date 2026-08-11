'use client'

/**
 * M3 Slider wrapper for the rollout % control.
 */

import { Slider, Typography, Box } from '@shared/m3'

interface Props {
  value: number
  onChange: (next: number) => void
  disabled?: boolean
}

export function RolloutSlider({
  value, onChange, disabled,
}: Props) {
  return (
    <Box data-testid="rollout-slider">
      <Typography variant="caption">
        Rollout percentage: {value}%
      </Typography>
      <Slider
        value={value}
        min={0}
        max={100}
        step={1}
        disabled={disabled}
        aria-label="Rollout percentage"
        onChange={(_e, v) =>
          onChange(
            Array.isArray(v) ? v[0] : v,
          )
        }
      />
    </Box>
  )
}
