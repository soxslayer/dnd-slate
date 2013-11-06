TEMPLATE = app
INCLUDEPATH += slate
QT += network

CONFIG += debug debug_and_release debug_and_release_target warn_on \
          link_pkgconfig

PKGCONFIG += lua protobuf

QMAKE_CXXFLAGS += -std=c++0x

DEFINES += NETWORK_PROTOCOL_DEBUG

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
  slate/dnd_controller.h \
  slate/dnd_object.h \
  slate/dnd_server.h \
  slate/game_board.h \
  slate/game_tile.h \
  slate/image.h \
  slate/image_database.h \
  slate/image_id.h \
  slate/image_request.h \
  slate/image_tile.h \
  slate/image_transfer.h \
  slate/lua_bindings.h \
  slate/non_copyable.h \
  slate/player.h \
  slate/player_list.h \
  slate/net_buffer_pool.h \
  slate/restricted_file_dialog.h \
  slate/serializable.h \
  slate/sha1.h \
  slate/sha_util.h \
  slate/slate_window.h \
  slate/tile.h \
  slate/tile_select_dialog.h \
  slate/util.h \
  slate/uuid.h

SOURCES += \
  slate/buffer.cpp \
  slate/buffer_manager.cpp \
  slate/chat_widget.cpp \
  slate/command.cpp \
  slate/command_manager.cpp \
  slate/command_param.cpp \
  slate/command_param_list.cpp \
  slate/connect_dialog.cpp \
  slate/custom_tile.cpp \
  slate/dnd_client.cpp \
  slate/dnd_controller.cpp \
  slate/dnd_object.cpp \
  slate/dnd_server.cpp \
  slate/game_board.cpp \
  slate/game_tile.cpp \
  slate/image.cpp \
  slate/image_database.cpp \
  slate/image_id.cpp \
  slate/image_request.cpp \
  slate/image_tile.cpp \
  slate/image_transfer.cpp \
  slate/lua_bindings.cpp \
  slate/player.cpp \
  slate/player_list.cpp \
  slate/net_buffer_pool.cpp \
  slate/restricted_file_dialog.cpp \
  slate/serializable.cpp \
  slate/sha1.cpp \
  slate/sha_util.cpp \
  slate/slate.cpp \
  slate/slate_window.cpp \
  slate/tile.cpp \
  slate/tile_select_dialog.cpp \
  slate/util.cpp \
  slate/uuid.cpp
