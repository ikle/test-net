/*
 * RPC Accounting
 *
 * Copyright (c) 2026 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef RPC_ACCOUNT_H
#define RPC_ACCOUNT_H  1

int rpc_pty_open   (void);
int rpc_pty_close  (int dev);
int rpc_pty_login  (int dev, const char *user, const char *host);
int rpc_pty_logout (int dev);

#endif  /* RPC_ACCOUNT_H */
