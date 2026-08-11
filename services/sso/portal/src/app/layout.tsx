import type { ReactNode } from 'react';
import '@/styles/globals.css';

export const metadata = {
  title: 'Sign In — NextExtra',
};

/** Root layout for the SSO app. */
export default function RootLayout({
  children,
}: {
  children: ReactNode;
}) {
  return (
    <html lang="en">
      <body>{children}</body>
    </html>
  );
}
