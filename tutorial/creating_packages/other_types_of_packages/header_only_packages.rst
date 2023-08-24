.. _creating_packages_other_header_only:

Header-only packages
====================

In this section, we are going to learn how to create a recipe for a header-only library.

在本节中，我们将学习如何创建一个 header-only 库的配方。

Please, first clone the sources to recreate this project. You can find them in the
`examples2.0 repository <https://github.com/conan-io/examples2>`_ on GitHub:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/creating_packages/other_packages/header_only


A header-only library is composed only of header files. That means a consumer doesn't link with any library but
includes headers, so we need only one binary configuration for a header-only library.

header-only 库只由头文件组成。这意味着使用者不会链接到任何库，而是包含头文件，因此对于 header-only库，我们只需要一个二进制配置。

In the :ref:`Create your first Conan package
<creating_packages_create_your_first_conan_package>` section, we learned about the settings, and how building the
recipe applying different ``build_type`` (Release/Debug) generates a new binary package.

在:ref:`Create your first Conan package <creating_packages_create_your_first_conan_package>` 部分，
我们了解了设置，以及应用不同 ``build_type`` (Release/Debug)构建菜谱如何生成一个新的二进制包。

As we only need one binary package, we don't need to declare the `settings` attribute.
This is a basic recipe for a header-only recipe:

因为我们只需要一个二进制包，所以不需要声明 `settings` 属性。这是一个 header-only 的基本配方:

.. code-block:: python
   :caption: conanfile.py


    from conan import ConanFile
    from conan.tools.files import copy


    class SumConan(ConanFile):
        name = "sum"
        version = "0.1"
        # No settings/options are necessary, this is header only
        exports_sources = "include/*"
        # We can avoid copying the sources to the build folder in the cache
        no_copy_source = True

        def package(self):
            # This will also copy the "include" folder
            copy(self, "*.h", self.source_folder, self.package_folder)

        def package_info(self):
            # For header-only packages, libdirs and bindirs are not used
            # so it's necessary to set those as empty.
            self.cpp_info.bindirs = []
            self.cpp_info.libdirs = []

Please, note that we are setting ``cpp_info.bindirs`` and ``cpp_info.libdirs`` to ``[]`` because
header-only libraries don't have compiled libraries or binaries, but they default to ``["bin"]``, and ``["lib"]``, then it is necessary to change it.

请注意，我们正在设置 ``cpp_info.bindirs`` 和 ``cpp_info.libdirs`` 为 ``[]``， 因为 header-only  库没有已编译的库或二进制文件，
但是它们默认为 ``["bin"]`` 和 ``["lib"]`` ，所以有必要更改它。

Our header-only library is this simple function that sums two numbers:

我们的 header-only 题库是这个简单的函数，它总结了两个数字:

.. code-block:: cpp
   :caption: include/sum.h

    inline int sum(int a, int b){
        return a + b;
    }


The folder `examples2/tutorial/creating_packages/other_packages/header_only` in the cloned project contains a ``test_package``
folder with an example of an application consuming the header-only library. So we can run a ``conan create .`` command
to build the package and test the package:

克隆的项目中的文件夹 `examples2/tutorial/creating_packages/other_packages/header_only` 包含一个 
``test_package`` 文件夹，其中有一个应用程序使用 header-only 库的示例。所以我们可以运行一个 ``conan create .`` 命令来构建包并测试包:

.. code-block:: bash

    $ conan create .
    ...
    [ 50%] Building CXX object CMakeFiles/example.dir/src/example.cpp.o
    [100%] Linking CXX executable example
    [100%] Built target example

    -------- Testing the package: Running test() ----------
    sum/0.1 (test package): Running test()
    sum/0.1 (test package): RUN: ./example
    1 + 3 = 4

After running the ``conan create`` a new binary package is created for the header-only library, and we can see how the
``test_package`` project can use it correctly.

在运行了  ``conan create``  之后，为 header-only 库创建了一个新的二进制包，我们可以看到 ``test_package`` 项目如何正确地使用它。

We can list the binary packages created running this command:

我们可以列出运行以下命令创建的二进制包:

.. code-block:: bash

    $ conan list sum/0.1#:*
    Local Cache:
    sum
        sum/0.1#8d9f1fb3655adcb348befcd8374c5292 (2022-12-22 17:33:45 UTC)
        PID: da39a3ee5e6b4b0d3255bfef95601890afd80709 (2022-12-22 17:33:45 UTC)
            No package info/revision was found.

