.. _local_package_development_flow:

Package Development Flow
========================

This section introduces the **Conan local development flow**, which allows you to work on
packages in your local project directory without having to export the contents of the
package to the Conan cache first.

本节介绍 **Conan local development flow**，它允许您在本地项目目录中处理包，
而不必首先将包的内容导出到 Conan 缓存。

This local workflow encourages users to perform trial-and-error in a local sub-directory
relative to their recipe, much like how developers typically test building their projects
with other build tools. The strategy is to test the `conanfile.py` methods individually
during this phase.

这种本地工作流鼓励用户在相对于其配方的本地子目录中执行试错，
这与开发人员通常使用其他构建工具测试构建项目的方式非常相似。
策略是在这个阶段分别测试 `conanfile.py` 方法。

Let's use this flow for the ``hello`` package we created in :ref:`the previous
section<tutorial_creating_packages>`.

让我们将这个流用于我们在 :ref:`the previous section<tutorial_creating_packages>` 中创建的 ``hello`` 包。

Please clone the sources to recreate this project. You can find them in the `examples2.0
repository <https://github.com/conan-io/examples2>`_ on GitHub:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/developing_packages/local_package_development_flow

You can check the contents of the folder:

..  code-block:: text

    .
    ├── conanfile.py
    └── test_package
        ├── CMakeLists.txt
        ├── conanfile.py
        └── src
            └── example.cpp

conan source
------------

You will generally want to start with the :command:`conan source` command. The strategy
here is that you’re testing your source method in isolation and downloading the files to a
temporary sub-folder relative to the `conanfile.py`. This relative folder is defined by
the `self.folders.source` property in the `layout()` method. In this case, as we are using
the pre-defined `cmake_layout` we set the value with the `src_folder` argument.

您通常需要从 :command:`conan source` 命令开始。这里的策略是独立测试源方法，并将文件下载到相对于 
`conanfile.py` 的临时子文件夹中。这个相对文件夹是由 `layout()` 方法中的 
`self.folders.source` 属性定义的。在本例中，由于我们使用的是预定义的 `cmake_layout`，
因此我们使用 `src_folder` 参数设置值。

.. note::

    In this example we are packaging a third-party library from a remote repository. In the
    case you have your sources beside your recipe in the same repository, running
    :command:`conan source` will not be necessary for most of the cases.

    在这个示例中，我们从一个远程存储库打包一个第三方库。如果在同一个存储库中，
    配方旁边有源代码，那么在大多数情况下不需要运行 :command:`conan source`。

Let's have a look at the recipe's `source()` and `layout()` method:

让我们看一下配方的 `source()` 和 `layout()` 方法:

..  code-block:: python

    ...

    def source(self):
        # Please be aware that using the head of the branch instead of an immutable tag
        # or commit is not a good practice in general.
        get(self, "https://github.com/conan-io/libhello/archive/refs/heads/main.zip", 
            strip_root=True)

    def layout(self):
        cmake_layout(self, src_folder="src")

    ...


Now run the :command:`conan source` command and check the results:

现在运行 :command:`conan source` 命令并检查结果:

.. code-block:: bash

    $ conan source .
    conanfile.py (hello/1.0): Calling source() in /Users/.../local_package_development_flow/src
    Downloading main.zip
    conanfile.py (hello/1.0): Unzipping 3.7KB
    Unzipping 100%        

You can see that a new `src` folder has appeared containing all the `hello` library sources.

您可以看到出现了一个新的 `src` 文件夹，其中包含所有 `hello` 库源。

..  code-block:: text
    :emphasize-lines: 3-10

    .
    ├── conanfile.py
    ├── src
    │   ├── CMakeLists.txt
    │   ├── LICENSE
    │   ├── README.md
    │   ├── include
    │   │   └── hello.h
    │   └── src
    │       └── hello.cpp
    └── test_package
        ├── CMakeLists.txt
        ├── conanfile.py
        └── src
            └── example.cpp

Now it's easy to check the sources and validate them. Once you've got your source method
right and it contains the files you expect, you can move on to testing the various
attributes and methods related to downloading dependencies.

