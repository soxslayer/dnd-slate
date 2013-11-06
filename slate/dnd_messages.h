/* Copyright (c) 2013, Dustin Mitchell dmmitche <at> gmail <dot> com
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

#include "image_id.h"

#define COMM_PROTO_MAJOR 0
#define COMM_PROTO_MINOR 3

#define SYNC_FIELD 0xdeadbeef

/* Message Types */
#define DND_COMM_PROTO_REQ 0
#define DND_COMM_PROTO_RESP 1
#define DND_SERVER_MESSAGE 2
#define DND_USER_ADD_REQ 3
#define DND_USER_ADD_RESP 4
#define DND_USER_DEL 5
#define DND_CHAT_MESSAGE 6
#define DND_LOAD_MAP 7
#define DND_REQUEST_IMAGE 8
#define DND_IMAGE_BEGIN 9
#define DND_IMAGE_DATA 10
#define DND_ADD_TILE 11
#define DND_MOVE_TILE 12
#define DND_DELETE_TILE 13
#define DND_PING_PONG 14
#define DND_PING_PONG_RECORD 15
#define DND_IMAGE_QUERY 16

#pragma pack(1)

struct DnDMessageHeader
{
  quint16 type;
};

struct DnDCommProtoReq
{
  DnDMessageHeader header;
};

struct DnDCommProtoResp
{
  DnDMessageHeader header;
  quint16 major;
  quint16 minor;
};

/* Server message flags*/
#define MESSAGE_ERROR 1
#define MESSAGE_WARN 2
#define MESSAGE_INFO 4

struct DnDServerMessage
{
  DnDMessageHeader header;
  quint8 flags;
  char msg[1];
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

struct DnDLoadMap
{
  DnDMessageHeader header;
  quint16 w;
  quint16 h;
  char image_id[ImageId::ID_LEN];
};

struct DnDRequestImage
{
  DnDMessageHeader header;
  char image_id[ImageId::ID_LEN];
};

struct DnDImageBegin
{
  DnDMessageHeader header;
  quint64 total_size;
};

#define DND_IMAGE_MAX_CHUNK_SIZE 1024

struct DnDImageData
{
  DnDMessageHeader header;
  quint32 sequence;
  char data[1];
};

struct DnDAddTile
{
  DnDMessageHeader header;
  quint32 tile_uuid;
  quint8 type;
  quint16 x;
  quint16 y;
  quint16 w;
  quint16 h;
  char text[ImageId::ID_LEN];
};

struct DnDMoveTile
{
  DnDMessageHeader header;
  quint32 tile_uuid;
  quint16 x;
  quint16 y;
};

struct DnDDeleteTile
{
  DnDMessageHeader header;
  quint32 tile_uuid;
};

struct DnDPingPong
{
  DnDMessageHeader header;
};

struct DnDPingPongRecord
{
  DnDMessageHeader header;
  quint32 player_uuid;
  quint32 delay;
};

struct DnDImageQuery
{
  DnDMessageHeader header;
  char image_id[ImageId::ID_LEN];
};

#pragma pack()

#endif /* __DND_MESSAGES__ */
