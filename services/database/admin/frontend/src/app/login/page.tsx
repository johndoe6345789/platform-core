'use client';

import LoginForm from '@/components/organisms/LoginForm';
import { useSession } from '@/hooks/useSession';

/** @brief Login page. */
export default function LoginPage() {
  const { login } = useSession();
  return <LoginForm onLogin={login} />;
}
