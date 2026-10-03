import { NextResponse } from 'next/server';
import { publicOrigin } from '@/lib/publicOrigin';

export const dynamic = 'force-dynamic';

/**
 * GET /sso/logout
 * Clears the nextra_sso cookie and redirects
 * to the SSO login page.
 */
export function GET() {
  const res = NextResponse.redirect(
    new URL(
      '/sso/login',
      publicOrigin(),
    ),
  );
  res.cookies.set('nextra_sso', '', {
    httpOnly: true,
    path: '/',
    maxAge: 0,
  });
  return res;
}
