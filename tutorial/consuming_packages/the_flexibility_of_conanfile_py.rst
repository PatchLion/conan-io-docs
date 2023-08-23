.. _consuming_packages_flexibility_of_conanfile_py:

Understanding the flexibility of using conanfile.py vs conanfile.txt
====================================================================

In the previous examples, we declared our dependencies (*Zlib* and *CMake*) in a
*conanfile.txt* file. Let's have a look at that file:

在前面的示例中，我们在 *conanfile.txt* 文件中声明了我们的依赖项(*Zlib* 和 *CMake*)。让我们看看那个文件:

.. code-block:: ini
    :caption: **conanfile.txt**

    [requires]
    zlib/1.2.11

    [tool_requires]
    cmake/3.22.6

    [generators]
    CMakeDeps
    CMakeToolchain

Using a *conanfile.txt* to build your projects using Conan it's enough for simple cases,
but if you need more flexibility you should use a *conanfile.py* file where you can use
Python code to make things such as adding requirements dynamically, changing options
depending on other options or setting options for your requirements. Let's see an example
on how to migrate to a *conanfile.py* and use some of those features.

使用 *conanfile.txt* 来构建使用 Conan 的项目，对于简单的情况来说已经足够了，
但是如果你需要更多的灵活性，你应该使用 *conanfile.py* 文件，
在这里你可以使用 Python 代码来动态添加需求，根据其他选项更改选项或者为你的需求设置选项。
让我们看一个关于如何迁移到 *conanfile.py* 并使用其中一些特性的示例。

Please, first clone the sources to recreate this project. You can find them in the
`examples2.0 repository <https://github.com/conan-io/examples2>`_ in GitHub:

请首先克隆这些源代码来重新创建这个项目，您可以在 Git Hub 的  `examples2.0 repository <https://github.com/conan-io/examples2>`_ 中找到它们:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/consuming_packages/conanfile_py

Check the contents of the folder and note that the contents are the same that in the
previous examples but with a *conanfile.py* instead of a *conanfile.txt*.

.. code-block:: bash

    .
    ├── CMakeLists.txt
    ├── conanfile.py
    └── src
        └── main.c

Remember that in the previous examples the *conanfile.txt* had this information:

.. code-block:: ini
    :caption: **conanfile.txt**

    [requires]
    zlib/1.2.11

    [tool_requires]
    cmake/3.22.6

    [generators]
    CMakeDeps
    CMakeToolchain

We will translate that same information to a *conanfile.py*. This file is what is
typically called a **"Conan recipe"**. It can be used for consuming packages, like in this
case, and also to create packages. For our current case, it will define our requirements
(both libraries and build tools) and logic to modify options and set how we want to
consume those packages. In the case of using this file to create packages, it can define
(among other things) how to download the package’s source code, how to build the binaries
from those sources, how to package the binaries, and information for future consumers on
how to consume the package. We will explain how to use Conan recipes to create
packages in the :ref:`Creating Packages<tutorial_creating_packages>` section later.

我们将把相同的信息转换为  *conanfile.py*。这个文件通常被称为 **"Conan recipe"**。它可以用于使用包(如本例所示) ，
也可以用于创建包。对于我们当前的情况，它将定义我们的需求(包括库和构建工具)和修改选项的逻辑，
并设置我们希望如何使用这些包。在使用这个文件创建包的情况下，它可以定义(除了其他事情之外)如何下载包的源代码，
如何从这些源构建二进制文件，如何打包二进制文件，以及如何使用包的信息。
稍后，我们将在 :ref:`Creating Packages<tutorial_creating_packages>` 一节中解释如何使用 Conan 配方来创建包。

The equivalent of the *conanfile.txt* in form of Conan recipe could look like this:

.. code-block:: python
    :caption: **conanfile.py**

    from conan import ConanFile


    class CompressorRecipe(ConanFile):
        settings = "os", "compiler", "build_type", "arch"
        generators = "CMakeToolchain", "CMakeDeps"

        def requirements(self):
            self.requires("zlib/1.2.11")
        
        def build_requirements(self):
            self.tool_requires("cmake/3.22.6")


To create the Conan recipe we declared a new class that inherits from the ``ConanFile``
class. This class has different class attributes and methods:

为了创建 Conan 配方，我们声明了一个从 ``ConanFile`` 类继承的新类。这个类有不同的类属性和方法:

* **settings** this class attribute defines the project-wide variables, like the compiler,
  its version, or the OS itself that may change when we build our project. This is related
  to how Conan manages binary compatibility as these values will affect the value of the
  **package ID** for Conan packages. We will explain how Conan uses this value to manage
  binary compatibility later.

  **settings** 这个类属性定义项目范围的变量，如编译器、其版本或生成项目时可能更改的操作系统本身。
  这与 Conan 如何管理二进制兼容性有关，因为这些值将影响 Conan 包的 **package ID**  的值。
  稍后我们将解释 Conan 如何使用此值来管理二进制兼容性。

