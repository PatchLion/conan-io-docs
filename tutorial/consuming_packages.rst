.. _tutorial_consuming_packages:

Consuming packages
==================

This section shows how to build your projects using Conan to manage your dependencies. We
will begin with a basic example of a C project that uses CMake and depends on the **zlib**
library. This project will use a *conanfile.txt* file to declare its dependencies.

本节说明如何使用 Conan 生成项目以管理依赖项。我们将从一个使用 CMake 并依赖于 **zlib** 库的 C 项目的基本示例开始。该项目将使用 *conanfile.txt* 文件来声明其依赖项。

We will also cover how you can not only use 'regular' libraries with Conan but also manage
tools you may need to use while building: like CMake, msys2, MinGW, etc. 

我们还将介绍如何不仅可以在 Conan 中使用“常规”库，还可以管理构建时可能需要使用的工具: 如 CMake、 msys2、 MinGW 等。

Then, we will explain different Conan concepts like settings and options and how you can
use them to build your projects for different configurations like Debug, Release, with
static or shared libraries, etc. 

然后，我们将解释不同的 Conan 概念，比如设置和选项，以及如何使用它们为不同的配置(如 Debug、 Release、带有静态或共享库等)构建项目。

Also, we will explain how to transition from the *conanfile.txt* file we used in the first
example to a more powerful *conanfile.py*.

此外，我们还将解释如何从第一个示例中使用的 *conanfile.txt* 文件转换到功能更强大的 *conanfile.py* 文件。

After that, we will introduce the concept of Conan build and host profiles and explain how
you can use them to cross-compile your application to different platforms.

之后，我们将介绍 Conan 构建和主机配置文件的概念，并解释如何使用它们将应用程序交叉编译到不同的平台。

Then, in the "Introduction to versioning" we will learn about using different versions, 
defining requirements with version ranges, the concept of revisions and a brief introduction 
to lockfiles to achieve reproducibility of the dependency graph.

然后，在“版本控制简介”中，我们将学习如何使用不同的版本，用版本范围定义需求，修订的概念，以及对锁文件的简要介绍，以实现依赖关系图的可重复性。

.. toctree::
   :maxdepth: 2
   :caption: Table of contents
   
   consuming_packages/build_simple_cmake_project
   consuming_packages/use_tools_as_conan_packages
   consuming_packages/different_configurations
   consuming_packages/the_flexibility_of_conanfile_py
   consuming_packages/cross_building_with_conan.rst
   consuming_packages/intro_to_versioning
