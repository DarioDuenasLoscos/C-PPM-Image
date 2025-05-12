add_executable(imtool-aos main.cpp)
target_link_libraries(imtool-aos PRIVATE common imgaos Microsoft.GSL::GSL)
