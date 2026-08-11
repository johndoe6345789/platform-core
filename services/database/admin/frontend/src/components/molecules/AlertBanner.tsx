/** @brief Alert banner for errors and messages. */

/** @brief Props for AlertBanner. */
interface AlertBannerProps {
  message: string;
  variant?: 'error' | 'success' | 'info';
  onDismiss?: () => void;
}

/** @brief Dismissible alert banner. */
export default function AlertBanner(
  { message, variant = 'error', onDismiss }:
    AlertBannerProps,
) {
  return (
    <div
      role="alert"
      className={`alert alert--${variant}`}
      data-testid="alert-banner"
    >
      <span data-testid="alert-message">
        {message}
      </span>
      {onDismiss && (
        <button
          onClick={onDismiss}
          aria-label="Dismiss alert"
          data-testid="alert-dismiss"
        >
          ×
        </button>
      )}
    </div>
  );
}
