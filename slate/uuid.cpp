#include "uuid.h"

UuidManager::UuidManager (QObject* parent)
  : QObject (parent),
    _uuid (0)
{
}

Uuid UuidManager::get_uuid ()
{
  return _uuid++;
}
