import SsoForgotForm from '@/components/SsoForgotForm';
import { safeNext } from '@/lib/safeNext';

interface ForgotPageProps {
  searchParams: Promise<{ next?: string }>;
}

/**
 * SSO forgot-password page. Collects the user's
 * email and triggers a reset link via the API.
 *
 * @param props - Page props with searchParams.
 */
export default async function ForgotPage({
  searchParams,
}: ForgotPageProps) {
  const { next: raw } = await searchParams;
  const next = safeNext(raw);

  return (
    <div className="card">
      <p className="logo">NextExtra</p>
      <p className="subtitle">Reset your password</p>
      <SsoForgotForm next={next} />
    </div>
  );
}
