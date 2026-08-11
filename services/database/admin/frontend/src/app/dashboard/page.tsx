'use client';

import { useEffect, useState } from 'react';
import { useRouter } from 'next/navigation';
import { useSession } from '@/hooks/useSession';
import { AppShell } from '@/components/organisms';
import TablesPanel
  from '@/components/organisms/TablesPanel';
import SQLQueryPanel
  from '@/components/organisms/SQLQueryPanel';
import QueryBuilderPanel
  from '@/components/organisms/QueryBuilderPanel';
import TableManagerPanel
  from '@/components/organisms/TableManagerPanel';
import ColumnManagerPanel
  from '@/components/organisms/ColumnManagerPanel';
import ConstraintPanel
  from '@/components/organisms/ConstraintPanel';
import IndexPanel
  from '@/components/organisms/IndexPanel';

/** @brief Main dashboard page with tab routing. */
export default function DashboardPage() {
  const router = useRouter();
  const { session, ready, logout } = useSession();
  const [tab, setTab] = useState('tables');

  useEffect(() => {
    if (ready && !session.isAuthenticated) {
      router.push('/login');
    }
  }, [ready, session.isAuthenticated, router]);

  const handleLogout = () => {
    logout();
    router.push('/login');
  };

  if (!ready || !session.isAuthenticated) {
    return null;
  }

  return (
    <AppShell
      activeTab={tab}
      onTabChange={setTab}
      onLogout={handleLogout}
    >
      {tab === 'tables' && <TablesPanel />}
      {tab === 'sql' && <SQLQueryPanel />}
      {tab === 'builder' && <QueryBuilderPanel />}
      {tab === 'manage' && <TableManagerPanel />}
      {tab === 'columns' && <ColumnManagerPanel />}
      {tab === 'constraints' && (
        <ConstraintPanel />
      )}
      {tab === 'indexes' && <IndexPanel />}
    </AppShell>
  );
}
