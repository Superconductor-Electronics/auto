cmake_minimum_required(VERSION 2.8)

project(auto3)

add_executable(auto3
	common.cpp
	common.hpp
	auto3.cpp
)

add_executable(cutt
	cutt.cpp
	common.cpp
	common.hpp
)

add_executable(ext
	common.cpp
	common.hpp
	ext.cpp
)

add_executable(tabcomp
	common.cpp
	common.hpp
	tabcomp.cpp
)

