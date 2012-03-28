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
