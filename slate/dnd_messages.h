/* Copyright (c) 2012, Dustin Mitchell dmmitche <at> gmail <dot> com
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the document
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __DND_MESSAGES__
#define __DND_MESSAGES__

#include <QtGlobal>

#define SYNC_FIELD 0xdeadbeef

/* Message Types */
#define DND_USER_ADD_REQ 0
#define DND_USER_ADD_RESP 1
#define DND_USER_DEL 2
#define DND_CHAT_MESSAGE 3

#pragma pack(1)

struct DnDMessageHeader
{
  quint16 type;
};

struct DnDUserAddReq
{
  DnDMessageHeader header;
  char name[1];
};

struct DnDUserAddResp
{
  DnDMessageHeader header;
  quint32 uuid;
  char name[1];
};

struct DnDUserDel
{
  DnDMessageHeader header;
  quint32 uuid;
};

/* Chat flags */
#define CHAT_BROADCAST 1

struct DnDChatMessage
{
  DnDMessageHeader header;
  quint8 flags;
  quint32 src_uuid;
  quint32 dst_uuid;
  char message[1];
};

#pragma pack()

#endif /* __DND_MESSAGES__ */
