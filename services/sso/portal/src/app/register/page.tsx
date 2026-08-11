import SsoRegisterForm from '@/components/SsoRegisterForm';
import { safeNext } from '@/lib/safeNext';

interface RegisterPageProps {
  searchParams: Promise<{ next?: string }>;
}

/**
 * SSO registration page. Creates a new account
 * and auto-logs-in, then redirects to `next`.
 *
 * @param props - Page props with searchParams.
 */
export default async function RegisterPage({
  searchParams,
}: RegisterPageProps) {
  const { next: raw } = await searchParams;
  const next = safeNext(raw);

  return (
    <div className="card">
      <p className="logo">NextExtra</p>
      <p className="subtitle">Create an account</p>
      <SsoRegisterForm next={next} />
    </div>
  );
}
