// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Copyright (c) 2017-2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_VALIDATIONINTERFACE_H
#define BITCOIN_VALIDATIONINTERFACE_H

#include "optional.h"
#include "sapling/incrementalmerkletree.h"

#include <boost/signals2/signal.hpp>

class CBlock;
struct CBlockLocator;
class CBlockIndex;
class CConnman;
class CReserveScript;
class CTransaction;
class CValidationInterface;
class CValidationState;
class uint256;

#include <functional>
using namespace boost::placeholders;

// These functions dispatch to one or all registered wallets

/** Register a wallet to receive updates from core */
void RegisterValidationInterface(CValidationInterface* pwalletIn);
/** Unregister a wallet from core */
void UnregisterValidationInterface(CValidationInterface* pwalletIn);
/** Unregister all wallets from core */
void UnregisterAllValidationInterfaces();

class CValidationInterface {
protected:
    virtual void UpdatedBlockTip(const CBlockIndex *pindexNew, const CBlockIndex *pindexFork, bool fInitialDownload) {}
    virtual void SyncTransaction(const CTransaction &tx, const CBlockIndex *pindex, int posInBlock) {}
    virtual void ChainTip(const CBlockIndex *pindex, const CBlock *pblock, Optional<SaplingMerkleTree> added) {}
    virtual void NotifyTransactionLock(const CTransaction &tx) {}
    virtual void SetBestChain(const CBlockLocator &locator) {}
    virtual bool UpdatedTransaction(const uint256 &hash) { return false;}
    virtual void ResendWalletTransactions(CConnman* connman) {}
    virtual void BlockChecked(const CBlock&, const CValidationState&) {}
    virtual void ResetRequestCount(const uint256 &hash) {};
    friend void ::RegisterValidationInterface(CValidationInterface*);
    friend void ::UnregisterValidationInterface(CValidationInterface*);
    friend void ::UnregisterAllValidationInterfaces();
};

struct CMainSignals {
// XX42    boost::signals2::signal<void(const uint256&)> EraseTransaction;
    /** Notifies listeners of updated block chain tip */
    boost::signals2::signal<void (const CBlockIndex *, const CBlockIndex *, bool fInitialDownload)> UpdatedBlockTip;
    /** A posInBlock value for SyncTransaction which indicates the transaction was conflicted, disconnected, or not in a block */
    static const int SYNC_TRANSACTION_NOT_IN_BLOCK = -1;
    /** Notifies listeners of updated transaction data (transaction, and optionally the block it is found in. */
    boost::signals2::signal<void (const CTransaction &, const CBlockIndex *pindex, int posInBlock)> SyncTransaction;
    /** Notifies listeners of an updated transaction lock without new data. */
    boost::signals2::signal<void (const CTransaction &)> NotifyTransactionLock;
    /** Notifies listeners of an updated transaction without new data (for now: a coinbase potentially becoming visible). */
    boost::signals2::signal<bool (const uint256 &)> UpdatedTransaction;
    /** Notifies listeners of a new active block chain. */
    boost::signals2::signal<void (const CBlockLocator &)> SetBestChain;
    /** Tells listeners to broadcast their data. */
    boost::signals2::signal<void (CConnman* connman)> Broadcast;
    /** Notifies listeners of a block validation result */
    boost::signals2::signal<void (const CBlock&, const CValidationState&)> BlockChecked;
    /** Notifies listeners that a block has been successfully mined */
    boost::signals2::signal<void (const uint256 &)> BlockFound;

    /** Notifies listeners of a change to the tip of the active block chain. */
    boost::signals2::signal<void (const CBlockIndex *, const CBlock *, Optional<SaplingMerkleTree>)> ChainTip;
};

CMainSignals& GetMainSignals();

#endif // BITCOIN_VALIDATIONINTERFACE_H
