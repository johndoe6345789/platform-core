'use client'

/**
 * Renders next fire-times or an error message
 * for a given cron expression preview.
 * @module CronPreviewPanel
 */

/** Format a Unix-seconds epoch as ISO-like string. */
export function fmtEpoch(secs: number): string {
  try {
    return new Date(secs * 1000)
      .toISOString()
      .replace('T', ' ')
      .substring(0, 19)
  } catch {
    return String(secs)
  }
}

interface CronPreviewPanelProps {
  /** Fires array (epoch seconds). */
  fires: number[]
  /** Error message if expression is invalid. */
  error: string | null
}

/**
 * Panel that shows next fire times or an error.
 * @param props - fires and error from useCronPreview.
 */
export function CronPreviewPanel({
  fires,
  error,
}: CronPreviewPanelProps) {
  return (
    <div
      className="cron-preview"
      data-testid="cron-preview"
      aria-label="Cron preview"
      data-state={error ? 'error' : 'ok'}
    >
      {error ?? (
        fires.length
          ? fires.map(fmtEpoch).join('\n')
          : 'Type a cron expression to preview fire times.'
      )}
    </div>
  )
}
