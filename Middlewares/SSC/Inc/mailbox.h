/**
 * \addtogroup Mailbox Mailbox Functions
 * @{
 */

/**
\file mailbox.h
\author EthercatSSC@beckhoff.com

\version 5.11

<br>Changes to version V5.01:<br>
V5.11 ECAT10: change PROTO handling to prevent compiler errors<br>
V5.11 MBX1: "return value of ""MBX_StartMailboxHandler()"" changed to UINT16"<br>
V5.11 MBX3: set application triggered emergency and EoE data to pending if no mailbox queue is supported and another mailbox request is currently handled, Handle only one mailbox request at a time (in case that MAILBPX_QUEUE is disabled)<br>
<br>Changes to version - :<br>
V5.01 : Start file change log
 */

#ifndef _MAILBOX_H_
#define _MAILBOX_H_

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"


/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/
#define     MBX_TYPE_AOE                        1 /**< \brief Mailbox type AoE*/
#define     MBX_TYPE_EOE                        2 /**< \brief Mailbox type EoE*/
#define     MBX_TYPE_COE                        3 /**< \brief Mailbox type CoE*/
#define     MBX_TYPE_FOE                        4 /**< \brief Mailbox type FoE*/
#define     MBX_TYPE_SOE                        5 /**< \brief Mailbox type SoE*/
#define     MBX_TYPE_VOE                        15 /**< \brief Mailbox type VoE*/

#define     EMCY_SERVICE                        ((UINT8) 0x0001) /**< \brief Emergency service*/
#define     COE_SERVICE                         ((UINT8) 0x0002) /**< \brief CoE service*/
#define     SOE_SERVICE                         ((UINT8) 0x0004) /**< \brief SoE service*/
#define     EOE_SERVICE                         ((UINT8) 0x0008) /**< \brief EoE service*/
#define     AOE_SERVICE                         ((UINT8) 0x0010) /**< \brief AoE service*/
#define     VOE_SERVICE                         ((UINT8) 0x0020) /**< \brief VoE service*/
#define     FOE_SERVICE                         ((UINT8) 0x0040) /**< \brief FoE service*/
#define     FRAGMENTS_FOLLOW                    ((UINT8) 0x0080) /**< \brief Fragments follow service*/

#ifndef DISABLE_MBX_INT
    #define    DISABLE_MBX_INT /**< \brief Disable mailbox interrupt*/
#endif
#ifndef ENABLE_MBX_INT
    #define    ENABLE_MBX_INT /**< \brief Enable mailbox interrupt*/
#endif

#ifndef    ENTER_MBX_CRITICAL
    #define    ENTER_MBX_CRITICAL /**< \brief Enter mailbox critical section*/
#endif

#ifndef    LEAVE_MBX_CRITICAL
    #define    LEAVE_MBX_CRITICAL /**< \brief Leave mailbox critical section*/
#endif

#if MAILBOX_QUEUE
#ifndef    MAX_MBX_QUEUE_SIZE
    #define    MAX_MBX_QUEUE_SIZE    10 /**< \brief Mailbox queue size*/
#endif
#endif


/*---------------------------------------------
-    Command Codes for the mailbox type 0
-----------------------------------------------*/
#define    MBXSERVICE_MBXERRORCMD          0x01 /**< \brief Mailbox error command*/


/*---------------------------------------------
-    Error Codes for a mailbox error response
-----------------------------------------------*/
#define    MBXERR_SYNTAX                   0x01 /**< \brief Mailbox error "syntax"*/
#define    MBXERR_UNSUPPORTEDPROTOCOL      0x02 /**< \brief Mailbox error "unsupported protocol"*/
#define    MBXERR_INVALIDCHANNEL           0x03 /**< \brief Mailbox error "invalid channel"*/
#define    MBXERR_SERVICENOTSUPPORTED      0x04 /**< \brief Mailbox error "service not supported"*/
#define    MBXERR_INVALIDHEADER            0x05 /**< \brief Mailbox error "invalid header"*/
#define    MBXERR_SIZETOOSHORT             0x06 /**< \brief Mailbox error "Size too short"*/
#define    MBXERR_NOMOREMEMORY             0x07 /**< \brief Mailbox error "No memory"*/
#define    MBXERR_INVALIDSIZE              0x08 /**< \brief Mailbox error "Invalid size"*/
#define    MBXERR_SERVICEINWORK            0x09 /**< \brief Mailbox error "Service in work"*/


/**
 * \brief Mailbox header
 */
typedef struct MBX_STRUCT_PACKED_START
{
    UINT16                          Length; /**< \brief Length*/
    UINT16                          Address; /**< \brief Address*/

#if MBX_16BIT_ACCESS
    UINT16                          Flags[1]; /**< \brief Flags*/
    #define    MBX_OFFS_TYPE        0 /**< \brief Protocol type offset*/
    #define    MBX_MASK_TYPE        0x0F00 /**< \brief Protocol type mask*/
    #define    MBX_SHIFT_TYPE       8 /**< \brief Protocol type shift*/
    #define    MBX_OFFS_COUNTER     0 /**< \brief Protocol counter offset*/
    #define    MBX_MASK_COUNTER     0xF000 /**< \brief Protocol counter mask*/
    #define    MBX_SHIFT_COUNTER    12 /**< \brief Protocol counter shift*/
#else
    UINT8                           Flags[2]; /**< \brief Flags*/
#if BIG_ENDIAN_16BIT
    #define    MBX_OFFS_TYPE        0 /**< \brief Protocol type offset*/
    #define    MBX_OFFS_COUNTER     0 /**< \brief Protocol counter offset*/
#else
    #define    MBX_OFFS_TYPE        1 /**< \brief Protocol type offset*/
    #define    MBX_OFFS_COUNTER     1 /**< \brief Protocol counter offset*/
#endif
    #define    MBX_MASK_TYPE        0x0F /**< \brief Protocol type mask*/
    #define    MBX_MASK_COUNTER     0xF0 /**< \brief Protocol counter mask*/
    #define    MBX_SHIFT_TYPE       0 /**< \brief Protocol type shift*/
    #define    MBX_SHIFT_COUNTER    4 /**< \brief Protocol counter shift*/
#endif
}MBX_STRUCT_PACKED_END
TMBXHEADER;


