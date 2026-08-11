'use client'

/**
 * RestoreDialog — confirmation modal shown
 * when the operator clicks "Restore" on a
 * runs-table row.  Destructive action needs
 * a human-in-the-loop ack before POSTing
 * /api/backups/restore.
 */

interface Props {
  s3Key: string | null
  onConfirm: (key: string) => void
  onCancel: () => void
}

export function RestoreDialog(
  { s3Key, onConfirm, onCancel }: Props,
) {
  if (!s3Key) return null

  return (
    <div
      className="backups-dialog"
      role="dialog"
      aria-label="Confirm restore"
      data-testid="restore-dialog"
    >
      <h2>Restore backup?</h2>
      <p>
        This will queue a restore from{' '}
        <code>{s3Key}</code>. An operator
        must still apply the dump manually
        after this request is recorded.
      </p>
      <div className="backups-toolbar">
        <button
          type="button"
          data-testid="restore-confirm"
          aria-label="Confirm restore"
          onClick={() => onConfirm(s3Key)}
        >
          Confirm
        </button>
        <button
          type="button"
          data-testid="restore-cancel"
          aria-label="Cancel restore"
          onClick={onCancel}
        >
          Cancel
        </button>
      </div>
    </div>
  )
}
