.. _creating_packages_create_your_first_conan_package:

Create your first Conan package
===============================

In previous sections, we *consumed* Conan packages (like the *Zlib* one), first using a
*conanfile.txt* and then with a *conanfile.py*. But a *conanfile.py* recipe file is not only
meant to consume other packages, it can be used to create your own packages as well. In
this section, we explain how to create a simple Conan package with a *conanfile.py* recipe
and how to use Conan commands to build those packages from sources.

在前面的章节中，我们*使用*了 Conan 包(如 *Zlib* 包) ，首先使用了 *conanfile.txt*，
然后使用了 *conanfile.py*。但是 *conanfile.py* 配方文件不仅可以使用其他包，
还可以用来创建自己的包。在本节中，我们将解释如何使用 *conanfile.py*
配方创建一个简单的 Conan 包，以及如何使用 Conan 命令从源代码构建这些包。

.. important::

    This is a **tutorial** section. You are encouraged to execute these commands. For this
    concrete example, you will need **CMake** installed  in your path. It is not strictly
    required by Conan to create packages, you can use other build systems (such as VS,
    Meson, Autotools, and even your own) to do that, without any dependency on CMake.

    这是一个 **教程** 部分。鼓励您执行这些命令。对于这个具体的示例，您需要在路径中安装 **CMake**。
    Conan 并不严格要求包的创建，您可以使用其他构建系统(如 VS、 Meson、 Autotools，甚至您自己的)
    来完成这项工作，而不需要依赖于 CMake。

Use the :command:`conan new` command to create a "Hello World" C++ library example project:

使用 :command:`conan new` 命令创建一个 “Hello World” C++库示例项目:

.. code-block:: bash

    $ conan new cmake_lib -d name=hello -d version=1.0


This will create a Conan package project with the following structure.

这将创建具有以下结构的 Conan 包项目。

.. code-block:: text

  .
  ├── CMakeLists.txt
  ├── conanfile.py
  ├── include
  │   └── hello.h
  ├── src
  │   └── hello.cpp
  └── test_package
      ├── CMakeLists.txt
      ├── conanfile.py
      └── src
          └── example.cpp

The generated files are:

生成的文件如下:

- **conanfile.py**: On the root folder, there is a *conanfile.py* which is the main recipe
  file, responsible for defining how the package is built and consumed.

  在根文件夹中，有一个 conanfile.py，它是主配方文件，负责定义如何构建和使用包。

- **CMakeLists.txt**: A simple generic *CMakeLists.txt*, with nothing specific about Conan
  in it.

  一个简单的通用 *CMakeLists.txt*，其中没有特定于 Conan 的内容。

- **src** folder: the *src* folder that contains the simple C++ "hello" library.
  
  包含简单 C++ "hello" 库的 *src* 文件夹。

- **test_package** folder: contains an *example* application that will require
  and link with the created package. It is not mandatory, but it is useful to check that
  our package is correctly created.

  包含一个 *example* 应用程序，该应用程序需要并链接到创建的包。它不是强制性的，
  但是检查包是否正确创建是很有用的。

Let's have a look at the package recipe *conanfile.py*:

让我们看一下包的配方 *conanfile.py*:

.. code-block:: python

  from conan import ConanFile
  from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout


  class helloRecipe(ConanFile):
      name = "hello"
      version = "1.0"

      # Optional metadata
      license = "<Put the package license here>"
      author = "<Put your name here> <And your email here>"
      url = "<Package recipe repository url here, for issues about the package>"
      description = "<Description of hello package here>"
      topics = ("<Put some tag here>", "<here>", "<and here>")

      # Binary configuration
      settings = "os", "compiler", "build_type", "arch"
      options = {"shared": [True, False], "fPIC": [True, False]}
      default_options = {"shared": False, "fPIC": True}

      # Sources are located in the same place as this recipe, copy them to the recipe
      exports_sources = "CMakeLists.txt", "src/*", "include/*"

      def config_options(self):
          if self.settings.os == "Windows":
              del self.options.fPIC

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
          cmake = CMake(self)
          cmake.install()

      def package_info(self):
          self.cpp_info.libs = ["hello"]


Let's explain the different sections of the recipe briefly:

让我们简要解释一下配方的不同部分: 

First, you can see the **name and version** of the Conan package defined:

首先，您可以看到定义的 Conan 包的 **name and version**:

