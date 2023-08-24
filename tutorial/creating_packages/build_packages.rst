.. _tutorial_creating_build:

Build packages: the build() method
==================================

We already used a Conan recipe that has a :ref:`build() method<reference_conanfile_methods_build>` and learned how to use that
to invoke a build system and build our packages. In this tutorial, we will modify that
method and explain how you can use it to do things like:

我们已经使用了具有 :ref:`build() method<reference_conanfile_methods_build>` 方法的 Conan 配方，并学习了如何使用该方法调用构建系统和构建包。
在本教程中，我们将修改这个方法，并解释如何使用它来做这样的事情:

* Building and running tests
  
  生成和运行测试

* Conditional patching of the source code

  源代码的有条件修补

* Select the build system you want to use conditionally

  选择要有条件使用的生成系统

Please, first clone the sources to recreate this project. You can find them in the
`examples2.0 repository <https://github.com/conan-io/examples2>`_ on GitHub:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/creating_packages/build_method


Build and run tests for your project
------------------------------------

You will notice some changes in the **conanfile.py** file from the previous recipe.
Let's check the relevant parts:

您将注意到在 **conanfile.py** 文件中与前面的配方相比发生了一些变化，让我们检查相关部分:

Changes introduced in the recipe
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python
    :caption: *conanfile.py*
    :emphasize-lines: 12, 19, 33-34

    class helloRecipe(ConanFile):
        name = "hello"
        version = "1.0"

        ...

        def source(self):
            git = Git(self)
            git.clone(url="https://github.com/conan-io/libhello.git", target=".")
            # Please, be aware that using the head of the branch instead of an immutable tag
            # or commit is not a good practice in general
            git.checkout("with_tests")

        ...

        def requirements(self):
            if self.options.with_fmt:
                self.requires("fmt/8.1.1")
            self.test_requires("gtest/1.11.0")

        ...

        def generate(self):
            tc = CMakeToolchain(self)
            if self.options.with_fmt:
                tc.variables["WITH_FMT"] = True
            tc.generate()

        def build(self):
            cmake = CMake(self)
            cmake.configure()
            cmake.build()
            if not self.conf.get("tools.build:skip_test", default=False):
                test_folder = os.path.join("tests")
                if self.settings.os == "Windows":
                    test_folder = os.path.join("tests", str(self.settings.build_type))
                self.run(os.path.join(test_folder, "test_hello"))

        ...

* We added the *gtest/1.11.0* requirement to the recipe as a ``test_requires()``. It's a
  type of requirement intended for testing libraries like **Catch2** or **gtest**.

  我们在菜谱中添加了 *gtest/1.11.0* 需求作为 ``test_requires()``。
  它是一种用于测试类似 **Catch2** 或 **gtest** 的库的需求类型。

* We use the ``tools.build:skip_test`` configuration (``False`` by default), to tell CMake
  whether to build and run the tests or not. A couple of things to bear in mind:

  我们使用 ``tools.build:skip_test`` 配置(默认为 ``False``)来告诉 CMake 是否构建和运行测试。请记住以下几点:
 
  - If we set the ``tools.build:skip_test`` configuration to ``True`` Conan will
    automatically inject the ``BUILD_TESTING`` variable to CMake set to ``OFF``. You will
    see in the next section that we are using this variable in our *CMakeLists.txt* to
    decide wether to build the tests or not.
    
    如果我们将 ``tools.build:skip_test`` 配置设置为 ``True`` Conan 将自动将 ``BUILD_TESTING`` 
    变量注入到 CMake 中，并将其设置为  ``OFF``。在下一节中，您将看到我们在 *CMakeLists.txt* 
    中使用这个变量来决定是否构建测试。
  
  - We use the ``tools.build:skip_test`` configuration in the ``build()`` method,
    after building the package and tests, to decide if we want to run the tests or not.

    在构建包和测试之后，我们在 ``build()`` 方法中使用  ``tools.build:skip_test`` 配置来决定是否要运行测试。
  
  - In this case we are using **gtest** for testing and we have to add the check if the
    build method to run the tests or not, but this configuration also affects the
    execution of ``CMake.test()`` if you are using CTest and ``Meson.test()`` for Meson.

    在这种情况下，我们使用 **gtest**  进行测试，并且我们必须添加检查是否要运行测试的 build 方法，
    但是如果您使用 CTest 和 ``Meson.test()`` 进行 Meson，这个配置也会影响  ``CMake.test()`` 的执行。
  

Changes introduced in the library sources
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

First, please note that we are using `another branch
<https://github.com/conan-io/libhello/tree/with_tests>`_ from the **libhello** library. This
branch has two novelties on the library side:

首先，请注意我们正在使用 libhello 库中的  `another branch <https://github.com/conan-io/libhello/tree/with_tests>`_ 。
这个分支在图书馆方面有两个新颖之处:

