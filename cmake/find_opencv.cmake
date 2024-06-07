function(find_opencv opencv_dir_vaule target_version)
    set(base_dir ${CMAKE_BINARY_DIR}/.cache)
    set(download_dir ${base_dir}/opencv-download)
    set(extract_dir ${base_dir}/opencv-extract)
    set(install_dir ${base_dir}/opencv-lib)
    set(download_file ${download_dir}/LICENSE)
    set(extract_file ${extract_dir})
    set(enable_contrib ON)
    set(build_shared_libs ON)

    # 手动clone到下载目录
    if(NOT EXISTS ${download_file})
        message(STATUS "download opencv")
        execute_process(COMMAND git clone -b ${target_version} --depth 1 https://github.com/opencv/opencv.git ${download_dir})
        # check download
        if(NOT EXISTS ${download_file})
            message(FATAL_ERROR "download opencv failed")
        endif()
        # check version
        execute_process(COMMAND git describe --tags WORKING_DIRECTORY ${download_dir} OUTPUT_VARIABLE opencv_version)
        string(STRIP ${opencv_version} opencv_version)
        string(REPLACE "v" "" opencv_version ${opencv_version})
        string(REPLACE "\n" "" opencv_version ${opencv_version})
        if(NOT ${opencv_version} STREQUAL ${target_version})
            message(FATAL_ERROR "checkout opencv version failed")
        endif()
    endif()
    string(REPLACE "." "" versoion_num ${target_version})

    # 是否需要下载contrib
    if (${enable_contrib})
        set(contrib_download_dir ${base_dir}/opencv-contrib-download)
        set(contrib_extract_dir ${base_dir}/opencv-contrib-extract)
        set(contrib_download_file ${contrib_download_dir}/LICENSE)
        set(contrib_extract_file ${contrib_extract_dir})
        # 手动clone到下载目录
        if(NOT EXISTS ${contrib_download_file})
            message(STATUS "download opencv-contrib")
            execute_process(COMMAND git clone -b ${target_version} --depth 2 https://github.com/opencv/opencv_contrib.git ${contrib_download_dir})
            # check download
            if(NOT EXISTS ${contrib_download_file})
                message(FATAL_ERROR "download opencv-contrib failed")
            endif()
            # check version
            execute_process(COMMAND git describe --tags WORKING_DIRECTORY ${contrib_download_dir} OUTPUT_VARIABLE opencv_contrib_version)
        endif()
    endif()

    # 根据平台选择编译器 linux: ninja, windows: Visual Studio 17 2022
    if (WIN32)
        set(CMAKE_MAKE_PROGRAM "Visual Studio 17 2022")
    else()
        set(CMAKE_MAKE_PROGRAM "Unix Makefiles")
    endif()

    # 手动cmake build到解压目录
    set(install_debug_dir ${install_dir}-debug)
    if(NOT EXISTS ${install_debug_dir}/lib/opencv_world${versoion_num}d.lib AND NOT EXISTS ${install_debug_dir}/x64/vc17/staticlib/OpenCVConfig.cmake)
        file(MAKE_DIRECTORY ${extract_file})
        set(opencv_build_dir ${extract_file}/build)
        file(MAKE_DIRECTORY ${opencv_build_dir})
        
        message(STATUS "build opencv static library CMAKE_MAKE_PROGRAM: ${CMAKE_MAKE_PROGRAM}")
        execute_process(COMMAND cmake -G "${CMAKE_MAKE_PROGRAM}" -DCMAKE_CONFIGURATION_TYPES=Debug -DCMAKE_INSTALL_PREFIX=${install_debug_dir} 
        -DBUILD_SHARED_LIBS=${build_shared_libs} 
        -DBUILD_opencv_apps=OFF -DBUILD_opencv_aruco=OFF -DBUILD_opencv_bgsegm=OFF -DBUILD_opencv_bioinspired=OFF 
        -DBUILD_opencv_calib3d=ON 
        -DBUILD_opencv_ccalib=OFF -DBUILD_opencv_core=ON -DBUILD_opencv_datasets=OFF -DBUILD_opencv_dnn=OFF -DBUILD_opencv_dnn_objdetect=OFF 
        -DBUILD_opencv_dnn_superres=OFF -DBUILD_opencv_dpm=OFF -DBUILD_opencv_face=OFF 
        -DBUILD_opencv_flann=ON 
        -DBUILD_opencv_features2d=ON
        -DBUILD_opencv_fuzzy=OFF -DBUILD_opencv_gapi=OFF -DBUILD_opencv_hfs=OFF -DBUILD_opencv_highgui=OFF -DBUILD_opencv_imgcodecs=ON 
        -DBUILD_opencv_imgproc=ON -DBUILD_opencv_intensity_transform=OFF -DBUILD_opencv_line_descriptor=OFF -DBUILD_opencv_mcc=OFF -DBUILD_opencv_ml=OFF 
        -DBUILD_opencv_objdetect=OFF -DBUILD_opencv_optflow=OFF -DBUILD_opencv_phase_unwrapping=OFF 
        -DBUILD_opencv_photo=ON -DBUILD_opencv_plot=ON 
        -DBUILD_opencv_quality=OFF -DBUILD_opencv_rapid=OFF -DBUILD_opencv_reg=OFF -DBUILD_opencv_rgbd=OFF -DBUILD_opencv_saliency=OFF 
        -DBUILD_opencv_shape=OFF -DBUILD_opencv_stereo=OFF -DBUILD_opencv_stitching=OFF -DBUILD_opencv_structured_light=OFF -DBUILD_opencv_superres=OFF 
        -DBUILD_opencv_surface_matching=OFF -DBUILD_opencv_text=OFF -DBUILD_opencv_tracking=OFF -DBUILD_opencv_ts=OFF -DBUILD_opencv_video=OFF 
        -DBUILD_opencv_videoio=OFF -DBUILD_opencv_videostab=OFF -DBUILD_opencv_world=ON 
        -DBUILD_opencv_wechat_qrcode=OFF 
        -DBUILD_opencv_python_bindings_generator=OFF -DBUILD_opencv_python_tests=OFF -DBUILD_JAVA=OFF -DBUILD_opencv_java_bindings_generator=OFF 
        -DBUILD_opencv_js=OFF -DBUILD_opencv_js_bindings_generator=OFF -DBUILD_opencv_objc_bindings_generator=OFF -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF 
        -DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF -DBUILD_WITH_DEBUG_INFO=OFF -DWITH_ITT=OFF -DBUILD_ITT=OFF -Dccitt=OFF 
        -DBUILD_WITH_STATIC_CRT=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON ${download_dir} 
        # contrib about
        -DOPENCV_ENABLE_NONFREE=${enable_contrib}
        -DOPENCV_EXTRA_MODULES_PATH=${contrib_download_dir}/modules
        -DBUILD_opencv_xfeatures2d=ON 
        -DBUILD_opencv_ximgproc=ON 
        -DBUILD_opencv_xobjdetect=ON 
        -DBUILD_opencv_xphoto=ON 

        WORKING_DIRECTORY ${opencv_build_dir})
        
        execute_process(COMMAND cmake --build . --config Debug WORKING_DIRECTORY ${opencv_build_dir})
        execute_process(COMMAND cmake --build . --target install --config Debug WORKING_DIRECTORY ${opencv_build_dir})
        
        if(WIN32)
            if(NOT EXISTS ${install_debug_dir}/x64/vc17/staticlib/OpenCVConfig.cmake AND 
                NOT EXISTS ${install_debug_dir}/x64/vc17/lib/OpenCVConfig.cmake AND 
                NOT EXISTS ${install_debug_dir}/lib/OpenCVConfig.cmake)
                message(FATAL_ERROR "opencv build failed")
            endif()
        else()
            if(NOT EXISTS ${install_debug_dir}/lib/libopencv_world.so)
                message(FATAL_ERROR "opencv build failed")
            endif()
        endif()
    endif()

    # 手动cmake build到解压目录
    set(install_release_dir ${install_dir}-release)
    if(NOT EXISTS ${install_release_dir}/lib/opencv_world${versoion_num}.lib AND NOT EXISTS ${install_release_dir}/x64/vc17/staticlib/OpenCVConfig.cmake)
        file(MAKE_DIRECTORY ${extract_file})
        set(opencv_build_dir ${extract_file}/build)
        file(MAKE_DIRECTORY ${opencv_build_dir})

        message(STATUS "build opencv static library CMAKE_MAKE_PROGRAM: ${CMAKE_MAKE_PROGRAM}")
        execute_process(COMMAND cmake -G "${CMAKE_MAKE_PROGRAM}" -DCMAKE_CONFIGURATION_TYPES=Release -DCMAKE_INSTALL_PREFIX=${install_release_dir} 
        -DBUILD_opencv_apps=OFF -DBUILD_opencv_aruco=OFF -DBUILD_opencv_bgsegm=OFF -DBUILD_opencv_bioinspired=OFF 
        -DBUILD_opencv_calib3d=ON 
        -DBUILD_opencv_ccalib=OFF -DBUILD_opencv_core=ON -DBUILD_opencv_datasets=OFF -DBUILD_opencv_dnn=OFF -DBUILD_opencv_dnn_objdetect=OFF 
        -DBUILD_opencv_dnn_superres=OFF -DBUILD_opencv_dpm=OFF -DBUILD_opencv_face=OFF 
        -DBUILD_opencv_flann=ON 
        -DBUILD_opencv_features2d=ON
        -DBUILD_opencv_fuzzy=OFF -DBUILD_opencv_gapi=OFF -DBUILD_opencv_hfs=OFF -DBUILD_opencv_highgui=OFF -DBUILD_opencv_imgcodecs=ON 
        -DBUILD_opencv_imgproc=ON -DBUILD_opencv_intensity_transform=OFF -DBUILD_opencv_line_descriptor=OFF -DBUILD_opencv_mcc=OFF -DBUILD_opencv_ml=OFF 
        -DBUILD_opencv_objdetect=OFF -DBUILD_opencv_optflow=OFF -DBUILD_opencv_phase_unwrapping=OFF 
        -DBUILD_opencv_photo=ON -DBUILD_opencv_plot=ON 
        -DBUILD_opencv_quality=OFF -DBUILD_opencv_rapid=OFF -DBUILD_opencv_reg=OFF -DBUILD_opencv_rgbd=OFF -DBUILD_opencv_saliency=OFF 
        -DBUILD_opencv_shape=OFF -DBUILD_opencv_stereo=OFF -DBUILD_opencv_stitching=OFF -DBUILD_opencv_structured_light=OFF -DBUILD_opencv_superres=OFF 
        -DBUILD_opencv_surface_matching=OFF -DBUILD_opencv_text=OFF -DBUILD_opencv_tracking=OFF -DBUILD_opencv_ts=OFF -DBUILD_opencv_video=OFF 
        -DBUILD_opencv_videoio=OFF -DBUILD_opencv_videostab=OFF -DBUILD_opencv_world=ON 
        -DBUILD_opencv_wechat_qrcode=OFF 
        -DBUILD_opencv_python_bindings_generator=OFF -DBUILD_opencv_python_tests=OFF -DBUILD_JAVA=OFF -DBUILD_opencv_java_bindings_generator=OFF 
        -DBUILD_opencv_js=OFF -DBUILD_opencv_js_bindings_generator=OFF -DBUILD_opencv_objc_bindings_generator=OFF -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF 
        -DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF -DBUILD_WITH_DEBUG_INFO=OFF -DWITH_ITT=OFF -DBUILD_ITT=OFF -Dccitt=OFF 
        -DBUILD_WITH_STATIC_CRT=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON ${download_dir} 
        # contrib about
        -DOPENCV_ENABLE_NONFREE=${enable_contrib}
        -DOPENCV_EXTRA_MODULES_PATH=${contrib_download_dir}/modules
        -DBUILD_opencv_xfeatures2d=ON 
        -DBUILD_opencv_ximgproc=ON 
        -DBUILD_opencv_xobjdetect=ON 
        -DBUILD_opencv_xphoto=ON 

        WORKING_DIRECTORY ${opencv_build_dir})
        
        execute_process(COMMAND cmake --build . --config Release WORKING_DIRECTORY ${opencv_build_dir})
        execute_process(COMMAND cmake --build . --target install --config Release WORKING_DIRECTORY ${opencv_build_dir})
        
        if(WIN32)
            if(NOT EXISTS ${install_release_dir}/x64/vc17/staticlib/OpenCVConfig.cmake AND 
                NOT EXISTS ${install_release_dir}/x64/vc17/lib/OpenCVConfig.cmake AND 
                NOT EXISTS ${install_debug_dir}/lib/OpenCVConfig.cmake)
                message(FATAL_ERROR "opencv build failed")
            endif()
        else()
            if(NOT EXISTS ${install_release_dir}/lib/libopencv_world.so)
                message(FATAL_ERROR "opencv build failed")
            endif()
        endif()
    endif()

    # # 安装opencv
    # install(DIRECTORY ${install_dir}/ DESTINATION ${CMAKE_INSTALL_PREFIX} USE_SOURCE_PERMISSIONS)
    list(APPEND install_dirs ${install_debug_dir})
    list(APPEND install_dirs ${install_release_dir})
    foreach(clear_install_dir IN LISTS install_dirs)
        if(EXISTS ${clear_install_dir}/x64/vc17/lib/OpenCVConfig.cmake)
            # rm *.cmake
            file(GLOB cmake_files ${clear_install_dir}/x64/vc17/lib/*.cmake)
            foreach(cmake_file ${cmake_files})
                file(REMOVE ${cmake_file})
            endforeach()

            # copy x64/vc17/lib to install_dir/lib
            file(GLOB lib_files ${clear_install_dir}/x64/vc17/lib/*.lib)
            foreach(lib_file ${lib_files})
                file(COPY ${lib_file} DESTINATION ${clear_install_dir}/lib)
            endforeach()

            # copy x64/vc17/bin to install_dir/bin
            file(GLOB bin_files ${clear_install_dir}/x64/vc17/bin/*.dll)
            foreach(bin_file ${bin_files})
                file(COPY ${bin_file} DESTINATION ${clear_install_dir}/bin)
            endforeach()
        endif()
        if(EXISTS ${clear_install_dir}/x64/vc17/staticlib/OpenCVConfig.cmake)
            # rm *.cmake
            file(GLOB cmake_files ${clear_install_dir}/x64/vc17/staticlib/*.cmake)
            foreach(cmake_file ${cmake_files})
                file(REMOVE ${cmake_file})
            endforeach()

            # copy x64/vc17/staticlib to install_dir/lib
            file(GLOB lib_files ${clear_install_dir}/x64/vc17/staticlib/*.lib)
            foreach(lib_file ${lib_files})
                file(COPY ${lib_file} DESTINATION ${clear_install_dir}/lib)
            endforeach()
        endif()
        if (EXISTS ${clear_install_dir}/etc)
            # rm x64/vc17/staticlib
            file(REMOVE_RECURSE ${clear_install_dir}/x64)
            file(REMOVE_RECURSE ${clear_install_dir}/etc)
        endif()
    endforeach()
    
    # 清理编译目录
    file(REMOVE_RECURSE ${extract_dir})

    set(${opencv_dir_vaule} ${install_dir} PARENT_SCOPE)
    message(STATUS "auto find opencv done")
endfunction()
