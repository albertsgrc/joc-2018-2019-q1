cmake_minimum_required(VERSION 3.13)
project(joc_2018_2019_q1)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-variable -DDEBUG -D_GLIBCXX_DEBUG -g")
set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-variable -Ofast -march=native -g")

include_directories(.)

add_executable(joc_2018_2019_q1
        Action.cc
        Action.hh
        AIDemo.cc
        AINull.cc
        AICancellara.cc
        Board.cc
        Board.hh
        Game.cc
        Game.hh
        Info.cc
        Info.hh
        Main.cc
        Player.cc
        Player.hh
        Random.cc
        Random.hh
        Registry.cc
        Registry.hh
        Settings.cc
        Settings.hh
        State.cc
        State.hh
        Structs.cc
        Structs.hh
        Utils.cc
        Utils.hh
        AIDummy.o
        AIPrevious.cc)
