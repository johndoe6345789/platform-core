'use client';

/** @brief Props for ConfirmDialog. */
interface ConfirmDialogProps {
  open: boolean;
  title: string;
  message: string;
  onConfirm: () => void;
  onCancel: () => void;
}

/** @brief Modal confirmation dialog. */
export default function ConfirmDialog(
  {
    open, title, message, onConfirm, onCancel,
  }: ConfirmDialogProps,
) {
  if (!open) return null;

  return (
    <div
      className="dialog-overlay"
      data-testid="confirm-dialog"
    >
      <div
        className="dialog"
        role="dialog"
        aria-label={title}
        aria-modal="true"
      >
        <h3>{title}</h3>
        <p>{message}</p>
        <div className="dialog-actions">
          <button
            onClick={onCancel}
            data-testid="dialog-cancel"
            aria-label="Cancel"
          >
            Cancel
          </button>
          <button
            onClick={onConfirm}
            className="btn-danger"
            data-testid="dialog-confirm"
            aria-label="Confirm"
          >
            Confirm
          </button>
        </div>
      </div>
    </div>
  );
}
