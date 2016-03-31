cmake_minimum_required(VERSION 2.8)
project( HLp )
find_package( OpenCV REQUIRED )
add_executable( ../HLp ../src/houghlinep.cpp )
target_link_libraries( ../HLp ${OpenCV_LIBS} )

#cmake_minimum_required(VERSION 2.8)
#project( lanedetect )
#find_package( OpenCV REQUIRED )
#add_executable( lanedetect lanedetect.cpp )
#target_link_libraries( lanedetect ${OpenCV_LIBS} )

cmake_minimum_required(VERSION 2.8)
project( HLCHEVPvideo )
find_package( OpenCV REQUIRED )
add_executable( ../HLCHEVPvideo ../src/houghlinevideoCHEVP.cpp )
target_link_libraries( ../HLCHEVPvideo ${OpenCV_LIBS} )

cmake_minimum_required(VERSION 2.8)
project( HLvideo )
find_package( OpenCV REQUIRED )
add_executable( ../HLvideo ../src/houghlinevideo.cpp )
target_link_libraries( ../HLvideo ${OpenCV_LIBS} )

cmake_minimum_required(VERSION 2.8)
project( HLCHEVP )
find_package( OpenCV REQUIRED )
add_executable( ../HLCHEVP ../src/houghlineCHEVP.cpp )
target_link_libraries( ../HLCHEVP ${OpenCV_LIBS} )

cmake_minimum_required(VERSION 2.8)
project( get_transform )
find_package( OpenCV REQUIRED )
add_executable( ../get_transform ../src/getting_perspective_transform.cpp )
target_link_libraries( ../get_transform ${OpenCV_LIBS} )


cmake_minimum_required(VERSION 2.8)
project( ipt_lanedetection )
find_package( OpenCV REQUIRED )
add_executable( ../ipt_lanedetection ../src/ipt_lanedetection.cpp )
target_link_libraries( ../ipt_lanedetection ${OpenCV_LIBS} )