We get one package with the package ID ``da39a3ee5e6b4b0d3255bfef95601890afd80709``.
Let's see what happen if we run the ``conan create`` but specifying ``-s build_type=Debug``:

我们得到一个包 ID 为 ``da39a3ee5e6b4b0d3255bfef95601890afd80709`` 的包。让我们看看如果运行 
``conan create`` 但指定 ``-s build_type=Debug`` 会发生什么:

.. code-block:: bash

    $ conan create . -s build_type=Debug
    $ conan list sum/0.1#:*
    Local Cache:
    sum
        sum/0.1#8d9f1fb3655adcb348befcd8374c5292 (2022-12-22 17:34:23 UTC)
        PID: da39a3ee5e6b4b0d3255bfef95601890afd80709 (2022-12-22 17:34:23 UTC)
            No package info/revision was found.

Even in the ``test_package`` executable is built for Debug, we get the same binary package for the header-only library.
This is because we didn't specify the ``settings`` attribute in the recipe, so the changes in the input settings (``-s build_type=Debug``)
do not affect the recipe and therefore the generated binary package is always the same.

即使在为 Debug 构建的 ``test_package`` 可执行文件中，我们也可以为 header-only 库获得相同的二进制包。
这是因为我们没有在配方中指定 ``settings`` 属性，所以输入设置(``-s build_type=Debug``)
中的更改不会影响配方，因此生成的二进制包总是相同的。

Header-only library with tests
------------------------------

In the previous example, we saw why a recipe header-only library shouldn't declare the ``settings`` attribute,
but sometimes the recipe needs them to build some executable, for example, for testing the library.
Nonetheless, the binary package of the header-only library should still be unique, so we are going to review how to
achieve that.

在前面的示例中，我们了解了为什么只有菜谱头的库不应该声明 ``settings`` 属性，
但是有时候配方需要它们来构建一些可执行文件，例如，用于测试库。尽管如此，仅标头库的二进制包仍然应该是唯一的，
因此我们将回顾如何实现这一点。


Please, first clone the sources to recreate this project. You can find them in the
`examples2.0 repository <https://github.com/conan-io/examples2>`_ on GitHub:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/creating_packages/other_packages/header_only_gtest

We have the same header-only library that sums two numbers, but now we have this recipe:

我们有相同的只包含头部的库，可以对两个数字求和，但是现在我们有了这个配方:

.. code-block:: python

    import os
    from conan import ConanFile
    from conan.tools.files import copy
    from conan.tools.cmake import cmake_layout, CMake


    class SumConan(ConanFile):
        name = "sum"
        version = "0.1"
        settings = "os", "arch", "compiler", "build_type"
        exports_sources = "include/*", "test/*"
        no_copy_source = True
        generators = "CMakeToolchain", "CMakeDeps"

        def requirements(self):
            self.test_requires("gtest/1.11.0")

        def validate(self):
            check_min_cppstd(self, 11)

        def layout(self):
            cmake_layout(self)

        def build(self):
            if not self.conf.get("tools.build:skip_test", default=False):
                cmake = CMake(self)
                cmake.configure(build_script_folder="test")
                cmake.build()
                self.run(os.path.join(self.cpp.build.bindir, "test_sum"))

        def package(self):
            # This will also copy the "include" folder
            copy(self, "*.h", self.source_folder, self.package_folder)

        def package_info(self):
            # For header-only packages, libdirs and bindirs are not used
            # so it's necessary to set those as empty.
            self.cpp_info.bindirs = []
            self.cpp_info.libdirs = []

        def package_id(self):
            self.info.clear()




These are the changes introduced in the recipe:

