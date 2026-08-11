import { NextResponse } from 'next/server';

/**
 * GET /sso/logout
 * Clears the nextra_sso cookie and redirects
 * to the SSO login page.
 */
export function GET() {
  const res = NextResponse.redirect(
    new URL(
      '/sso/login',
      process.env.NEXT_PUBLIC_ORIGIN
        ?? 'http://localhost:8889',
    ),
  );
  res.cookies.set('nextra_sso', '', {
    httpOnly: true,
    path: '/',
    maxAge: 0,
  });
  return res;
}