* **generators** this class attribute specifies which Conan generators will be run when we
  call the :command:`conan install` command. In this case, we added **CMakeToolchain** and
  **CMakeDeps** as in the *conanfile.txt*.

  **generators** 这个类属性指定当我们调用 :command:`conan install` 命令时将运行哪个 Conan 生成器。
  在本例中，我们像在 *conanfile.txt* 中那样添加了 **CMakeToolchain**  和 **CMakeDeps**。

* **requirements()** in this method we can use the ``self.requires()`` and
  ``self.tool_requires()`` methods to declare all our dependencies (libraries and build
  tools).

  **requirements()** 在这个方法中，我们可以使用 ``self.requires()`` 和 ``self.tool_requires()`` 
  方法来声明所有的依赖项(库和构建工具)。

You can check that running the same commands as in the previous examples will lead to the
same results as before.

您可以检查运行与前面示例中相同的命令将得到与前面相同的结果。

.. code-block:: bash
    :caption: Windows

    $ conan install . --output-folder=build --build=missing
    $ cd build
    $ conanbuild.bat
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
    $ deactivate_conanbuild.bat

.. code-block:: bash
    :caption: Linux, macOS
    
    $ conan install . --output-folder build --build=missing
    $ cd build
    $ source conanbuild.sh
    Capturing current environment in deactivate_conanbuildenv-release-x86_64.sh
    Configuring environment variables    
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
    $ source deactivate_conanbuild.sh

So far we have achieved the same functionality we had using a *conanfile.txt*, let's see
how we can take advantage of the capabilities of the *conanfile.py* to define the project
structure we want to follow and also to add some logic using Conan settings and options.

到目前为止，我们已经实现了使用 *conanfile.txt* 的相同功能，让我们看看如何利用 *conanfile.py* 
的功能来定义我们想要遵循的项目结构，并使用 Conan 设置和选项添加一些逻辑。

.. _consuming_packages_flexibility_of_conanfile_py_use_layout:

Use the layout() method
-----------------------

In the previous examples, every time we executed a `conan install` command, we had to use
the `--output-folder` argument to define where we wanted to create the files that Conan
generates. There's a neater way to decide where we want Conan to generate the files for
the build system that will allow us to decide, for example, if we want different output
folders depending on the type of CMake generator we are using. You can define this
directly in the `conanfile.py` inside the `layout()` method and make it work for every
platform without adding more changes.

在前面的示例中，每次执行 `conan install` 命令时，都必须使用 `--output-folder` 参数来定义创建 Conan 
生成的文件的位置。有一种更简单的方法可以决定我们希望 Conan 在哪里为构建系统生成文件，
这将允许我们决定，例如，我们是否需要不同的输出文件夹，这取决于我们使用的 CMake 生成器的类型。
您可以在 `layout()` 方法中的 `conanfile.py` 中直接定义它，并使其适用于每个平台，而无需添加更多更改。

.. code-block:: python
    :caption: **conanfile.py**

    import os

    from conan import ConanFile


    class CompressorRecipe(ConanFile):
        settings = "os", "compiler", "build_type", "arch"
        generators = "CMakeToolchain", "CMakeDeps"

        def requirements(self):
            self.requires("zlib/1.2.11")

        def build_requirements(self):
            self.tool_requires("cmake/3.22.6")

        def layout(self):
            # We make the assumption that if the compiler is msvc the
            # CMake generator is multi-config
            multi = True if self.settings.get_safe("compiler") == "msvc" else False
            if multi:
                self.folders.generators = os.path.join("build", "generators")
            else:
                self.folders.generators = os.path.join("build", str(self.settings.build_type), "generators")


As you can see, we defined the **self.folders.generators** attribute in the `layout()`
method. This is the folder where all the auxiliary files generated by Conan (CMake
toolchain and cmake dependencies files) will be placed.

正如您所看到的，我们在 `layout()` 方法中定义了 **self.folders.generators** 属性。
这个文件夹将放置 Conan (CMake 工具链和 CMake 依赖项文件)生成的所有辅助文件。

Note that the definitions of the folders is different if it is a multi-config generator
(like Visual Studio), or a single-config generator (like Unix Makefiles). In the
first case, the folder is the same irrespective of the build type, and the build system
will manage the different build types inside that folder. But single-config generators
like Unix Makefiles, must use a different folder for each different configuration (as a
different build_type Release/Debug). In this case we added a simple logic to consider
multi-config if the compiler name is `msvc`.

注意，如果是多配置生成器(如 Visual Studio)或单配置生成器(如 Unix Makefile) ，
则文件夹的定义是不同的。在第一种情况下，不管生成类型如何，文件夹都是相同的，
生成系统将管理该文件夹中的不同生成类型。但是像 Unix Makefile 
这样的单配置生成器必须为每个不同的配置使用不同的文件夹(作为不同的 build_type Release/Debug)。
在本例中，如果编译器名称为 `msvc`，我们添加了一个简单的逻辑来考虑 multi-config。

Check that running the same commands as in the previous examples without the
`--output-folder` argument will lead to the same results as before:

检查在没有 `--output-folder` 参数的情况下运行与前面示例相同的命令将得到与前面相同的结果:

