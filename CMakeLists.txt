cmake_minimum_required(VERSION 3.23)
project(Impasse)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

add_executable(Impasse main.cpp Agents/evaluate.h Agents/evaluate.cpp Board.cpp Board.h misc.h Move.cpp Move.h misc.cpp Game.cpp Game.h Agents/BaseAgent.h Agents/RandomAgent.cpp Agents/RandomAgent.h BoardState.cpp BoardState.h Agents/AlphaBeta.cpp Agents/AlphaBeta.h TT.cpp TT.h ZobristHash.cpp ZobristHash.h Agents/Human.cpp Agents/Human.h)
add_executable(Validator gameValidator.cpp Board.cpp Board.h misc.h Move.cpp Move.h misc.cpp Game.cpp Game.h Agents/BaseAgent.h Agents/RandomAgent.cpp Agents/RandomAgent.h BoardState.cpp BoardState.h ZobristHash.cpp ZobristHash.h)
add_executable(EvalTest evalTest.cpp Agents/evaluate.h Agents/evaluate.cpp Board.cpp Board.h misc.h Move.cpp Move.h misc.cpp Game.cpp Game.h Agents/BaseAgent.h BoardState.cpp BoardState.h Agents/AlphaBeta.cpp Agents/AlphaBeta.h TT.cpp TT.h ZobristHash.cpp ZobristHash.h)
