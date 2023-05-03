cmake_minimum_required(VERSION 3.15)
project(ques_type)

enable_testing()

# for sampling packages
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
if(COMMAND cmake_policy)
       cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)



find_library(LP_SOLVE NAMES liblpsolve55.so PATHS /usr/lib/lp_solve)
if (NOT LP_SOLVE)
        message(FATAL_ERROR "This program requires the lp_solve library, and will not be compiled.")
else ()
        message(STATUS "Library lp_solve found: ${LP_SOLVE}")
        set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")



        set(CMAKE_CXX_STANDARD 14)

        set(INC_DIR /usr/local/Cellar/glpk/4.65/include)
        set(LINK_DIR /usr/local/Cellar/glpk/4.65/lib)

        include_directories(${INC_DIR})
        link_directories(${LINK_DIR})
        link_libraries(glpk)


        include_directories (BEFORE ./external/Eigen)
        include_directories (BEFORE ./external)
        include_directories (BEFORE ./external/minimum_ellipsoid)
        include_directories (BEFORE ./external/LPsolve_src/run_headers)
        include_directories (BEFORE ./external/boost)
        include_directories (BEFORE ./include/generators)
        include_directories (BEFORE ./include/volume)
        include_directories (BEFORE ./include)
        include_directories (BEFORE ./include/convex_bodies)
        include_directories (BEFORE ./include/annealing)
        include_directories (BEFORE ./include/samplers)
        include_directories (BEFORE ./include/lp_oracles)
        include_directories (BEFORE ./include/misc)
      
        #for Eigen
        if (${CMAKE_VERSION} VERSION_LESS "3.12.0") 
          add_compile_options(-D "EIGEN_NO_DEBUG")
        else ()
          add_compile_definitions("EIGEN_NO_DEBUG")
        endif ()

        add_definitions(${CMAKE_CXX_FLAGS} "-std=c++11")  # enable C++11 standard
        add_definitions(${CMAKE_CXX_FLAGS} "-g")  # enable debuger
        add_definitions(${CXX_COVERAGE_COMPILE_FLAGS} "-lm")
        add_definitions(${CXX_COVERAGE_COMPILE_FLAGS} "-ldl")
        add_definitions(${CXX_COVERAGE_COMPILE_FLAGS} "-DBOOST_NO_AUTO_PTR")

        add_executable(ques_type
                2DPI/2dPI.cpp
                2DPI/2dPI.h
                RH/alg_one.cpp
                RH/alg_one.h
                HDPI/HDPI.cpp
                HDPI/HDPI.h
                rev_HDPI/rev_HDPI.cpp
                rev_HDPI/rev_HDPI.h
                exact/Exact_revised.h
                exact/Exact_revised.cpp
                exact/exact_question.h
                exact/exact_question.cpp
                sampling/sampling.h
                sampling/sampling.cpp
                sampling/sampling_question.h
                sampling/sampling_question.cpp
                optimal/optimal.h
                optimal/optimal.cpp
                optimal/optimal_question.h
                optimal/optimal_question.cpp
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
                exp_stats.cpp
                exp_stats.h
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

        target_link_libraries(ques_type glpk)
        TARGET_LINK_LIBRARIES(ques_type ${LP_SOLVE})
endif()