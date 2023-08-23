.. _consuming_packages_tool_requires:

Using build tools as Conan packages
===================================

In the previous example, we built our CMake project and used Conan to install and locate
the **Zlib** library. Conan used the CMake version found in the system path to build this
example. But, what happens if you don’t have CMake installed in your build environment or
want to build your project with a specific CMake version different from the one you have
already installed system-wide? In this case, you can declare this dependency in Conan
using a type of requirement named ``tool_requires``. Let’s see an example of how to add a
``tool_requires`` to our project, and use a different CMake version to build it.

在前面的示例中，我们构建了 CMake 项目，并使用 Conan 安装和定位 **Zlib** 库。Conan 使用系统路径中的 
CMake 版本构建此示例。但是，如果在构建环境中没有安装 CMake，
或者希望使用与系统范围内已经安装的版本不同的特定 CMake 版本来构建项目，
那么会发生什么情况呢？在这种情况下，您可以使用一种名为 ``tool_requires`` 
的需求类型在 Conan 中声明这种依赖关系。让我们看一个如何向我们的项目添加 
``tool_requires`` 的示例，并使用不同的 CMake 版本来构建它。

Please, first clone the sources to recreate this project. You can find them in the
`examples2.0 repository <https://github.com/conan-io/examples2>`_ in GitHub:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/consuming_packages/tool_requires

The structure of the project is the same as the one of the previous example:

.. code-block:: text

    .
    ├── conanfile.txt
    ├── CMakeLists.txt
    └── src
        └── main.c


The main difference is the addition of the :ref:`reference_config_files_profiles_tool_requires` section in the
**conanfile.txt** file. In this section, we declare that we want to build our application
using CMake **v3.22.6**.


主要区别在于在 **conanfile.txt**  文件中添加了 :ref:`reference_config_files_profiles_tool_requires` 部分。在本节中，我们声明希望使用 **v3.22.6** 构建应用程序。

.. code-block:: ini
    :caption: **conanfile.txt**
    :emphasize-lines: 4,5

    [requires]
    zlib/1.2.11

    [tool_requires]
    cmake/3.22.6

    [generators]
    CMakeDeps
    CMakeToolchain

We also added a message to the *CMakeLists.txt* to output the CMake version:

.. code-block:: cmake
    :caption: **CMakeLists.txt**
    :emphasize-lines: 6

    cmake_minimum_required(VERSION 3.15)
    project(compressor C)

    find_package(ZLIB REQUIRED)

    message("Building with CMake version: ${CMAKE_VERSION}")
    
    add_executable(${PROJECT_NAME} src/main.c)
    target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)

Now, as in the previous example, we will use Conan to install **Zlib** and **CMake
3.22.6** and generate the files to find both of them. We will generate those
files the folder *build*. To do that, just run:

现在，与前面的示例一样，我们将使用 Conan 来安装 **Zlib** 和 **CMake 3.22.6**，
并生成文件来查找它们。我们将生成  *build* 文件夹的那些文件。要做到这一点，只需运行:

.. code-block:: bash

    $ conan install . --output-folder=build --build=missing

You can check the output:

.. code-block:: bash

    -------- Computing dependency graph ----------
    cmake/3.22.6: Not found in local cache, looking in remotes...
    cmake/3.22.6: Checking remote: conancenter
    cmake/3.22.6: Trying with 'conancenter'...
    Downloading conanmanifest.txt
    Downloading conanfile.py
    cmake/3.22.6: Downloaded recipe revision 3e3d8f3a848b2a60afafbe7a0955085a
    Graph root
        conanfile.txt: /Users/user/Documents/developer/conan/examples2/tutorial/consuming_packages/tool_requires/conanfile.txt
    Requirements
        zlib/1.2.11#f1fadf0d3b196dc0332750354ad8ab7b - Cache
    Build requirements
        cmake/3.22.6#3e3d8f3a848b2a60afafbe7a0955085a - Downloaded (conancenter)

    -------- Computing necessary packages ----------
    Requirements
        zlib/1.2.11#f1fadf0d3b196dc0332750354ad8ab7b:2a823fda5c9d8b4f682cb27c30caf4124c5726c8#48bc7191ec1ee467f1e951033d7d41b2 - Cache
    Build requirements
        cmake/3.22.6#3e3d8f3a848b2a60afafbe7a0955085a:f2f48d9745706caf77ea883a5855538256e7f2d4#6c519070f013da19afd56b52c465b596 - Download (conancenter)

    -------- Installing packages ----------

    Installing (downloading, building) binaries...
    cmake/3.22.6: Retrieving package f2f48d9745706caf77ea883a5855538256e7f2d4 from remote 'conancenter'
    Downloading conanmanifest.txt
    Downloading conaninfo.txt
    Downloading conan_package.tgz
    Decompressing conan_package.tgz
    cmake/3.22.6: Package installed f2f48d9745706caf77ea883a5855538256e7f2d4
    cmake/3.22.6: Downloaded package revision 6c519070f013da19afd56b52c465b596
    zlib/1.2.11: Already installed!

    -------- Finalizing install (deploy, generators) ----------
    conanfile.txt: Generator 'CMakeToolchain' calling 'generate()'
    conanfile.txt: Generator 'CMakeDeps' calling 'generate()'
    conanfile.txt: Aggregating env generators

