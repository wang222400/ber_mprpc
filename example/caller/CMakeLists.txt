set(SRC_LIST calluserservice.cc
             ../user.pb.cc
             ../resultcode.pb.cc
             ../friend.pb.cc)
add_executable(consumer ${SRC_LIST})
target_link_libraries(consumer mprpc protobuf)