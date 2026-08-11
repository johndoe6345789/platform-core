'use client';

import '@scss/globals.scss';
import '@scss/pgadmin.scss';

/** @brief Root layout with M3 SCSS theme. */
export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html lang="en">
      <body>{children}</body>
    </html>
  );
}