Now, if you check the folder you will see that Conan generated a new
file called ``conanbuild.sh/bat``. This is the result of automatically invoking a
``VirtualBuildEnv`` generator when we declared the ``tool_requires`` in the
**conanfile.txt**. This file sets some environment variables like a new ``PATH`` that
we can use to inject to our environment the location of CMake v3.22.6.

现在，如果你检查文件夹，你会看到Conan生成了一个名为 ``conanbuild.sh/bat``
的新文件。这是在 **conanfile.txt** 中声明 ``tool_requires`` 时自动调用 
``VirtualBuildEnv`` 生成器的结果。这个文件设置了一些环境变量，
比如一个新的 ``PATH``，我们可以使用它将 CMake v3.22.6的位置注入到我们的环境中。

Activate the virtual environment, and run ``cmake --version`` to check that you
have installed the new CMake version in the path.

激活虚拟环境，运行 ``cmake --version`` 以检查路径中是否安装了新的 CMake 版本。

.. code-block:: bash
    :caption: Windows

    $ cd build
    $ conanbuild.bat

.. code-block:: bash
    :caption: Linux, macOS
    
    $ cd build
    $ source conanbuild.sh
    Capturing current environment in deactivate_conanbuildenv-release-x86_64.sh
    Configuring environment variables

Run ``cmake`` and check the version:

.. code-block:: bash
    
    $ cmake --version
    cmake version 3.22.6
    ...

As you can see, after activating the environment, the CMake v3.22.6 binary folder was
added to the path and is the currently active version now. Now you can build your project as
you previously did, but this time Conan will use CMake 3.22.6 to build it:

如您所见，激活环境后，CMake v3.22.6二进制文件夹被添加到路径中，
现在是当前活动版本。现在您可以像以前一样构建您的项目，但是这次 Conan 将使用 CMake 3.22.6 来构建它:

.. code-block:: bash
    :caption: Windows

    # assuming Visual Studio 15 2017 is your VS version and that it matches your default profile
    $ cmake .. -G "Visual Studio 15 2017" -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
    $ cmake --build . --config Release
    ...
    Building with CMake version: 3.22.6
    ...
    [100%] Built target compressor
    $ Release\compressor.exe
    Uncompressed size is: 233
    Compressed size is: 147
    ZLIB VERSION: 1.2.11

.. code-block:: bash
    :caption: Linux, macOS
    
    $ cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
    $ cmake --build .
    ...
    Building with CMake version: 3.22.6
    ...
    [100%] Built target compressor
    $ ./compressor
    Uncompressed size is: 233
    Compressed size is: 147
    ZLIB VERSION: 1.2.11


Note that when we activated the environment, a new file named
``deactivate_conanbuild.sh/bat`` was created in the same folder. If you source this file
you can restore the environment as it was before.

请注意，当我们激活环境时，会在同一个文件夹中创建的一个名为 ``deactivate_conanbuild.sh/bat`` 的新文件。
如果您提供了这个文件的源代码，那么您可以恢复以前的环境。

.. code-block:: bash
    :caption: Windows
    
    $ deactivate_conanbuild.bat

.. code-block:: bash
    :caption: Linux, macOS
    
    $ source deactivate_conanbuild.sh
    Restoring environment


Run ``cmake`` and check the version, it will be the version that was installed previous to
the environment activation:

.. code-block:: bash
    
    $ cmake --version
    cmake version 3.22.0
    ...


.. note::

    **Best practice**

    ``tool_requires`` and tool packages are intended for executable applications, like ``cmake`` or ``ninja``. Do not
    use ``tool_requires`` to depend on library or library-like dependencies.

    ``tool_requires`` 和 tool 包是为可执行应用程序(如  ``cmake``  或 ``ninja``)准备的。不要使用 ``tool_requires`` 来依赖于库或类似库的依赖项。


Read more
---------

- :ref:`Using [system_tools] in your profiles <reference_config_files_profiles_system_tools>`.
- :ref:`Creating recipes for tool_requires: packaging build tools <tutorial_other_tool_requires_packages>`.
- :ref:`examples_graph_tool_requires_protobuf`
- Using MinGW as tool_requires
- Using tool_requires in profiles
- Using conf to set a toolchain from a tool requires
