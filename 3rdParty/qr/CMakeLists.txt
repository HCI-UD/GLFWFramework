project(QRGenerator)
#cmake_minimum_required(VERSION 3.4)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode)
add_library(qrcoder STATIC
   qrgen.h
   LibQREncode/bitstream.c
   LibQREncode/mask.c
   LibQREncode/mmask.c
   LibQREncode/mqrspec.c
   LibQREncode/qrencode.c
   LibQREncode/qrinput.c
   LibQREncode/qrspec.c
   LibQREncode/rscode.c
   LibQREncode/split.c
   stdafx.cpp
   LibQREncode/bitstream.h
   LibQREncode/config.h
   LibQREncode/mask.h
   LibQREncode/mmask.h
   LibQREncode/mqrspec.h
   LibQREncode/qrencode.h
   LibQREncode/qrencode_inner.h
   LibQREncode/qrinput.h
   LibQREncode/qrspec.h
   LibQREncode/rscode.h
   LibQREncode/split.h
   qrgen.h
   stdafx.h
   targetver.h
   )
#set(SOURCE
#   ${SOURCE}
#   ${CMAKE_CURRENT_SOURCE_DIR}/QRGenerator.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/bitstream.c
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/mask.c
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/mmask.c
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/mqrspec.c
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/qrencode.c
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/qrinput.c
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/qrspec.c
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/rscode.c
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/split.c
#   ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.cpp
#)

#set(HEADERS
#   ${HEADERS}
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/bitstream.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/config.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/mask.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/mmask.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/mqrspec.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/qrencode.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/qrencode_inner.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/qrinput.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/qrspec.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/rscode.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/LibQREncode/split.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/qrgen.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/targetver.h
   #PARENT_SCOPE
#)
#add_executable(qrgenerator ${SOURCE} ${HEADERS})
