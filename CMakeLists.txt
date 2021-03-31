set(SRCS
    channel.cpp
    Epoll.cpp
    Eventloop.cpp
    main.cpp
    otherFun.cpp
    Task.cpp
    ThreadPool.cpp
    Httpdata.cpp
    Log.cpp
    LogFile.cpp
    TimeRound.cpp
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -Wno-unused-variable -pthread")
add_executable(WebServer ${SRCS})
