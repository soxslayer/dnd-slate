TEMPLATE = app
INCLUDEPATH += slate
QT += network

CONFIG += debug debug_and_release debug_and_release_target warn_on \
          link_pkgconfig

PKGCONFIG += lua

HEADERS += \
  slate/buffer.h \
  slate/buffer_manager.h \
  slate/chat_widget.h \
  slate/command.h \
  slate/command_manager.h \
  slate/command_param.h \
  slate/command_param_list.h \
  slate/connect_dialog.h \
  slate/custom_tile.h \
  slate/dnd_client.h \
  slate/dnd_server.h \
  slate/game_board.h \
  slate/game_tile.h \
  slate/image_tile.h \
  slate/lua_bindings.h \
  slate/player_list.h \
  slate/net_buffer_pool.h \
  slate/restricted_file_dialog.h \
  slate/slate_window.h \
  slate/tile.h \
  slate/tile_select_dialog.h \
  slate/uuid.h

SOURCES += \
  slate/buffer.cpp \
  slate/buffer_manager.cpp \
  slate/chat_widget.cpp \
  slate/command_manager.cpp \
  slate/command_param.cpp \
  slate/command_param_list.cpp \
  slate/connect_dialog.cpp \
  slate/custom_tile.cpp \
  slate/dnd_client.cpp \
  slate/dnd_server.cpp \
  slate/game_board.cpp \
  slate/game_tile.cpp \
  slate/image_tile.cpp \
  slate/lua_bindings.cpp \
  slate/player_list.cpp \
  slate/net_buffer_pool.cpp \
  slate/restricted_file_dialog.cpp \
  slate/slate.cpp \
  slate/slate_window.cpp \
  slate/tile.cpp \
  slate/tile_select_dialog.cpp \
  slate/uuid.cpp
