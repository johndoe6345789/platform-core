/** @brief Loading spinner atom. */

/** @brief Props for Spinner. */
interface SpinnerProps {
  size?: 'sm' | 'md' | 'lg';
}

/** @brief Animated loading indicator. */
export default function Spinner(
  { size = 'md' }: SpinnerProps,
) {
  const px =
    size === 'sm' ? 16 : size === 'lg' ? 48 : 24;
  return (
    <div
      data-testid="spinner"
      aria-label="Loading"
      style={{
        width: px,
        height: px,
        border: '3px solid var(--md-sys-color-outline)',
        borderTop: '3px solid var(--md-sys-color-primary)',
        borderRadius: '50%',
        animation: 'spin 0.8s linear infinite',
      }}
    />
  );
}