* ``name``: a string, with a minimum of 2 and a maximum of 100 **lowercase** characters
  that defines the package name. It should start with alphanumeric or underscore and can
  contain alphanumeric, underscore, +, ., - characters.

  一个字符串，长度最小为2，最大为100个定义包名称的 **lowercase(小写)** 字符。
  它应该以字母数字或下划线开头，可以包含字母数字、下划线、 +、 .、 -字符。

* ``version``: It is a string, and can take any value, matching the same constraints as
  the ``name`` attribute. In case the version follows semantic versioning in the form
  ``X.Y.Z-pre1+build2``, that value might be used for requiring this package through
  version ranges instead of exact versions.

  它是一个字符串，可以接受任何值，与 ``name`` 属性匹配相同的约束。
  如果版本遵循 ``X.Y.Z-pre1+build2`` 形式的语义版本控制，
  那么这个值可以用于通过版本范围而不是精确版本来要求这个包。

Then you can see, some attributes defining **metadata**. These are optional but recommended
and define things like a short ``description`` for the package, the ``author`` of the packaged
library, the ``license``, the ``url`` for the package repository, and the ``topics`` that the package
is related to.

然后您可以看到，一些定义 **metadata(元数据)** 的属性。这些都是可选的，但是推荐使用，并定义一些内容，
比如包的简短 ``描述``、包库的 ``作者``、 ``许可证``、包存储库的 ``URL`` 以及与包相关的 ``主题``。

After that, there is a section related with the binary configuration. This section defines
the valid settings and options for the package. As we explained in the :ref:`consuming
packages section<settings_and_options_difference>`:

之后，还有一个与二进制配置相关的部分。本节定义包的有效设置和选项。
正如我们在 :ref:`consuming packages section<settings_and_options_difference>` 所解释的:

* ``settings`` are project-wide configuration that cannot be defaulted in recipes. Things
  like the operating system, compiler or build configuration that will be common to
  several Conan packages

  ``settings`` 是项目范围的配置, 在配方中不能进行默认设置。比如操作系统、编译器或构建配置，
  这些配置对于于几个 Conan 包来说是通用的。

* ``options`` are package-specific configuration and can be defaulted in recipes, in this case, we
  have the option of creating the package as a shared or static library, being static the default.

  ``options`` 是特定于包的配置，可以在配方中中使用默认值，在这种情况下，
  我们可以选择将包创建为共享库或静态库，默认为静态库。

After that, the ``exports_sources`` attribute is set to define which sources are part of
the Conan package. These are the sources for the library you want to package. In this case
the sources for our "hello" library.

然后，将 ``exports_sources`` 属性设置为定义哪些源是 Conan 包的一部分。
这些是要打包的库的源代码。在这种情况下，为我们的 "hello"  库的源代码。

Then, several methods are declared:

然后，声明几个方法:

* The ``config_options()`` method (together with ``configure()`` one) allows to fine-tune the binary configuration
  model, for example, in Windows, there is no ``fPIC`` option, so it can be removed.

  ``config_options()`` 方法(连同  ``configure()``)允许对二进制配置模型进行微调，例如，
  在 Windows 中没有 ``fPIC`` 选项，因此可以删除它。

* The ``layout()`` method declares the locations where we expect to find the source files
  and also those where we want to save the generated files during the build process.
  Things like the folder for the generated binaries or all the files that the Conan
  generators create in the ``generate()`` method. In this case, as our project uses CMake
  as the build system, we call to ``cmake_layout()``. Calling this function will set the
  expected locations for a CMake project. 

  ``layout()`` 方法声明了我们希望找到源文件的位置，以及我们希望在构建过程中保存生成的文件的位置。
  比如生成的二进制文件的文件夹，或者 Conan 生成器在 ``generate()`` 方法中创建的所有文件。
  在这种情况下，由于我们的项目使用 CMake 作为构建系统，因此我们调用 ``cmake_layout()``。
  调用此函数将设置 CMake 项目的预期位置。

* The ``generate()`` method prepares the build of the package from source. In this case, it could be simplified
  to an attribute ``generators = "CMakeToolchain"``, but it is left to show this important method. In this case,
  the execution of ``CMakeToolchain`` ``generate()`` method will create a *conan_toolchain.cmake* file that translates
  the Conan ``settings`` and ``options`` to CMake syntax.

  ``generate()`` 方法准备从源代码构建包。在这种情况下，它可以简化为一个属性 ``generators = "CMakeToolchain"``，
  但它只显示这个重要的方法。在这种情况下， ``CMakeToolchain`` ``generate()`` 方法的执行将创建一个 *conan_toolchain.cmake* 文件。
  将 Conan ``settings`` 和 ``options`` 转换为 CMake 语法的 CMake 文件。

