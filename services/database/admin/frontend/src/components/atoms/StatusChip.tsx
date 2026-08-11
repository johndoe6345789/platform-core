/** @brief Status chip for labels and badges. */

/** @brief Props for StatusChip. */
interface StatusChipProps {
  label: string;
  variant?: 'info' | 'success' | 'warning' | 'error';
}

/** @brief Small labeled chip with color. */
export default function StatusChip(
  { label, variant = 'info' }: StatusChipProps,
) {
  return (
    <span
      className={`chip chip--${variant}`}
      data-testid="status-chip"
      aria-label={label}
    >
      {label}
    </span>
  );
}