#define     MBX_HEADER_SIZE         SIZEOF(TMBXHEADER) /**< \brief Mailbox header size*/


#define     MAX_MBX_DATA_SIZE       (MAX_MBX_SIZE - MBX_HEADER_SIZE) /**< \brief Mailbox data size*/

/**
 * \brief Mailbox datagram
 */
typedef struct MBX_STRUCT_PACKED_START
{
    TMBXHEADER                      MbxHeader; /**< \brief Mailbox header*/
    UINT16                          Data[(MAX_MBX_DATA_SIZE >> 1)]; /**< \brief Mailbox data*/
}MBX_STRUCT_PACKED_END
TMBX;


#if MAILBOX_QUEUE
/**
 * \brief Mailbox queue
 */
typedef struct
{
    UINT16    firstInQueue; /**< \brief First in Queue*/
    UINT16    lastInQueue; /**< \brief Last in Queue*/
    UINT16    maxQueueSize; /**< \brief Max queue size*/
    TMBX MBXMEM * queue[(MAX_MBX_QUEUE_SIZE)+1]; /**< \brief Queue buffer*/
} TMBXQUEUE;

#endif


#endif //_MAILBOX_H_


/*-----------------------------------------------------------------------------------------
------
------    Global Variables
------
-----------------------------------------------------------------------------------------*/
/* ECATCHANGE_START(V5.11) ECAT10*/
#if defined(_MAILBOX_) && (_MAILBOX_ == 1)
/* ECATCHANGE_END(V5.11) ECAT10*/
    #define PROTO
#else
    #define PROTO extern
#endif

PROTO BOOL                    bReceiveMbxIsLocked; /**< \brief Receive mailbox is locked (MBoxOut, default SM0)*/
PROTO BOOL                    bSendMbxIsFull; /**< \brief Send mailbox is full (MBoxIn, default SM1)*/
PROTO BOOL                    bMbxRunning; /**< \brief Mailbox running (At least PreOP state)*/
PROTO BOOL                    bMbxRepeatToggle; /**< \brief Mailbox repeat toggle*/
PROTO UINT16                  u16SendMbxSize; /**< \brief Send mailbox size (default SM1)*/
PROTO UINT16                  u16ReceiveMbxSize; /**< \brief Receive mailbox size (default SM0)*/
PROTO UINT16                  u16EscAddrReceiveMbx; /**< \brief Receive mailbox address (default SM0)*/
PROTO UINT16                  u16EscAddrSendMbx; /**< \brief Send mailbox address (default SM1)*/
PROTO UINT8                   u8MbxWriteCounter; /**< \brief Write mailbox counter*/
PROTO UINT8                   u8MbxReadCounter; /**< \brief Read mailbox counter*/
PROTO TMBX MBXMEM             asMbx[2]; /**< \brief Mailbox buffer (for repeat service)*/
PROTO UINT8                   u8MailboxSendReqStored; /**< \brief Mailbox send request stored*/
PROTO TMBX MBXMEM *           psWriteMbx; /**< \brief Pointer to write mailbox buffer*/
PROTO TMBX MBXMEM *           psReadMbx; /**< \brief Pointer to read mailbox buffer*/
PROTO TMBX MBXMEM *           psRepeatMbx; /**< \brief Pointer to repeat mailbox buffer*/
PROTO TMBX MBXMEM *           psStoreMbx; /**< \brief Pointer to store mailbox buffer*/
#if !MAILBOX_QUEUE
/*ECATCHANGE_START(V5.11) MBX3*/
PROTO BOOL                    bPendingMbxResponse;   /**indicates if a Mbox is pending (in case that no mailbox queue is supported only one mailbox request after the other shall be handled)*/
/*ECATCHANGE_END(V5.11) MBX3*/
#endif
#if MAILBOX_QUEUE
PROTO TMBXQUEUE MBXMEM        sMbxSendQueue; /**< \brief Send mailbox queue*/
PROTO TMBXQUEUE MBXMEM        sMbxReceiveQueue; /**< \brief Receive mailbox queue*/
#endif


/*-----------------------------------------------------------------------------------------
------
------    Global Functions
------
-----------------------------------------------------------------------------------------*/

PROTO   void     MBX_Init(void);
/* ECATCHANGE_START(V5.11) MBX1*/
PROTO   UINT16    MBX_StartMailboxHandler(void);
/* ECATCHANGE_END(V5.11) MBX1*/
PROTO   void     MBX_StopMailboxHandler(void);
PROTO   void     MBX_MailboxWriteInd(TMBX MBXMEM *pMbx);
PROTO   void     MBX_MailboxReadInd(void);
PROTO   void     MBX_MailboxRepeatReq(void);
PROTO   UINT8    MBX_MailboxSendReq(TMBX MBXMEM * pMbx, UINT8 flags);
PROTO   void     MBX_CheckAndCopyMailbox(void);
PROTO   UINT8    MBX_CopyToSendMailbox(TMBX MBXMEM *pMbx);
PROTO   void     MBX_Main(void);

#undef PROTO
/** @}*/