* We added a new function called ``compose_message()`` to the `library sources
  <https://github.com/conan-io/libhello/blob/with_tests/src/hello.cpp#L9-L12>`_ so we can add
  some unit tests over this function. This function is just creating an output message
  based on the arguments passed.

  我们在 `library sources <https://github.com/conan-io/libhello/blob/with_tests/src/hello.cpp#L9-L12>`_ 
  中添加了一个名为 ``compose_message()`` 的新函数，这样我们就可以在该函数上添加一些单元测试。
  这个函数只是根据传递的参数创建一个输出消息。

* As we mentioned in the previous section the `CMakeLists.txt for the library
  <https://github.com/conan-io/libhello/blob/with_tests/CMakeLists.txt#L15-L17>`_ uses the
  ``BUILD_TESTING`` CMake variable that conditionally adds the *tests* directory.

  正如我们在前一节中提到的， `CMakeLists.txt for the library <https://github.com/conan-io/libhello/blob/with_tests/CMakeLists.txt#L15-L17>`_
  使用 ``BUILD_TESTING`` CMake 变量，该变量有条件地添加测试目录。

.. code-block:: text
    :caption: *CMakeLists.txt*

    cmake_minimum_required(VERSION 3.15)
    project(hello CXX)

    ...

    if (NOT BUILD_TESTING STREQUAL OFF)
        add_subdirectory(tests)
    endif()

    ...

The ``BUILD_TESTING`` `CMake variable
<https://cmake.org/cmake/help/latest/module/CTest.html>`_ is declared and set to ``OFF``
by Conan (if not already defined) whenever the ``tools.build:skip_test`` configuration is
set to value ``True``. This variable is typically declared by CMake when you use CTest but
using the ``tools.build:skip_test`` configuration you can use it in your *CMakeListst.txt*
even if you are using another testing framework.

每当 ``tools.build:skip_test`` 配置设置为值 ``True`` 时，Conan 都会声明 
``BUILD_TESTING`` `CMake variable <https://cmake.org/cmake/help/latest/module/CTest.html>`_
并将其设置为 ``OFF`` (如果尚未定义)。这个变量通常在使用 CTest 时由 CMake 声明，
但是使用 ``tools.build:skip_test`` 配置时，即使使用其他测试框架，也可以在 *CMakeListst.txt* 中使用它。

* We have a `CMakeLists.txt
  <https://github.com/conan-io/libhello/blob/with_tests/tests/CMakeLists.txt>`_ in the
  *tests* folder using `googletest <https://github.com/google/googletest>`_ for
  testing.

  我们在 *tests* 文件夹中有一个 `CMakeLists.txt  <https://github.com/conan-io/libhello/blob/with_tests/tests/CMakeLists.txt>`_，
  使用 `googletest <https://github.com/google/googletest>`_  进行测试。

.. code-block:: cmake
    :caption: *tests/CMakeLists.txt*

    cmake_minimum_required(VERSION 3.15)
    project(PackageTest CXX)

    find_package(GTest REQUIRED CONFIG)

    add_executable(test_hello test.cpp)
    target_link_libraries(test_hello GTest::gtest GTest::gtest_main hello)


With basic tests on the functionality of the ``compose_message()`` function:

通过对 ``compose_message()`` 函数功能的基本测试:

.. code-block:: cpp
    :caption: *tests/test.cpp*

    #include "../include/hello.h"
    #include "gtest/gtest.h"

    namespace {
        TEST(HelloTest, ComposeMessages) {
        EXPECT_EQ(std::string("hello/1.0: Hello World Release! (with color!)\n"), compose_message("Release", "with color!"));
        ...
        }
    }

Now that we have gone through all the changes in the code, let's try them out:

现在我们已经完成了代码中的所有更改，让我们尝试一下:

.. code-block:: bash
    :emphasize-lines: 6-23

    $ conan create . --build=missing -tf=None
    ...
    [ 25%] Building CXX object CMakeFiles/hello.dir/src/hello.cpp.o
    [ 50%] Linking CXX static library libhello.a
    [ 50%] Built target hello
    [ 75%] Building CXX object tests/CMakeFiles/test_hello.dir/test.cpp.o
    [100%] Linking CXX executable test_hello
    [100%] Built target test_hello
    hello/1.0: RUN: ./tests/test_hello
    Capturing current environment in /Users/user/.conan2/p/tmp/c51d80ef47661865/b/build/generators/deactivate_conanbuildenv-release-x86_64.sh
    Configuring environment variables
    Running main() from /Users/user/.conan2/p/tmp/3ad4c6873a47059c/b/googletest/src/gtest_main.cc
    [==========] Running 1 test from 1 test suite.
    [----------] Global test environment set-up.
    [----------] 1 test from HelloTest
    [ RUN      ] HelloTest.ComposeMessages
    [       OK ] HelloTest.ComposeMessages (0 ms)
    [----------] 1 test from HelloTest (0 ms total)

    [----------] Global test environment tear-down
    [==========] 1 test from 1 test suite ran. (0 ms total)
    [  PASSED  ] 1 test.
    hello/1.0: Package '82b6c0c858e739929f74f59c25c187b927d514f3' built
    ...

