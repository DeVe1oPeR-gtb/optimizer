cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# デバッグ用の最適化フラグ (-g を有効化)
set(CMAKE_BUILD_TYPE Debug)

# CppUTest のパスを手動設定
set(CPPUTEST_DIR "/home/mcqueen/cpputest")
include_directories(${CPPUTEST_DIR}/include)
link_directories(${CPPUTEST_DIR}/lib)

# ソースディレクトリ
set(ROOT_DIR ${CMAKE_SOURCE_DIR})
set(OPTIMIZER_DIR ${ROOT_DIR}/Optimizer)
set(PSO_DIR ${ROOT_DIR}/PSO)
set(LM_DIR ${ROOT_DIR}/LM)
set(DE_DIR ${ROOT_DIR}/DE)
set(UTIL_DIR ${ROOT_DIR}/util)  # util フォルダを追加

# 追加インクルード
include_directories(${UTIL_DIR})

# Optimizer モジュール
file(GLOB OPTIMIZER_SOURCES ${OPTIMIZER_DIR}/*.cpp)
add_library(Optimizer STATIC ${OPTIMIZER_SOURCES})
target_include_directories(Optimizer PUBLIC ${OPTIMIZER_DIR})

# PSO モジュール（Optimizer を使用）
file(GLOB PSO_SOURCES ${PSO_DIR}/*.cpp)
add_library(PSO STATIC ${PSO_SOURCES})
target_include_directories(PSO PUBLIC ${PSO_DIR})
target_link_libraries(PSO PUBLIC Optimizer)

# LM モジュール（Optimizer を使用）
file(GLOB LM_SOURCES ${LM_DIR}/*.cpp)
add_library(LM STATIC ${LM_SOURCES})
target_include_directories(LM PUBLIC ${LM_DIR})
target_link_libraries(LM PUBLIC Optimizer)

# DE モジュール（Optimizer を使用）
file(GLOB DE_SOURCES ${DE_DIR}/*.cpp)
add_library(DE STATIC ${DE_SOURCES})
target_include_directories(DE PUBLIC ${DE_DIR})
target_link_libraries(DE PUBLIC Optimizer)

# テスト用実行ファイル
add_executable(AllTests AllTests.cpp)
target_link_libraries(AllTests PRIVATE PSO LM DE CppUTest CppUTestExt)
# target_link_libraries(AllTests PRIVATE PSO CppUTest CppUTestExt)
