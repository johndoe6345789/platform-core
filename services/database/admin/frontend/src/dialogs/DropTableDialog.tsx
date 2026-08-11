'use client';

/** @brief Props for DropTableDialog. */
interface Props {
  open: boolean;
  tableName: string;
  onConfirm: () => void;
  onCancel: () => void;
}

/** @brief Confirmation dialog to drop a table. */
export default function DropTableDialog(
  { open, tableName, onConfirm, onCancel }: Props,
) {
  if (!open) return null;

  return (
    <div
      className="dialog-overlay"
      data-testid="drop-table-dialog"
    >
      <div
        className="dialog"
        role="dialog"
        aria-label="Drop Table"
        aria-modal="true"
      >
        <h3>Drop Table</h3>
        <p>
          Are you sure you want to drop{' '}
          <strong>{tableName}</strong>?
          This action cannot be undone.
        </p>
        <div className="dialog-actions">
          <button
            onClick={onCancel}
            aria-label="Cancel"
            data-testid="cancel-drop-table"
          >
            Cancel
          </button>
          <button
            onClick={onConfirm}
            className="btn-danger"
            data-testid="confirm-drop-table"
            aria-label="Drop table"
          >
            Drop Table
          </button>
        </div>
      </div>
    </div>
  );
}
