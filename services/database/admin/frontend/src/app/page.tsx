import { redirect } from 'next/navigation';

/** @brief Root page redirects to dashboard. */
export default function Home() {
  redirect('/dashboard');
}
