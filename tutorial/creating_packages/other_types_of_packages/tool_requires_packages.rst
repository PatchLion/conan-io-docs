.. _tutorial_other_tool_requires_packages:


Tool requires packages
======================

In the ":ref:`Using build tools as Conan packages <consuming_packages_tool_requires>`" section we learned how to use
a tool require to build (or help building) our project or Conan package.
In this section we are going to learn how to create a recipe for a tool require.

在 ":ref:`Using build tools as Conan packages <consuming_packages_tool_requires>`" 部分，
我们学习了如何使用构建(或帮助构建)我们的项目或 Conan 包所需的工具。在本节中，我们将学习如何为工具需求创建配方。

.. note::

    **Best practice**

    ``tool_requires`` and tool packages are intended for executable applications, like ``cmake`` or ``ninja`` that
    can be used as ``tool_requires("cmake/[>=3.25]")`` by other packages to put those executables in their path. They
    are not intended for library-like dependencies (use ``requires`` for them), for test frameworks (use ``test_requires``)
    or in general for anything that belongs to the "host" context of the final application. Do not abuse ``tool_requires``
    for other purposes.

    ``tool_requires`` 和 tool 包是为可执行应用程序准备的，比如 ``cmake`` 或者 ``ninja``，它们可以被其他包用作 ``tool_requires("cmake/[>=3.25]")`` ，
    将这些可执行程序放在它们的路径中。它们不适用于类似库的依赖项(对它们的使用 ``requires`` )、测试框架(使用  ``test_requires``)或一般来说
    属于最终应用程序的“主机”上下文的任何东西。不要为了其他目的滥用 ``tool_requires``。
    

Please, first clone the sources to recreate this project. You can find them in the
`examples2.0 repository <https://github.com/conan-io/examples2>`_ on GitHub:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/creating_packages/other_packages/tool_requires/tool


A simple tool require recipe
----------------------------

This is a recipe for a (fake) application that receiving a path returns 0 if the path is secure.
We can check how the following simple recipe covers most of the ``tool-require`` use-cases:

这是一个(假)应用程序的配方，如果路径是安全的，那么接收路径将返回0。
我们可以检查下面的简单配方是如何涵盖大多数 ``tool-require`` 用例的:

.. code-block:: python
    :caption: conanfile.py

    import os
    from conan import ConanFile
    from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
    from conan.tools.files import copy


    class secure_scannerRecipe(ConanFile):
        name = "secure_scanner"
        version = "1.0"
        package_type = "application"

        # Binary configuration
        settings = "os", "compiler", "build_type", "arch"

        # Sources are located in the same place as this recipe, copy them to the recipe
        exports_sources = "CMakeLists.txt", "src/*"

        def layout(self):
            cmake_layout(self)

        def generate(self):
            tc = CMakeToolchain(self)
            tc.generate()

        def build(self):
            cmake = CMake(self)
            cmake.configure()
            cmake.build()

        def package(self):
            extension = ".exe" if self.settings_build.os == "Windows" else ""
            copy(self, "*secure_scanner{}".format(extension),
                 self.build_folder, os.path.join(self.package_folder, "bin"), keep_path=False)

        def package_info(self):
            self.buildenv_info.define("MY_VAR", "23")


There are few relevant things in this recipe:

这个配方中几乎没有什么相关的东西:

1. It declares ``package_type = "application"``, this is optional but convenient, it will indicate conan that the current
   package doesn't contain headers or libraries to be linked. The consumers will know that this package is an application.

   它声明 ``package_type = "application"``，这是可选的，但是很方便，它将指示当前包不包含要链接的头或库。消费者将知道这个包是一个应用程序。

2. The ``package()`` method is packaging the executable into the ``bin/`` folder, that is declared by default as a bindir:
   ``self.cpp_info.bindirs = ["bin"]``.

   ``package()`` 方法将可执行文件打包到 ``bin/`` 文件夹中，该文件默认声明为 ``self.cpp_info.bindirs = ["bin"]``。

3. In the ``package_info()`` method, we are using ``self.buildenv_info`` to define a environment variable ``MY_VAR``
   that will also be available in the consumer.

   在 ``package_info()`` 方法中，我们使用 ``self.buildenv_info`` 来定义一个环境变量 ``MY_VAR``，它也可以在使用者中使用。


Let's create a binary package for the tool_require:

让我们为 tool_require 创建一个二进制包:

.. code-block:: bash

    $ conan create .
    ...
    secure_scanner/1.0: Calling package()
    secure_scanner/1.0: Copied 1 file: secure_scanner
    secure_scanner/1.0 package(): Packaged 1 file: secure_scanner
    ...
    Security Scanner: The path 'mypath' is secure!


Let's review the ``test_package/conanfile.py``:

.. code-block:: python

    from conan import ConanFile


    class secure_scannerTestConan(ConanFile):
        settings = "os", "compiler", "build_type", "arch"

        def build_requirements(self):
            self.tool_requires(self.tested_reference_str)

        def test(self):
            extension = ".exe" if self.settings_build.os == "Windows" else ""
            self.run("secure_scanner{} mypath".format(extension))


We are requiring the ``secure_scanner`` package as ``tool_require`` doing ``self.tool_requires(self.tested_reference_str)``.
In the ``test()`` method we are running the application, because it is available in the PATH. In the
next example we are going to see why the executables from a ``tool_require`` are available in the consumers.

我们需要 ``secure_scanner`` 包，因为 ``tool_require`` 需要完成 ``self.tool_requires(self.tested_reference_str)``。
在 ``test()`` 方法中，我们正在运行应用程序，因为它在 PATH 中可用。在下一个示例中，我们将看到为什么 ``tool_require`` 中的可执行文件在使用者中可用。

So, let's create a consumer recipe to test if we can run the ``secure_scanner`` application of the ``tool_require`` and
read the environment variable. Go to the `examples2/tutorial/creating_packages/other_packages/tool_requires/consumer`
folder:

因此，让我们创建一个用户配方来测试我们是否可以运行 ``tool_require`` 的 ``secure_scanner`` 应用程序并读取环境变量。
转到 `examples2/tutorial/creating_packages/other_packages/tool_requires/consumer` 文件夹:

.. code-block:: python
    :caption: conanfile.py

    from conan import ConanFile

    class MyConsumer(ConanFile):
        name = "my_consumer"
        version = "1.0"
        settings = "os", "arch", "compiler", "build_type"
        tool_requires = "secure_scanner/1.0"

        def build(self):
            extension = ".exe" if self.settings_build.os == "Windows" else ""
            self.run("secure_scanner{} {}".format(extension, self.build_folder))
            if self.settings_build.os != "Windows":
                self.run("echo MY_VAR=$MY_VAR")
            else:
                self.run("set MY_VAR")


In this simple recipe we are declaring a ``tool_require`` to ``secure_scanner/1.0`` and we are calling directly the packaged
application ``secure_scanner`` in the ``build()`` method, also printing the value of the ``MY_VAR`` env variable.

在这个简单的配方中，我们声明  ``tool_require`` 为 ``secure_scanner/1.0``，并在 ``build()`` 方法中直接调用打包的应用程序 
``secure_scanner``，同时打印 ``MY_VAR`` 环境变量的值。

If we build the consumer:


.. code-block:: bash


    $ conan build .

    -------- Installing (downloading, building) binaries... --------
    secure_scanner/1.0: Already installed!

    -------- Finalizing install (deploy, generators) --------
    ...
    conanfile.py (my_consumer/1.0): RUN: secure_scanner /Users/luism/workspace/examples2/tutorial/creating_packages/other_packages/tool_requires/consumer
    ...
    Security Scanner: The path '/Users/luism/workspace/examples2/tutorial/creating_packages/other_packages/tool_requires/consumer' is secure!
    ...
    MY_VAR=23


We can see that the executable returned 0 (because our folder is secure) and it printed ``Security Scanner: The path is secure!`` message.
It also printed the "23" value assigned to ``MY_VAR`` but, why are these automatically available?

我们可以看到可执行文件返回0(因为我们的文件夹是安全的) ，它打印安全扫描器:  ``Security Scanner: The path is secure!``  信息。它还打印了赋给 ``MY_VAR`` 的 "23" 值，
但是，为什么这些值是自动可用的？

- The generators ``VirtualBuildEnv`` and ``VirtualRunEnv`` are automatically used.

  自动使用生成器 ``VirtualBuildEnv`` 和 ``VirtualRunEnv``。

