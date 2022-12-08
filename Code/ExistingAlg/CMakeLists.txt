cmake_minimum_required(VERSION 3.15)
project(InteractiveTopk)

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

        add_executable(ExistingAlg
                Active_Ranking/active_ranking.cpp
                Active_Ranking/active_ranking.h
                Others/Array.cc
                Others/Array.hh
                Others/data_struct.h
                Others/data_utility.cpp
                Others/data_utility.h
                UtilityApprox/UtilityApprox.cpp
                UtilityApprox/UtilityApprox.h
                rev_HDPI/rev_HDPI.cpp
                rev_HDPI/rev_HDPI.h
                PointPrune/partition_func.cpp
                PointPrune/partition_func.h
                PointPrune/PointPrune.cpp
                PointPrune/PointPrune.h
                PointPrune/user_mode.cpp
                PointPrune/user_mode.h
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
                Others/lp.cpp
                Others/lp.h
                exp_stats.cpp
                exp_stats.h
                main.cpp
                UH/maxUtility.cpp
                UH/maxUtility.h
                Median_Hull/medianhull.cpp
                Median_Hull/medianhull.h
                Qhull/mem.cpp
                Qhull/mem.h
                Qhull/merge.cpp
                Qhull/merge.h
                Others/operation.cpp
                Others/operation.h
                Qhull/poly.cpp
                Qhull/poly.h
                Qhull/poly2.cpp
                Preference_Learning/preferLearn.cpp
                Preference_Learning/preferLearn.h
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
                Qhull/qhull_build.cpp Qhull/qhull_build.h)

        target_link_libraries(ExistingAlg glpk)
        TARGET_LINK_LIBRARIES(ExistingAlg ${LP_SOLVE})

endif()