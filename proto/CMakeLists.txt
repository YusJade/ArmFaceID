if(WIN32)
    set(protobuf_DIR D:\\cmake-install-modules\\lib\\cmake\\protobuf)
    set(absl_DIR D:\\cmake-install-modules\\lib\\cmake\\absl)
    set(utf8_range_DIR D:\\cmake-install-modules\\lib\\cmake\\utf8_range)
    set(gRPC_DIR D:\\cmake-install-modules\\lib\\cmake\\grpc)
endif()


find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(c-ares CONFIG)
# find_package(Threads)

#
# Protobuf/Grpc source files
#
set(PROTO_FILES
    face_network.proto
)

#
# Add Library target with protobuf sources
#
add_library(face_proto ${PROTO_FILES})
target_link_libraries(face_proto
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)
target_include_directories(face_proto PUBLIC
    ${PROTOBUF_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_BINARY_DIR})

#
# Compile protobuf and grpc files in mointor proto target to cpp
#
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET face_proto LANGUAGE cpp)
protobuf_generate(TARGET face_proto LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")
