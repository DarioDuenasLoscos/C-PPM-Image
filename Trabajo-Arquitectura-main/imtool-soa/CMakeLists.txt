add_executable(imtool-soa main.cpp)
target_link_libraries (imtool-soa PRIVATE common imgsoa Microsoft.GSL::GSL)
