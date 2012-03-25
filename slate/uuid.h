#ifndef __UUID__
#define __UUID__

#include <QObject>

typedef quint32 Uuid;

class UuidManager : public QObject
{
public:
  UuidManager (QObject* parent = 0);

  Uuid get_uuid ();

private:
  Uuid _uuid;
};

#endif /* __UUID__ */
