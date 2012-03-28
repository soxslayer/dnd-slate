TEMPLATE = app
INCLUDEPATH += slate
QT += network
CONFIG += debug

DESTDIR = build
OBJECTS_DIR = build/.obj
MOC_DIR = build/.moc
RCC_DIR = build/.rcc
UI_DIR = build/.ui

HEADERS += \
  slate/buffer.h \
  slate/buffer_manager.h \
  slate/chat_widget.h \
  slate/connect_dialog.h \
  slate/dnd_client.h \
  slate/dnd_server.h \
  slate/player_list.h \
  slate/net_buffer_pool.h \
  slate/slate_window.h \
  slate/uuid.h

SOURCES += \
  slate/buffer.cpp \
  slate/buffer_manager.cpp \
  slate/chat_widget.cpp \
  slate/connect_dialog.cpp \
  slate/dnd_client.cpp \
  slate/dnd_server.cpp \
  slate/player_list.cpp \
  slate/net_buffer_pool.cpp \
  slate/slate.cpp \
  slate/slate_window.cpp \
  slate/uuid.cpp