- The ``VirtualRunEnv`` is reading the ``tool-requires`` and is creating a launcher like ``conanbuildenv-release-x86_64.sh`` appending
  all ``cpp_info.bindirs`` to the ``PATH``, all the ``cpp_info.libdirs`` to the ``LD_LIBRARY_PATH`` environment variable and
  declaring each variable of ``self.buildenv_info``.

  ``VirtualRunEnv`` 正在读取 ``tool-requires`` 并创建一个类似 ``conanbuildenv-release-x86_64.sh`` 的加载器追加所有的 ``cpp_info.bindirs`` 到 ``PATH``、
  所有 ``cpp_info.libdirs`` 到 ``LD_LIBRARY_PATH`` 环境变量，并声明 ``self.buildenv_info`` 的每个变量。

- Every time conan executes the ``self.run``, by default, activates the ``conanbuild.sh`` file before calling any command.
  The ``conanbuild.sh`` is including the ``conanbuildenv-release-x86_64.sh``, so the application is in the PATH
  and the enviornment variable "MYVAR" has the value declared in the tool-require.

  默认情况下，每次 Conan 执行 ``self.run`` 时，都会在调用任何命令之前激活 ``conanbuild.sh`` 文件。 ``conanbuild.sh`` 包含了 ``conanbuildenv-release-x86_64.sh``，
  因此应用程序位于 PATH 中，而环境变量 "MYVAR" 具有在  tool-require 中声明的值。  


Removing settings in package_id()
---------------------------------

With the previous recipe, if we call :command:`conan create` with different setting like different compiler versions, we will get
different binary packages with a different ``package ID``. This might be convenient to, for example, keep better traceability of
our tools. In this case, the <MISSING PAGE> compatibility.py plugin can help to locate the best matching binary in case Conan doesn't find the
binary for our specific compiler version.

使用前面的配方，如果我们使用不同的设置(比如不同的编译器版本)调用 :command:`conan create` ，我们将获得具有不同  ``package ID`` 的不同二进制包。例如，
这可能有助于保持我们的工具具有更好的可跟踪性。在这种情况下，<MISSING PAGE> compatibility.py 插件可以帮助定位最匹配的二进制文件，
以防Conan找不到我们特定编译器版本的二进制文件。

But in some cases we might want to just generate a binary taking into account only the ``os``, ``arch`` or at most
adding the ``build_type`` to know if the application is built for Debug or Release. We can add a ``package_id()`` method
to remove them:

但是在某些情况下，我们可能只想生成一个二进制文件，只考虑  ``os``, ``arch`` 或者最多添加 ``build_type`` ，
以了解应用程序是为 Debug 还是 Release。我们可以添加 ``package_id()`` 方法来删除它们:


.. code-block:: python
    :caption: conanfile.py

    import os
    from conan import ConanFile
    from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
    from conan.tools.files import copy


    class secure_scannerRecipe(ConanFile):
        name = "secure_scanner"
        version = "1.0"
        settings = "os", "compiler", "build_type", "arch"
        ...

        def package_id(self):
            del self.info.settings.compiler
            del self.info.settings.build_type


So, if we call :command:`conan create` with different ``build_type`` we will get exactly the same ``package_id``.

因此，如果我们使用不同的 ``build_type`` 调用 :command:`conan create`，我们将得到完全相同的 ``package_id``。

.. code-block:: bash

    $ conan create .
    ...
    Package '82339cc4d6db7990c1830d274cd12e7c91ab18a1' created

    $ conan create . -s build_type=Debug
    ...
    Package '82339cc4d6db7990c1830d274cd12e7c91ab18a1' created

We got the same binary ``package_id``. The second ``conan create . -s build_type=Debug`` created and overwrote (created a newer package revision) of the previous Release binary, because they have the same ``package_id`` identifier. It is typical to create only the ``Release`` one, and if for any reason managing both Debug and Release binaries is intended, then the approach would be not removing the ``del self.info.settings.build_type``

我们得到了相同的二进制 ``package_id``。第二个  ``conan create . -s build_type=Debug``  创建并覆盖(创建了一个较新的软件包版本)上一版本的 Release 二进制文件，
因为它们具有相同的 ``package_id`` 标识符。通常只创建 ``Release`` 文件，如果出于某种原因打算同时管理 Debug 和 Release 二进制文件，那么这种方法将不会删除 ``del self.info.settings.build_type``

Read more
---------

- - :ref:`examples_graph_tool_requires_protobuf`
- Toolchains (compilers)
- Usage of `self.rundenv_info`
- ``settings_target``
