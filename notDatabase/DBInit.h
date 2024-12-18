#ifndef DBINIT_H
#define DBINIT_H

// External declarations for database initialization arrays
extern const char *TxnDBInit[];
extern const char *LedgerDBInit[];
extern const char *WalletDBInit[];
extern const char *HashNodeDBInit[];
extern const char *NetNodeDBInit[];

// External declarations for array sizes
extern int TxnDBCount;
extern int LedgerDBCount;
extern int WalletDBCount;
extern int HashNodeDBCount;
extern int NetNodeDBCount;

#endif // DBINIT_H
