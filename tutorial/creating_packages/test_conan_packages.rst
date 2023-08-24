.. _tutorial_creating_test:

Testing Conan packages
======================

In all the previous sections of the tutorial, we used the *test_package*. It was invoked
automatically at the end of the ``conan create`` command after building our package
verifying that the package is created correctly. Let's explain the *test_package* in more
detail in this section:

在本教程前面的所有部分中，我们都使用了 *test_package*。在构建包并验证包是否正确创建之后，在 
``conan create`` 命令的末尾自动调用了它。让我们在本节中更详细地解释 *test_package*:

Please, first clone the sources to recreate this project. You can find them in the
`examples2.0 repository <https://github.com/conan-io/examples2>`_ on GitHub:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/creating_packages/testing_packages


Some important notes to have in mind about the *test_package*:

关于 *test_package* 需要记住的一些重要注意事项:

* The *test_package* folder is different from unit or integration tests. These tests are
  “package” tests, and validate that the package is properly created, and that the package
  consumers will be able to link against it and reuse it.

  *test_package* 文件夹不同于单元测试或集成测试。这些测试是“包”测试，并验证包是否正确创建，
  以及包使用者是否能够对其进行链接并重用它。

* It is a small Conan project itself, it contains its own *conanfile.py*, and its source
  code including build scripts, that depends on the package being created, and builds and
  execute a small application that requires the library in the package.

  它本身是一个很小的 Conan 项目，它包含自己的 *conanfile.py* 和包含构建脚本的源代码，
  这些源代码依赖于创建的包，并构建和执行一个需要包中的库的小应用程序。

* It doesn’t belong to the package. It only exist in the source repository, not in the
  package.

  它不属于包，它只存在于源代码库中，而不在包中。

The *test_package* folder for our hello/1.0 Conan package has the following contents:

Hello/1.0 Conan 包的 *test_package* 文件夹包含以下内容:

.. code-block:: text

   test_package
    ├── CMakeLists.txt
    ├── conanfile.py
    └── src
        └── example.cpp

Let's have a look at the different files that are part of the *test_package*. First,
*example.cpp* is just a minimal example of how to use the *libhello* library that we are
packaging:

让我们看看 *test_package* 中的不同文件。首先，*example.cpp* 只是一个简单的例子，
说明如何使用我们打包的 *libhello* 库:

.. code-block:: cpp
    :caption: *test_package/src/example.cpp*

    #include "hello.h"

    int main() {
        hello();
    }

Then the *CMakeLists.txt* file to tell CMake how to build the example:

然后是 *CMakeLists.txt* 文件，告诉 CMake 如何构建示例:

.. code-block:: cpp
    :caption: *test_package/src/example.cpp*

    cmake_minimum_required(VERSION 3.15)
    project(PackageTest CXX)

    find_package(hello CONFIG REQUIRED)

    add_executable(example src/example.cpp)
    target_link_libraries(example hello::hello)

Finally, the recipe for the *test_package* that consumes the *hello/1.0* Conan package:

最后，使用 *hello/1.0* Conan 包的 *test_package* 的配方是:

.. code-block:: python
    :caption: *test_package/conanfile.py*

    import os

    from conan import ConanFile
    from conan.tools.cmake import CMake, cmake_layout
    from conan.tools.build import can_run


    class helloTestConan(ConanFile):
        settings = "os", "compiler", "build_type", "arch"
        generators = "CMakeDeps", "CMakeToolchain"

        def requirements(self):
            self.requires(self.tested_reference_str)

        def build(self):
            cmake = CMake(self)
            cmake.configure()
            cmake.build()

        def layout(self):
            cmake_layout(self)

        def test(self):
            if can_run(self):
                cmd = os.path.join(self.cpp.build.bindir, "example")
                self.run(cmd, env="conanrun")

Let's go through the most relevant parts:

让我们来看看最相关的部分:

* We add the requirements in the ``requirements()`` method, but in this case we use the
  ``tested_reference_str`` attribute that Conan sets to pass to the test_package. This is
  a convenience attribute to avoid hardcoding the package name in the test_package so that
  we can reuse the same test_package for several versions of the same Conan package. In
  our case, this variable will take the ``hello/1.0`` value.

  我们在 ``requirements()`` 方法中添加了需求，但是在本例中，我们使用了由 Conan 设置的 
  ``tested_reference_str`` 属性来传递给 *test_package*。这是一个方便的属性，可以避免在 
  *test_package* 中硬编码包名，这样我们就可以对同一个 Conan 包的几个版本重用同一个 
  *test_package*。在我们的示例中， 这个变量将采用 ``hello/1.0`` 值。

