.. _consuming_packages_build_simple_cmake_project:

Build a simple CMake project using Conan
========================================

Let's get started with an example: We are going to create a string compressor application
that uses one of the most popular C++ libraries: `Zlib <https://zlib.net/>`__.

让我们从一个示例开始: 我们将创建一个字符串压缩器应用程序，它使用最流行的 C++ 库之一:  `Zlib <https://zlib.net/>`__。

We'll use CMake as build system in this case but keep in mind that Conan **works with any
build system** and is not limited to using CMake. You can check more examples with other
build systems in the :ref:`Read More
section<consuming_packages_read_more>`.

在这种情况下，我们将使用 CMake 作为构建系统，但是请记住，Conan 可以使用 **任何构建系统**，并且不限于使用 CMake。您可以在  :ref:`Read More section<consuming_packages_read_more>` 中查看更多有关其他构建系统的示例。

Please, first clone the sources to recreate this project, you can find them in the
`examples2.0 repository <https://github.com/conan-io/examples2>`_ in GitHub:

请首先克隆这些源代码来重新创建这个项目，您可以在 GitHub 的 `examples2.0 repository <https://github.com/conan-io/examples2>`_  中找到它们:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/consuming_packages/simple_cmake_project


We start from a very simple C language project with this structure:

我们从一个非常简单的 C 语言项目开始，其结构如下:

.. code-block:: text

    .
    ├── CMakeLists.txt
    └── src
        └── main.c

This project contains a basic *CMakeLists.txt* including the **zlib** dependency and the
source code for the string compressor program in *main.c*.

该项目包含一个基本的 *CMakeLists.txt*，其中包含 **zlib** 依赖项和 *main.c* 中字符串压缩程序的源代码。

Let's have a look at the *main.c* file:

.. code-block:: cpp
    :caption: **main.c**

    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>

    #include <zlib.h>

    int main(void) {
        char buffer_in [256] = {"Conan is a MIT-licensed, Open Source package manager for C and C++ development "
                                "for C and C++ development, allowing development teams to easily and efficiently "
                                "manage their packages and dependencies across platforms and build systems."};
        char buffer_out [256] = {0};

        z_stream defstream;
        defstream.zalloc = Z_NULL;
        defstream.zfree = Z_NULL;
        defstream.opaque = Z_NULL;
        defstream.avail_in = (uInt) strlen(buffer_in);
        defstream.next_in = (Bytef *) buffer_in;
        defstream.avail_out = (uInt) sizeof(buffer_out);
        defstream.next_out = (Bytef *) buffer_out;

        deflateInit(&defstream, Z_BEST_COMPRESSION);
        deflate(&defstream, Z_FINISH);
        deflateEnd(&defstream);

        printf("Uncompressed size is: %lu\n", strlen(buffer_in));
        printf("Compressed size is: %lu\n", strlen(buffer_out));

        printf("ZLIB VERSION: %s\n", zlibVersion());

        return EXIT_SUCCESS;
    }

Also, the contents of *CMakeLists.txt* are:

.. code-block:: cmake
    :caption: **CMakeLists.txt**

    cmake_minimum_required(VERSION 3.15)
    project(compressor C)

    find_package(ZLIB REQUIRED)

    add_executable(${PROJECT_NAME} src/main.c)
    target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)

Our application relies on the **Zlib** library. Conan, by default, tries to install
libraries from a remote server called `ConanCenter <https://conan.io/center/>`_.
You can search there for libraries and also check the available versions. In our case, 
after checking the available versions for `Zlib <https://conan.io/center/zlib>`__ we
choose to use the latest available version: **zlib/1.2.11**.


我们的应用程序依赖于  **Zlib** 库。默认情况下，Conan尝试从名为 `ConanCenter <https://conan.io/center/>`_ 的远程服务器安装库。
您可以在那里搜索库，也可以检查可用的版本。在我们的示例中，在检查了  `Zlib <https://conan.io/center/zlib>`__  的可用版本之后，
我们选择使用最新的可用版本: **zlib/1.2.11**。

The easiest way to install the **Zlib** library and find it from our project with Conan is
using a *conanfile.txt* file. Let's create one with the following content:

要安装  **Zlib** 库并在我们的 Conan 项目中找到它，最简单的方法是使用 *conanfile.txt* 文件。让我们创建一个包含以下内容的文件:

.. code-block:: ini
    :caption: **conanfile.txt**

    [requires]
    zlib/1.2.11

    [generators]
    CMakeDeps
    CMakeToolchain

As you can see we added two sections to this file with a syntax similar to an *INI* file.