以下是配方中引入的变化:

    - We are introducing a ``test_require`` to ``gtest/1.11.0``. A ``test_require`` is similar to a regular requirement
      but it is not propagated to the consumers and cannot conflict.

      我们正在向 ``gtest/1.11.0`` 引入 ``test_require`` 。 ``test_require`` 类似于常规的需求，但是它不会传播给使用者，也不会冲突.

    - ``gtest`` needs at least C++11 to build. So we introduced a ``validate()`` method calling ``check_min_cppstd``.

      ``gtest`` 至少需要 C++11 来构建。因此，我们引入了一个名为 ``check_min_cppstd`` 的  ``validate()`` 方法。

    - As we are building the ``gtest`` examples with CMake, we use the generators ``CMakeToolchain`` and ``CMakeDeps``,
      and we declared the ``cmake_layout()`` to have a known/standard directory structure.

      当我们使用 CMake 构建 ``gtest`` 示例时，我们使用生成器 ``CMakeToolchain`` 和 ``CMakeDeps``，并且我们声明 ``cmake_layout()`` 
      具有一个已知/标准的目录结构。

    - We have a ``build()`` method, building the tests, but only when the standard conf ``tools.build:skip_test`` is not
      True. Use that conf as a standard way to enable/disable the testing. It is used by the helpers like ``CMake`` to
      skip the ``cmake.test()`` in case we implement the tests in CMake.

      我们有一个 ``build()`` 方法来构建测试，但只有当标准的 ``tools.build:skip_test`` 不是 True 时才使用。使用 conf 作为启用/禁用测试的标准方法。
      如果我们在 ``CMake`` 中实现测试，CMake 这样的助手会使用它来跳过 ``cmake.test()``。

    - We have a ``package_id()`` method calling ``self.info.clear()``. This is internally removing the settings
      from the package ID calculation so we generate only one configuration for our header-only library.

      我们有一个 ``package_id()`` 方法调用 ``self.info.clear()``。这是在内部从包 ID 计算中删除设置，因此我们只为 header-only 库生成一个配置。

We can call ``conan create`` to build and test our package.

我们可以调用 ``conan create`` 来构建和测试我们的包。

   .. code-block:: bash

         $ conan create . -s compiler.cppstd=14 --build missing
         ...
         Running main() from /Users/luism/.conan2/p/tmp/9bf83ef65d5ff0d6/b/googletest/src/gtest_main.cc
         [==========] Running 1 test from 1 test suite.
         [----------] Global test environment set-up.
         [----------] 1 test from SumTest
         [ RUN      ] SumTest.BasicSum
         [       OK ] SumTest.BasicSum (0 ms)
         [----------] 1 test from SumTest (0 ms total)

         [----------] Global test environment tear-down
         [==========] 1 test from 1 test suite ran. (0 ms total)
         [  PASSED  ] 1 test.
         sum/0.1: Package 'da39a3ee5e6b4b0d3255bfef95601890afd80709' built
         ...

We can run ``conan create`` again specifying a different ``compiler.cppstd`` and the built package would be the same:

我们可以再次运行 ``conan create`` ，指定一个不同的 ``compiler.cppstd`` ，构建的包将是相同的:

   .. code-block:: bash

         $ conan create . -s compiler.cppstd=17
         ...
         sum/0.1: RUN: ./test_sum
         Running main() from /Users/luism/.conan2/p/tmp/9bf83ef65d5ff0d6/b/googletest/src/gtest_main.cc
         [==========] Running 1 test from 1 test suite.
         [----------] Global test environment set-up.
         [----------] 1 test from SumTest
         [ RUN      ] SumTest.BasicSum
         [       OK ] SumTest.BasicSum (0 ms)
         [----------] 1 test from SumTest (0 ms total)

         [----------] Global test environment tear-down
         [==========] 1 test from 1 test suite ran. (0 ms total)
         [  PASSED  ] 1 test.
         sum/0.1: Package 'da39a3ee5e6b4b0d3255bfef95601890afd80709' built

   .. note::

      Once we have the ``sum/0.1`` binary package available (in a server, after a ``conan upload``, or in the local cache),
      we can install it even if we don't specify input values for ``os``, ``arch``, ... etc. This is a new feature of Conan 2.X.

      一旦我们有了 ``sum/0.1`` 二进制包(在服务器中，在  ``conan upload`` 之后，或者在本地缓存中) ，我们可以安装它，即使我们没有为 
      ``os``, ``arch``, ... 等指定值。这是Conan2.X的一个新特性。

      We could call ``conan install --require sum/0.1`` with an empty profile and would get the binary package from the
      server. But if we miss the binary and we need to build the package again, it will fail because of the lack of
      settings.

      我们可以使用空配置文件调用  ``conan install --require sum/0.1`` ，并从服务器获取二进制包。但是，如果我们错过了二进制文件，
      并且需要重新构建包，那么由于缺少设置，它将失败。