* We define a ``test()`` method. This method will only be invoked in the *test_package*
  recipes. It executes immediately after ``build()`` is called, and it's meant to run some
  executable or tests on binaries to prove the package is correctly created. A couple of
  comments about the contents of our ``test()`` method:

  我们定义一个 ``test()`` 方法。这个方法只能在 *test_package* 配方中调用。它在调用 
  ``build()`` 之后立即执行，  并且它意味着在二进制文件上运行一些可执行文件或测试，
  以证明包是正确创建的。 关于 ``test()`` 方法内容的一些注释:
  
  - We are using the :ref:`conan.tools.build.cross_building<conan_tools_build_can_run>`
    tool to check if we can run the built executable in our platform. This tool will
    return the value of the ``tools.build.cross_building:can_run`` in case it's set.
    Otherwise it will return if we are cross-building or not. It’s an useful feature for
    the case your architecture can run more than one target. For instance, Mac M1 machines
    can run both *armv8* and *x86_64*.

    我们使用  :ref:`conan.tools.build.cross_building<conan_tools_build_can_run>`
    工具检查是否可以在平台中运行构建的可执行文件。如果设置了这个工具，它将返回 
    ``tools.build.cross_building:can_run`` 的值。否则，无论我们是否交叉建设，它都会回来。
    对于体系结构可以运行多个目标的情况，这是一个非常有用的特性。例如，
    Mac M1机器可以同时运行 *armv8* 和 *x86_64*。

  - We run the example binary, that was generated in the ``self.cpp.build.bindir`` folder
    using the environment information that Conan put in the run environment. Conan will
    then invoke a launcher containing the runtime environment information, anything that
    is necessary for the environment to run the compiled executables and applications.

    我们运行生成在 ``self.cpp.build.bindir`` 文件夹中的示例二进制文件(使用 Conan 放在 run 环境中的环境信息)。
    然后，Conan将调用一个包含执行期函式库信息的启动程序，这些信息是环境运行已编译的可执行文件和应用程序所必需的。

Now that we have gone through all the important bits of the code, let's try our
*test_package*. Although we already learned that the *test_package* is invoked when we
call to ``conan create``, you can also just create the *test_package* if you have already
created the ``hello/1.0`` package in the Conan cache. This is done with the :ref:`conan
test<reference_commands>` command:

现在我们已经完成了代码的所有重要部分，让我们尝试一下 *test_package*。尽管我们已经了解到在调用 
``conan create`` 时调用 *test_package*，但是如果您已经在 Conan 缓存中创建了 ``hello/1.0`` 包，
那么您也可以创建 *test_package*。这是使用 :ref:`conan test<reference_commands>` 命令完成的:

.. code-block:: bash
    :emphasize-lines: 18, 21

    $ conan test test_package hello/1.0

    ...

    -------- test_package: Computing necessary packages --------
    Requirements
        fmt/8.1.1#cd132b054cf999f31bd2fd2424053ddc:ff7a496f48fca9a88dc478962881e015f4a5b98f#1d9bb4c015de50bcb4a338c07229b3bc - Cache
        hello/1.0#25e0b5c00ae41ef9fbfbbb1e5ac86e1e:fd7c4113dad406f7d8211b3470c16627b54ff3af#4ff3fd65a1d37b52436bf62ea6eaac04 - Cache
    Test requirements
        gtest/1.11.0#d136b3379fdb29bdfe31404b916b29e1:656efb9d626073d4ffa0dda2cc8178bc408b1bee#ee8cbd2bf32d1c89e553bdd9d5606127 - Skip
 
    ...

    [ 50%] Building CXX object CMakeFiles/example.dir/src/example.cpp.o
    [100%] Linking CXX executable example
    [100%] Built target example

    -------- Testing the package: Running test() --------
    hello/1.0 (test package): Running test()
    hello/1.0 (test package): RUN: ./example
    hello/1.0: Hello World Release! (with color!)

As you can see in the output, our *test_package* builds successfully testing that the
*hello/1.0* Conan package can be consumed with no problem.

正如您在输出中看到的，我们的 *test_package* 构建成功地测试了 *hello/1.0* Conan 包是否可以毫无问题地使用。

Read more
---------

- Test *tool_requires* packages
- ...