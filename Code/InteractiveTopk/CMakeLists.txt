cmake_minimum_required(VERSION 3.15)
project(InteractiveTopk)

set(CMAKE_CXX_STANDARD 14)

set(INC_DIR /usr/local/Cellar/glpk/4.65/include)
set(LINK_DIR /usr/local/Cellar/glpk/4.65/lib)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(glpk)

add_executable(InteractiveTopk
        2DPI/2dPI.cpp
        2DPI/2dPI.h
        RH/alg_one.cpp
        RH/alg_one.h
        HDPI/HDPI.cpp
        HDPI/HDPI.h
        Others/data_struct.h
        Others/data_utility.cpp
        Others/data_utility.h
        Others/frame.cpp
        Others/frame.h
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
        main.cpp
        Qhull/mem.cpp
        Qhull/mem.h
        Qhull/merge.cpp
        Qhull/merge.h
        Others/operation.cpp
        Others/operation.h
        Qhull/poly.cpp
        Qhull/poly.h
        Qhull/poly2.cpp
        Others/pruning.cpp
        Others/pruning.h
        Qhull/qhull_a.h
        Others/qhull_build.cpp
        Others/qhull_build.h
        Qhull/qset.cpp
        Qhull/qset.h
        Qhull/random.cpp
        Qhull/random.h
        Qhull/rboxlib.cpp
        Others/read_write.cpp
        Others/read_write.h
        Qhull/stat.cpp
        Qhull/stat.h
        Others/used_function.cpp
        Others/used_function.h
        Qhull/user.cpp
        Qhull/user.h
        Qhull/usermem.cpp
        Qhull/userprintf.cpp
        Qhull/userprintf_rbox.cpp)

target_link_libraries(InteractiveTopk glpk)
