set(SRC_LIST userservice.cc 
             ../user.pb.cc
             ../resultcode.pb.cc
             ../friend.pb.cc)
add_executable(provider ${SRC_LIST})
target_link_libraries(provider mprpc protobuf)