As you can see, the tests were built and run. Let's use now the ``tools.build:skip_test``
configuration in the command line to skip the test building and running:

如您所见，测试已经构建并运行。现在让我们在命令行中使用  ``tools.build:skip_test`` 配置来跳过测试构建和运行:

.. code-block:: bash

    $ conan create . -c tools.build:skip_test=True -tf=None
    ...
    [ 50%] Building CXX object CMakeFiles/hello.dir/src/hello.cpp.o
    [100%] Linking CXX static library libhello.a
    [100%] Built target hello
    hello/1.0: Package '82b6c0c858e739929f74f59c25c187b927d514f3' built
    ...


You can see now that only the library target was built and that no tests were built or
run.

现在您可以看到只生成了库目标，并且没有生成或运行任何测试。

Conditionally patching the source code
--------------------------------------

If you need to patch the source code the recommended approach is to do that in the
``source()`` method. Sometimes, if that patch depends on settings or options, you have
to use the ``build()`` method to apply patches to the source code before launching the
build. There are :ref:`several ways to do this <examples_tools_files_patches>` in Conan.
One of them would be using the :ref:`replace_in_file <conan_tools_files_replace_in_file>`
tool:

如果需要修补源代码，推荐的方法是在 ``source()`` 方法中这样做。有时，如果补丁依赖于设置或选项，
那么在启动构建之前，必须使用 ``build()`` 方法将补丁应用于源代码。在Conan中有 
:ref:`several ways to do this <examples_tools_files_patches>` 。
其中之一就是使用 :ref:`replace_in_file <conan_tools_files_replace_in_file>` 工具:

.. code-block:: python

    import os
    from conan import ConanFile
    from conan.tools.files import replace_in_file


    class helloRecipe(ConanFile):
        name = "hello"
        version = "1.0"

        # Binary configuration
        settings = "os", "compiler", "build_type", "arch"
        options = {"shared": [True, False], "fPIC": [True, False]}
        default_options = {"shared": False, "fPIC": True}

        def build(self):
            replace_in_file(self, os.path.join(self.source_folder, "src", "hello.cpp"), 
                            "Hello World", 
                            "Hello {} Friends".format("Shared" if self.options.shared else "Static"))


Please, note that patching in ``build()`` should avoided if possible and only be done for
very particular cases as it will make more difficult to develop your packages locally (we
will explain more about this in the local developement flow section later <MISSING REFERENCE>)

请注意，如果可能的话，应该避免 ``build()`` 中的补丁，并且只在非常特殊的情况下进行，
因为这将使得在本地开发软件包变得更加困难(我们将在后面的本地开发流程部分中详细解释这一点 <MISSING REFERENCE>)

Conditionally select your build system
--------------------------------------

It's not uncommon that some packages need one build system or another depending on the
platform we are building. For example, the *hello* library could build in Windows using
CMake and in Linux and MacOS using Autotools. This can be easily handled in the
``build()`` method like this:

根据我们正在构建的平台，有些软件包需要一个或另一个构建系统，这种情况并不少见。例如， *hello*  
库可以在 Windows 中使用 CMake 构建，在 Linux 和 MacOS 中使用 Autotools 构建。
在 ``build()`` 方法中可以像下面这样轻松地处理这个问题:

.. code-block:: python

    ...

    class helloRecipe(ConanFile):
        name = "hello"
        version = "1.0"

        # Binary configuration
        settings = "os", "compiler", "build_type", "arch"
        options = {"shared": [True, False], "fPIC": [True, False]}
        default_options = {"shared": False, "fPIC": True}

        ...

        def generate(self):
            if self.settings.os == "Windows":
                tc = CMakeToolchain(self)
                tc.generate()
                deps = CMakeDeps(self)
                deps.generate()
            else:
                tc = AutotoolsToolchain(self)
                tc.generate()
                deps = PkgConfigDeps(self)
                deps.generate()

        ...

        def build(self):
            if self.settings.os == "Windows":
                cmake = CMake(self)
                cmake.configure()
                cmake.build()
            else:
                autotools = Autotools(self)
                autotools.autoreconf()
                autotools.configure()
                autotools.make()

        ...


Read more
---------

- :ref:`Patching sources <examples_tools_files_patches>`
- ...