现在可以很容易地检查源代码并验证它们。一旦您的源方法正确，并且它包含您所期望的文件，
您就可以继续测试与下载依赖项相关的各种属性和方法。

conan install
-------------

After running the :command:`conan source` command, you can run the :command:`conan
install` command. This command will install all the recipe requirements if needed and
prepare all the files necessary for building by running the ``generate()`` method.

在运行了 :command:`conan source` 命令之后，您可以运行 :command:`conan install` 命令。
如果需要，这个命令将安装所有的配方需求，并通过运行 ``generate()`` 方法准备构建所需的所有文件。

We can check all the parts from our recipe that are involved in this step:

我们可以检查配方中涉及到这个步骤的所有部分:

.. code-block:: python

    ...

    class helloRecipe(ConanFile):

        ...
    
        generators = "CMakeDeps"

        ...

        def layout(self):
            cmake_layout(self, src_folder="src")

        def generate(self):
            tc = CMakeToolchain(self)
            tc.generate()

        ...

Now run the :command:`conan install` command and check the results:

现在运行 :command:`conan install` 命令并检查结果:

.. code-block:: bash

    $ conan install .
    ...
    -------- Finalizing install (deploy, generators) --------
    conanfile.py (hello/1.0): Writing generators to ...
    conanfile.py (hello/1.0): Generator 'CMakeDeps' calling 'generate()'
    conanfile.py (hello/1.0): Calling generate()
    ...
    conanfile.py (hello/1.0): Aggregating env generators 

You can see that a new `build` folder appeared with all the files that Conan needs for
building the library like a toolchain for `CMake` and several environment configuration
files.

您可以看到，出现了一个新的 `build` 文件夹，其中包含 Conan 构建库所需的所有文件，如 `CMake` 的工具链和几个环境配置文件。

..  code-block:: text
    :emphasize-lines: 3-10

    .
    ├── build
    │   └── Release
    │       └── generators
    │           ├── CMakePresets.json
    │           ├── cmakedeps_macros.cmake
    │           ├── conan_toolchain.cmake
    │           ├── conanbuild.sh
    │           ├── conanbuildenv-release-x86_64.sh
    │           ├── conanrun.sh
    │           ├── conanrunenv-release-x86_64.sh
    │           ├── deactivate_conanbuild.sh
    │           └── deactivate_conanrun.sh
    ├── conanfile.py
    ├── src
    │   ├── CMakeLists.txt
    │   ├── CMakeUserPresets.json
    │   ├── LICENSE
    │   ├── README.md
    │   ├── include
    │   │   └── hello.h
    │   └── src
    │       └── hello.cpp
    └── test_package
        ├── CMakeLists.txt
        ├── conanfile.py
        └── src
            └── example.cpp

Now that all the files necessary for building are generated, you can move on to testing
the `build()` method. 

现在生成了构建所需的所有文件，接下来可以测试 `build()` 方法。

conan build
-----------

Running the After :command:`conan build` command will invoke the `build()` method:

运行 :command:`conan build` 命令后将调用  `build()`  方法:

.. code-block:: python

    ...

    class helloRecipe(ConanFile):

        ...
    
        def build(self):
            cmake = CMake(self)
            cmake.configure()
            cmake.build()

        ...

Let's run :command:`conan build`:

.. code-block:: bash

    $ conan build .
    ...
    -- Conan toolchain: C++ Standard 11 with extensions ON
    -- Conan toolchain: Setting BUILD_SHARED_LIBS = OFF
    -- Configuring done
    -- Generating done
    -- Build files have been ...
    conanfile.py (hello/1.0): CMake command: cmake --build ...
    conanfile.py (hello/1.0): RUN: cmake --build ...
    [100%] Built target hello

For most of the recipes, the `build()` method should be very simple, and you can also
invoke the build system directly, without invoking Conan, as you have all the necessary
files available for building. If you check the contents of the `src` folder, you'll find a
`CMakeUserPresets.json` file that you can use to configure and build the `conan-release`
preset. Let's try it:

对于大多数配方， `build()` 方法应该非常简单，而且您还可以直接调用构建系统，而不用调用 Conan，
因为您拥有构建所需的所有文件。如果您检查 `src` 文件夹的内容，您将找到一个 `CMakeUserPresets.json` 文件，
您可以使用它来配置和构建 `conan-release` 预设置。让我们试试:

.. code-block:: bash

    $ cd src
    $ cmake --preset conan-conan-release
    ...
    -- Configuring done
    -- Generating done

    $ cmake --build --preset conan-conan-release
    ...
    [100%] Built target hello

You can check that the results of invoking CMake directly are equivalent to the ones we
got using the :command:`conan build` command.

您可以检查直接调用 CMake 得到的结果是否与使用 :command:`conan build` 命令得到的结果相等。

.. include:: ../cmake_presets_note.inc

conan export-pkg
----------------

Now that we built the package binaries locally we can also package those artifacts in the
Conan local cache using the :command:`conan export-pkg` command. Please note that this
command will create the package in the Conan cache and test it running the `test_package`
after that.

现在我们已经在本地构建了包二进制文件，我们还可以使用 :command:`conan export-pkg` 命令将这些构件打包到 
Conan 本地缓存中。请注意，这个命令将在 Conan 缓存中创建包，然后运行 `test_package` 对其进行测试。

..  code-block:: bash

    $ conan export-pkg .
    conanfile.py (hello/1.0) package(): Packaged 1 '.h' file: hello.h
    conanfile.py (hello/1.0) package(): Packaged 1 '.a' file: libhello.a
    conanfile.py (hello/1.0): Package 'b1d267f77ddd5d10d06d2ecf5a6bc433fbb7eeed' created
    conanfile.py (hello/1.0): Created package revision f09ef573c22f3919ba26ee91ae444eaa
    ...
    conanfile.py (hello/1.0): Package folder /Users/...
    conanfile.py (hello/1.0): Exported package binary
    ...
    [ 50%] Building CXX object CMakeFiles/example.dir/src/example.cpp.o
    [100%] Linking CXX executable example
    [100%] Built target example

    -------- Testing the package: Running test() --------
    hello/1.0 (test package): Running test()
    hello/1.0 (test package): RUN: ./example
    hello/1.0: Hello World Release!
    hello/1.0: __x86_64__ defined
    hello/1.0: __cplusplus201103
    hello/1.0: __GNUC__4
    hello/1.0: __GNUC_MINOR__2
    hello/1.0: __clang_major__14
    hello/1.0: __apple_build_version__14000029

Now you can list the packages in the local cache and check that the ``hello/1.0`` package
was created.

现在您可以列出本地缓存中的包，并检查 ``hello/1.0`` 包是否已创建。

..  code-block:: bash

    $ conan list hello/1.0
    Local Cache
    hello
        hello/1.0

.. seealso::
    
    - Reference for conan :ref:`source<reference_commands_source>`,
      :ref:`install<reference_commands_install>`, :ref:`build<reference_commands_build>`,
      :ref:`export-pkg<reference_commands_export-pkg>` and
      :ref:`test<reference_commands_test>` commands.

      参考conan :ref:`source<reference_commands_source>`，
      :ref:`install<reference_commands_install>`, :ref:`build<reference_commands_build>`,
      :ref:`export-pkg<reference_commands_export-pkg>` 和
      :ref:`test<reference_commands_test>` 命令。

    - Packaging prebuilt binaries :ref:`example<creating_packages_other_prebuilt>`

      打包预先构建的二进制文件 :ref:`example<creating_packages_other_prebuilt>`

    - When you are locally developing packages, at some poing you might need to step-into dependencies code while debugging. Please read this :ref:`example how to debug and step-into dependencies <examples_dev_flow_debug_step_into>` for more information about this use case.

      当您在本地开发包时，在某种程度上，您可能需要在调试时分步进入依赖项代码。请阅读此 :ref:`example how to debug and step-into dependencies <examples_dev_flow_debug_step_into>` ，以获得有关此用例的更多信息。