正如您所看到的，我们使用类似于 *INI* 文件的语法向该文件添加了两个部分。

    * **[requires]** section is where we declare the libraries we want to use in the
      project, in this case, **zlib/1.2.11**.

      **[requires]** 部分是我们声明要在项目中使用的库，在本例中是 **zlib/1.2.11**。

    * **[generators]** section tells Conan to generate the files that the compilers or
      build systems will use to find the dependencies and build the project. In this case,
      as our project is based in *CMake*, we will use :ref:`CMakeDeps<conan_tools_cmakedeps>` to
      generate information about where the **Zlib** library files are installed and
      :ref:`CMakeToolchain<conan_tools_cmaketoolchain>` to pass build information to *CMake*
      using a *CMake* toolchain file.

      **[generators]** 部分告诉 Conan 生成编译器或构建系统查找依赖项并构建项目的文件。
      在这种情况下，由于我们的项目基于 *CMake*，我们将使用 :ref:`CMakeDeps<conan_tools_cmakedeps>` 生成关于 **Zlib** 库文件安装位置的信息，
      并 :ref:`CMakeToolchain<conan_tools_cmaketoolchain>`  使用 *CMake* 工具链文件将构建信息传递给 *CMake*。


Besides the *conanfile.txt*, we need a **Conan profile** to build our project. Conan
profiles allow users to define a configuration set for things like the compiler, build
configuration, architecture, shared or static libraries, etc. Conan, by default, will
not try to detect a profile automatically, so we need to create one. To let Conan try
to guess the profile, based on the current operating system and installed tools, please
run:

除了 *conanfile.txt* 之外，我们还需要一个 **Conan profile** 文件来构建我们的项目。
Conan profile 文件允许用户为编译器、构建配置、体系结构、共享或静态库等定义配置集。
默认情况下，Conan 不会尝试自动检测配置文件，所以我们需要创建一个。
要让 Conan 根据当前的操作系统和已安装的工具来猜测配置文件，请运行:

.. code-block:: bash

    conan profile detect --force

This will detect the operating system, build architecture and compiler settings based on
the environment. It will also set the build configuration as *Release* by default. The
generated profile will be stored in the Conan home folder with name *default* and will be
used by Conan in all commands by default unless another profile is specified via the command
line. An example of the output of this command for MacOS would be:

这将根据环境检测操作系统、构建体系结构和编译器设置。默认情况下，它还会将构建配置设置为 *Release* 。
所生成的配置文件将存储在 Conan 主文件夹中，文件名为 *default* ，默认情况下，
Conan 将在所有命令中使用该配置文件，除非通过命令行指定另一个配置文件。
这个命令在 MacOS 上的输出示例如下:

.. code-block:: ini

    $ conan profile detect --force
    Found apple-clang 14.0
    apple-clang>=13, using the major as version
    Detected profile:
    [settings]
    arch=x86_64
    build_type=Release
    compiler=apple-clang
    compiler.cppstd=gnu17
    compiler.libcxx=libc++
    compiler.version=14
    os=Macos

.. note:: **A note about the detected C++ standard by Conan**

    Conan will always set the default C++ standard as the one that the detected compiler
    version uses by default, except for the case of macOS using apple-clang. In this case,
    for apple-clang>=11, it sets ``compiler.cppstd=gnu17``. If you want to use a different
    C++ standard, you can edit the default profile file directly. First, get the location
    of the default profile using:

    Conan 总是默认将的C++标准设置为被检测到的编译器版本的默认标准，
    但使用 apple-clang 的 macOS 除外。在这种情况下，对于 apple-clang>=11，
    它将设置 ``compiler.cppstd=gnu17``。如果您想使用不同的C++标准，
    您可以直接编辑默认配置文件。首先，使用以下方法获取默认配置文件的位置:

    .. code-block:: bash

        $ conan profile path default
        /Users/user/.conan2/profiles/default

    Then open and edit the file and set ``compiler.cppstd`` to the C++ standard you want
    to use.

    然后打开并编辑这个文件，并将  ``compiler.cppstd`` 设置为您想要使用的C++标准。

We will use Conan to install **Zlib** and generate the files that CMake needs to
find this library and build our project. We will generate those files in the folder
*build*. To do that, run:

我们将使用 Conan 来安装 **Zlib** 并生成 CMake 需要的文件，以找到这个库并构建我们的项目。
我们将在 *build* 文件夹生成中生成这些文件。要做到这一点，请运行:

.. code-block:: bash

    $ conan install . --output-folder=build --build=missing


You will get something similar to this as the output of that command:

您将得到与该命令的输出类似的结果:

.. code-block:: bash

    $ conan install . --output-folder=build --build=missing
    ...
    -------- Computing dependency graph ----------
    zlib/1.2.11: Not found in local cache, looking in remotes...
    zlib/1.2.11: Checking remote: conancenter
    zlib/1.2.11: Trying with 'conancenter'...
    Downloading conanmanifest.txt
    Downloading conanfile.py
    Downloading conan_export.tgz
    Decompressing conan_export.tgz
    zlib/1.2.11: Downloaded recipe revision f1fadf0d3b196dc0332750354ad8ab7b
    Graph root
        conanfile.txt: /home/conan/examples2/tutorial/consuming_packages/simple_cmake_project/conanfile.txt
    Requirements
        zlib/1.2.11#f1fadf0d3b196dc0332750354ad8ab7b - Downloaded (conancenter)

    -------- Computing necessary packages ----------
    Requirements
        zlib/1.2.11#f1fadf0d3b196dc0332750354ad8ab7b:cdc9a35e010a17fc90bb845108cf86cfcbce64bf#dd7bf2a1ab4eb5d1943598c09b616121 - Download (conancenter)

    -------- Installing packages ----------

    Installing (downloading, building) binaries...
    zlib/1.2.11: Retrieving package cdc9a35e010a17fc90bb845108cf86cfcbce64bf from remote 'conancenter'
    Downloading conanmanifest.txt
    Downloading conaninfo.txt
    Downloading conan_package.tgz
    Decompressing conan_package.tgz
    zlib/1.2.11: Package installed cdc9a35e010a17fc90bb845108cf86cfcbce64bf
    zlib/1.2.11: Downloaded package revision dd7bf2a1ab4eb5d1943598c09b616121

    -------- Finalizing install (deploy, generators) ----------
    conanfile.txt: Generator 'CMakeToolchain' calling 'generate()'
    conanfile.txt: Generator 'CMakeDeps' calling 'generate()'
    conanfile.txt: Aggregating env generators


As you can see in the output, there are a couple of things that happened:

正如您在输出中看到的，发生了以下几件事:

    * Conan installed the *Zlib* library from the remote server we configured at the
      beginning of the tutorial. This server stores both the Conan recipes, which are the
      files that define how libraries must be built, and the binaries that can be reused so we
      don't have to build from sources every time.

      Conan 从我们在本教程开头配置的远程服务器上安装了 *Zlib* 库。这个服务器既存储了 Conan recipes(方法)，
      也存储了定义必须如何构建库的文件，还存储了可重用的二进制文件，这样我们就不必每次都从源代码构建。

    * Conan generated several files under the **build** folder. Those files
      were generated by both the ``CMakeToolchain`` and ``CMakeDeps`` generators we set in
      the **conanfile.txt**. ``CMakeDeps`` generates files so that CMake finds the Zlib
      library we have just downloaded. On the other side, ``CMakeToolchain`` generates a
      toolchain file for CMake so that we can transparently build our project with CMake
      using the same settings that we detected for our default profile.

      Conan 在 **build** 文件夹下生成了几个文件。这些文件是由我们在 **conanfile.txt** 中设置的 ``CMakeToolchain`` 
      和 ``CMakeDeps`` 生成器生成的。 ``CMakeDeps`` 生成文件，以便 CMake 找到我们刚刚下载的 Zlib 库。
      另一方面， ``CMakeToolchain`` 为 CMake 生成一个工具链文件，
      这样我们就可以使用与默认配置文件相同的设置透明地使用 CMake 构建项目。
      

Now we are ready to build and run our **compressor** app:

现在，我们已经准备好构建和运行我们的 **compressor** 应用程序:

.. code-block:: bash
    :caption: Windows

    $ cd build
    # assuming Visual Studio 15 2017 is your VS version and that it matches your default profile
    $ cmake .. -G "Visual Studio 15 2017" -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
    $ cmake --build . --config Release
    ...
    [100%] Built target compressor
    $ Release\compressor.exe
    Uncompressed size is: 233
    Compressed size is: 147
    ZLIB VERSION: 1.2.11

.. code-block:: bash
    :caption: Linux, macOS
    
    $ cd build
    $ cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
    $ cmake --build .
    ...
    [100%] Built target compressor
    $ ./compressor
    Uncompressed size is: 233
    Compressed size is: 147
    ZLIB VERSION: 1.2.11


.. _consuming_packages_read_more:

Read more
---------

- :ref:`Getting started with Meson<examples_tools_meson_toolchain_build_simple_meson_project>`
- Getting started with Autotools
- ...
