cmake_minimum_required(VERSION 3.15)
project(User_study)


set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_EXE_LINKER_FLAGS -static)
set(INC_DIR /usr/local/Cellar/glpk/4.65/include)
set(LINK_DIR /usr/local/Cellar/glpk/4.65/lib)


include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(glpk)

add_executable(User_study
        active_ranking.cpp
        active_ranking.h
        RH.cpp
        RH.h
        HDPI.cpp
        HDPI.h
        Others/Array.cc
        Others/Array.hh
        Others/data_struct.h
        Others/data_utility.cpp
        Others/data_utility.h
        Qhull/frame.cpp
        Qhull/frame.h
        Qhull/geom.cpp
        Qhull/geom.h
        Qhull/geom2.cpp
        Qhull/global.cpp
        Qhull/io.cpp
        Qhull/io.h
        Qhull/libqhull.cpp
        Qhull/libqhull.h
        lp.cpp
        lp.h
        user_study.cpp
        maxUtility.cpp
        maxUtility.h
        Qhull/mem.cpp
        Qhull/mem.h
        Qhull/merge.cpp
        Qhull/merge.h
        Others/operation.cpp
        Others/operation.h
        Qhull/poly.cpp
        Qhull/poly.h
        Qhull/poly2.cpp
        preferLearn.cpp
        preferLearn.h
        Others/pruning.cpp
        Others/pruning.h
        Qhull/qhull_a.h
        Qhull/qset.cpp
        Qhull/qset.h
        Others/QuadProg++.cc
        Others/QuadProg++.hh
        Qhull/random.cpp
        Qhull/random.h
        Qhull/rboxlib.cpp
        Others/read_write.cpp
        Others/read_write.h
        Others/rtree.cpp
        Others/rtree.h
        Qhull/stat.cpp
        Qhull/stat.h
        Others/used_function.cpp
        Others/used_function.h
        Qhull/user.cpp
        Qhull/user.h
        Qhull/usermem.cpp
        Qhull/userprintf.cpp
        Qhull/userprintf_rbox.cpp
        Qhull/qhull_build.cpp Qhull/qhull_build.h main.cpp user_study.h)

target_link_libraries(User_study glpk)