.. code-block:: bash
    :caption: Windows

    $ conan install . --build=missing
    $ cd build
    $ generators\conanbuild.bat
    # assuming Visual Studio 15 2017 is your VS version and that it matches your default profile
    $ cmake .. -G "Visual Studio 15 2017" -DCMAKE_TOOLCHAIN_FILE=generators\conan_toolchain.cmake
    $ cmake --build . --config Release
    ...
    Building with CMake version: 3.22.6
    ...
    [100%] Built target compressor

    $ Release\compressor.exe
    Uncompressed size is: 233
    Compressed size is: 147
    ZLIB VERSION: 1.2.11
    $ generators\deactivate_conanbuild.bat

.. code-block:: bash
    :caption: Linux, macOS
    
    $ conan install . --build=missing
    $ cd build
    $ source ./Release/generators/conanbuild.sh
    Capturing current environment in deactivate_conanbuildenv-release-x86_64.sh
    Configuring environment variables    
    $ cmake .. -DCMAKE_TOOLCHAIN_FILE=Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
    $ cmake --build .
    ...
    Building with CMake version: 3.22.6
    ...
    [100%] Built target compressor

    $ ./compressor
    Uncompressed size is: 233
    Compressed size is: 147
    ZLIB VERSION: 1.2.11
    $ source ./Release/generators/deactivate_conanbuild.sh

There's no need to always write this logic in the `conanfile.py`. There are some
pre-defined layouts you can import and directly use in your recipe. For example, for the
CMake case, there's a :ref:`cmake_layout()<cmake_layout>` already defined in Conan:


没有必要总是在 `conanfile.py` 中编写这种逻辑。有一些预定义的布局，
您可以导入和直接使用在您的配方。例如，对于 CMake 案例，在 Conan 中已经定义了一个 
:ref:`cmake_layout()<cmake_layout>`:

.. code-block:: python
    :caption: **conanfile.py**

    from conan import ConanFile
    from conan.tools.cmake import cmake_layout


    class CompressorRecipe(ConanFile):
        settings = "os", "compiler", "build_type", "arch"
        generators = "CMakeToolchain", "CMakeDeps"

        def requirements(self):
            self.requires("zlib/1.2.11")

        def build_requirements(self):
            self.tool_requires("cmake/3.22.6")

        def layout(self):
            cmake_layout(self)


Use the validate() method to raise an error for non-supported configurations
----------------------------------------------------------------------------

The :ref:`validate() method<reference_conanfile_methods_validate>` is evaluated when Conan loads the *conanfile.py* and you can use
it to perform checks of the input settings. If, for example, your project does not support
*armv8* architecture on macOS you can raise the `ConanInvalidConfiguration` exception to
make Conan return with a special error code. This will indicate that the configuration
used for settings or options is not supported.

在 Conan 加载 *conanfile.py* 时，会计算 :ref:`validate() method<reference_conanfile_methods_validate>` 方法，
您可以使用它来执行输入设置的检查。例如，如果您的项目不支持 mac 操作系统上的 *armv8* 架构，您可以引发 
`ConanInvalidConfiguration` 异常，使 Conan 返回一个特殊的错误代码。这将表明不支持用于设置或选项的配置。

.. code-block:: python
    :caption: **conanfile.py**

    ...
    from conan.errors import ConanInvalidConfiguration

    class CompressorRecipe(ConanFile):
        ...

        def validate(self):
            if self.settings.os == "Macos" and self.settings.arch == "armv8":
                raise ConanInvalidConfiguration("ARM v8 not supported in Macos")


Conditional requirements using a conanfile.py
---------------------------------------------

You could add some logic to the :ref:`requirements() method<reference_conanfile_methods_requirements>` to add or remove requirements
conditionally. Imagine, for example, that you want to add an additional dependency in
Windows or that you want to use the system's CMake installation instead of using the Conan
`tool_requires`:

您可以向 :ref:`requirements() method<reference_conanfile_methods_requirements>` 方法添加一些逻辑，以便有条件地添加或删除需求。
例如，假设您想要在 Windows 中添加一个附加的依赖项，
或者您想要使用系统的 CMake 安装包而不是使用 Conan `tool_requires`:

.. code-block:: python
    :caption: **conanfile.py**

    from conan import ConanFile


    class CompressorRecipe(ConanFile):
        # Binary configuration
        settings = "os", "compiler", "build_type", "arch"
        generators = "CMakeToolchain", "CMakeDeps"

        def requirements(self):
            self.requires("zlib/1.2.11")
            
            # Add base64 dependency for Windows
            if self.settings.os == "Windows":
                self.requires("base64/0.4.0")

        def build_requirements(self):
            # Use the system's CMake for Windows
            if self.settings.os != "Windows":
                self.tool_requires("cmake/3.22.6")


Read more
---------

.. container:: examples

    - :ref:`Using "cmake_layout" + "CMakeToolchain" + "CMakePresets feature" to build your project<examples-tools-cmake-toolchain-build-project-presets>`.
    - :ref:`Understanding the Conan Package layout<tutorial_package_layout>`.
    - Importing resource files in the generate() method
    - Conditional generators in configure()