* The ``build()`` method uses the ``CMake`` wrapper to call CMake commands, it is a thin layer that will manage
  to pass in this case the ``-DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake`` argument. It will configure the
  project and build it from source.

  ``build()`` 方法使用 ``CMake`` 包装器调用 CMake 命令，它是一个薄层，在本例中将设法传递 
  ``-DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake`` 参数。它将配置项目并从源代码构建它。

* The ``package()`` method copies artifacts (headers, libs) from the build folder to the
  final package folder. It can be done with bare "copy" commands, but in this case, it is
  leveraging the already existing CMake install functionality (if the CMakeLists.txt
  didn't implement it, it is easy to write an equivalent using the :ref:`copy()
  tool<conan_tools_files_copy>` in the ``package()`` method.

  ``package()`` 法将构建文件夹中的工件(头、库)复制到最终的包文件夹中。它可以通过“复制”命令完成，
  但是在这种情况下，它利用了已经存在的 CMake 安装功能(如果 CMakeLists.txt 没有实现它，
  那么很容易使用 ``package()`` 方法中的 :ref:`copy() tool<conan_tools_files_copy>` 编写等价的命令)。

* Finally, the ``package_info()`` method defines that consumers must link with a "hello" library
  when using this package. Other information as include or lib paths can be defined as well. This
  information is used for files created by generators (as ``CMakeDeps``) to be used by consumers. 
  This is generic information about the current package, and is available to the consumers
  irrespective of the build system they are using and irrespective of the build system we
  have used in the ``build()`` method

  最后， ``package_info()`` 方法定义消费者在使用此包时必须链接到“hello”库。
  还可以定义 include 或 lib 路径等其他信息。此信息用于由生成器(作为 ``CMakeDeps``)创建的文件，
  以供消费者使用。这是关于当前包的通用信息，不管使用者使用的是什么构建系统，
  也不管我们在 ``build()`` 方法中使用的是什么构建系统，使用者都可以获得这些信息


The **test_package** folder is not critical now for understanding how packages are created. The important
bits are:

**test_package** 文件夹现在对于理解包是如何创建的并不重要，重要的是:

* **test_package** folder is different from unit or integration tests. These tests are
  "package" tests, and validate that the package is properly created and that the package
  consumers will be able to link against it and reuse it.

  **test_package** 文件夹不同于单元测试或集成测试。这些测试是“包”测试，
  并验证包是否正确创建，以及包使用者是否能够对其进行链接并重用它。

* It is a small Conan project itself, it contains its ``conanfile.py``, and its source
  code including build scripts, that depends on the package being created, and builds and
  executes a small application that requires the library in the package.

  它本身是一个很小的 Conan 项目，它包含它的 ``conanfile.py`` 和它的源代码(包括构建脚本) ，
  这取决于正在创建的包，并且构建和执行一个需要包中的库的小应用程序。

* It doesn't belong in the package. It only exists in the source repository, not in the
  package.

  它不属于包，它只存在于源代码库中，而不在包中。


Let's build the package from sources with the current default configuration, and then let
the ``test_package`` folder test the package:

让我们使用当前默认配置的源构建包，然后让 ``test_package`` 文件夹测试包:

.. code-block:: bash

    $ conan create .
    -------- Exporting the recipe ----------
    hello/1.0: Exporting package recipe
    ...
    [ 50%] Building CXX object CMakeFiles/example.dir/src/example.cpp.o
    [100%] Linking CXX executable example
    [100%] Built target example

    -------- Testing the package: Running test() ----------
    hello/1.0 (test package): Running test()
    hello/1.0 (test package): RUN: ./example
    hello/1.0: Hello World Release!
      hello/1.0: __x86_64__ defined
      hello/1.0: __cplusplus199711
      hello/1.0: __GNUC__4
      hello/1.0: __GNUC_MINOR__2
      hello/1.0: __clang_major__13
      hello/1.0: __clang_minor__1
      hello/1.0: __apple_build_version__13160021
    ...

If "Hello world Release!" is displayed, it worked. This is what has happened:

如果“ Hello world Release!”显示出来，它就起作用了:

* The *conanfile.py* together with the contents of the *src* folder have been copied
  (**exported**, in Conan terms) to the local Conan cache.

  *conanfile.py* 以及 *src* 文件夹的内容已经被复制(**exported**, Conan 术语)到本地 Conan 缓存中。

* A new build from source for the ``hello/1.0`` package starts, calling the
  ``generate()``, ``build()`` and ``package()`` methods. This creates the binary package
  in the Conan cache.

  为 ``hello/1.0`` 包从源代码启动一个新的构建，调用 ``generate()``, ``build()`` 和 ``package()`` 方法。
  这将在 Conan 缓存中创建二进制包。

* Conan then moves to the *test_package* folder and executes a :command:`conan install` +
  :command:`conan build` + ``test()`` method, to check if the package was correctly
  created.

  然后，Conan 移动到 *test_package* 文件夹并执行一个 :command:`conan install` + :command:`conan build` + ``test()`` 
  方法，以检查包是否正确创建。

We can now validate that the recipe and the package binary are in the cache:

我们现在可以验证配方和包二进制文件是否在缓存中:

.. code-block:: bash

    $ conan list hello
    Local Cache:
      hello
        hello/1.0

The :command:`conan create` command receives the same parameters as :command:`conan install`, so
you can pass to it the same settings and options. If we execute the following lines, we will create new package
binaries for Debug configuration or to build the hello library as shared:

:command:`conan create` 命令接收与 :command:`conan install` 相同的参数，因此可以向其传递相同的设置和选项。如果执行以下代码行，
我们将为 Debug 配置创建新的包二进制文件，或者将 hello 库构建为 shared:

.. code-block:: bash

    $ conan create . -s build_type=Debug
    ...
    hello/1.0: Hello World Debug!

    $ conan create . -o hello/1.0:shared=True
    ...
    hello/1.0: Hello World Release!


These new package binaries will be also stored in the Conan cache, ready to be used by any project in this computer,
we can see them with:

这些新的包二进制文件也将存储在Conan缓存中，准备在这台计算机的任何项目使用，我们可以看到他们:


.. code-block:: bash

    # list the binary built for the hello/1.0 package
    # latest is a placeholder to show the package that is the latest created
    $ conan list hello/1.0#:*
    Local Cache:
    hello
      hello/1.0#fa5f6b17d0adc4de6030c9ab71cdbede (2022-12-22 17:32:19 UTC)
        PID: 6679492451b5d0750f14f9024fdbf84e19d2941b (2022-12-22 17:32:20 UTC)
          settings:
            arch=x86_64
            build_type=Release
            compiler=apple-clang
            compiler.cppstd=gnu11
            compiler.libcxx=libc++
            compiler.version=14
            os=Macos
          options:
            fPIC=True
            shared=True
        PID: b1d267f77ddd5d10d06d2ecf5a6bc433fbb7eeed (2022-12-22 17:31:59 UTC)
          settings:
            arch=x86_64
            build_type=Release
            compiler=apple-clang
            compiler.cppstd=gnu11
            compiler.libcxx=libc++
            compiler.version=14
            os=Macos
          options:
            fPIC=True
            shared=False
        PID: d15c4f81b5de757b13ca26b636246edff7bdbf24 (2022-12-22 17:32:14 UTC)
          settings:
            arch=x86_64
            build_type=Debug
            compiler=apple-clang
            compiler.cppstd=gnu11
            compiler.libcxx=libc++
            compiler.version=14
            os=Macos
          options:
            fPIC=True


Now that we have created a simple Conan package, we will explain each of the methods of
the Conanfile in more detail. You will learn how to modify those methods to achieve things
like retrieving the sources from an external repository, adding dependencies to our
package, customising our toolchain and much more.

现在我们已经创建了一个简单的 Conan 包，我们将更详细地解释 Conanfile 的每个方法。
您将学习如何修改这些方法，以实现从外部存储库检索源代码、向包中添加依赖项、定制工具链等等。

A note about the Conan cache
----------------------------

When you did the :command:`conan create` command, the build of your package did not take
place in your local folder but in other folder inside the *Conan cache*. This cache is
located in the user home folder under the ``.conan2`` folder. Conan will use the
``~/.conan2`` folder to store the built packages and also different configuration files.
You already used the :command:`conan list` command to list the recipes and binaries stored
in the local cache. 

执行 :command:`conan create` 命令时，包的构建不在本地文件夹中进行，而是在 *Conan cache* 中的其他文件夹中进行。
缓存在的用户主文件夹中的 ``.conan2`` 文件夹。Conan将使用 ``~/.conan2`` 文件夹存储构建的包以及不同的配置文件。
您已经使用了 :command:`conan list` 命令来列出存储在本地缓存中的配方和二进制文件。

Read more
---------

- :ref:`Conan list command reference<reference_commands_list>`.
- Create your first Conan package with Autotools.
- Create your first Conan package with Meson.
- Create your first Conan package with Visual Studio.
