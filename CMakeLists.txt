cmake_minimum_required(VERSION 3.23)
project(raytracer)

set(CMAKE_CXX_STANDARD 20)

add_executable(raytracer main.cpp Vector3.h HelperFunctions.h Ray.h Camera.h Interval.h Scene.h Color3.h Material.h Renderer.h HitRecord.h)
