find_package(ZeroMQ REQUIRED)

add_executable(controller
    main_controller.cpp
    src/tcontroller.cpp
    src/tmessaging.cpp
    src/tsearch.cpp
    src/ttopology.cpp
    src/common.cpp
)
target_link_libraries(controller PRIVATE zmq)
target_include_directories(controller PRIVATE include)

add_executable(worker
    main_worker.cpp
    src/tworker.cpp
    src/tmessaging.cpp
    src/tsearch.cpp
    src/ttopology.cpp
    src/common.cpp
)
target_link_libraries(worker PRIVATE zmq)
target_include_directories(worker PRIVATE include)
