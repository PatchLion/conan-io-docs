
.. _creating_packages_preparing_the_build:

Preparing the build
===================

In the :ref:`previous tutorial section<creating_packages_add_dependencies_to_packages>`,
we added the `fmt <https://conan.io/center/fmt>`__ requirement to our Conan package to
provide colour output to our "Hello World" C++ library. In this section, we focus on the
``generate()`` method of the recipe. The aim of this method generating all the
information that could be needed while running the build step. That means things like:

在 :ref:`previous tutorial section<creating_packages_add_dependencies_to_packages>` 中，
我们将 `fmt <https://conan.io/center/fmt>`__ 要求添加到 Conan 包中，以便为“ Hello World” 
C++ 库提供颜色输出。在本节中，我们将重点讨论这个配方的 ``generate()``  方法。
此方法的目的是生成运行构建步骤时可能需要的所有信息。这意味着:

* Write files to be used in the build step, like
  :ref:`scripts<conan_tools_env_environment_model>` that inject environment variables,
  files to pass to the build system, etc.

  编写要在构建步骤中使用的文件，比如注入环境变量的 :ref:`scripts<conan_tools_env_environment_model>`，
  要传递到构建系统的文件等等。

* Configuring the toolchain to provide extra information based on the settings and options
  or removing information from the toolchain that Conan generates by default and may not
  apply for certain cases.

  将工具链配置为基于设置和选项提供额外信息，或者从 Conan 默认生成但可能不适用于某些情况的工具链中删除信息。


We explain to use this method for a simple example based on the previous tutorial section.
We add a `with_fmt` option to the recipe, depending on the value we require the
`fmt` library or not. We use the `generate()` method to modify the toolchain so that
it passes a variable to CMake so that we can conditionally add that library and use `fmt`
or not in the source code.

我们将在前面的教程部分的基础上为一个简单的示例解释如何使用此方法。我们将 `with_fmt` 选项添加到配方中，
这取决于我们是否需要 `fmt` 库的值。我们使用 `generate()` 方法修改工具链，以便它将一个变量传递给 CMake，
这样我们就可以有条件地添加该库并在源代码中使用或不使用 `fmt`。

Please, first clone the sources to recreate this project. You can find them in the
`examples2.0 repository <https://github.com/conan-io/examples2>`_ on GitHub:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/creating_packages/preparing_the_build

You will notice some changes in the `conanfile.py` file from the previous recipe.
Let's check the relevant parts:

.. code-block:: python
    :emphasize-lines: 12,16,20,27,30,35

    ...
    from conan.tools.build import check_max_cppstd, check_min_cppstd
    ...

    class helloRecipe(ConanFile):
        name = "hello"
        version = "1.0"

        ...
        options = {"shared": [True, False], 
                   "fPIC": [True, False],
                   "with_fmt": [True, False]}

        default_options = {"shared": False, 
                           "fPIC": True,
                           "with_fmt": True}
        ...

        def validate(self):
            if self.options.with_fmt:
                check_min_cppstd(self, "11")
                check_max_cppstd(self, "14")

        def source(self):
            git = Git(self)
            git.clone(url="https://github.com/conan-io/libhello.git", target=".")
            # Please, be aware that using the head of the branch instead of an immutable tag
            # or commit is not a good practice in general
            git.checkout("optional_fmt")

        def requirements(self):
            if self.options.with_fmt:
                self.requires("fmt/8.1.1")

        def generate(self):
            tc = CMakeToolchain(self)
            if self.options.with_fmt:
                tc.variables["WITH_FMT"] = True
            tc.generate()

        ...


As you can see:

* We declare a new ``with_fmt`` option with the default value set to ``True``

  我们声明一个新的 ``with_fmt`` 选项，默认值设置为 ``True``

* Based on the value of the ``with_fmt`` option:

  根据 ``with_fmt`` 选项的值:

    - We install or not the ``fmt/8.1.1`` Conan package.
      
      我们安装或不安装 ``fmt/8.1.1`` 包。

    - We require or not a minimum and a maximum C++ standard as the *fmt* library requires at least C++11 and it will not compile if we try to use a standard above C++14 (just an example, *fmt* can build with more modern standards)

      我们需要或不需要最低和最高的 C++ 标准，因为 *fmt* 库至少需要 C++11，如果我们试图使用 C++ 14之上的标准，它就不会编译(举个例子，*fmt* 可以使用更现代的标准来构建)

    - We inject the ``WITH_FMT`` variable with the value ``True`` to the :ref:`CMakeToolchain<conan_tools_cmaketoolchain>` so that we
      can use it in the *CMakeLists.txt* of the **hello** library to add the CMake **fmt::fmt** target
      conditionally.

      我们将带有值 ``True`` 的 ``WITH_FMT`` 变量注入到 :ref:`CMakeToolchain<conan_tools_cmaketoolchain>` 中，这样我们就可以在 **hello** 库的 *CMakeLists.txt* 中使用它来有条件地添加 CMake **fmt::fmt** 目标。

* We are cloning another branch of the library. The *optional_fmt* branch contains
  some changes in the code. Let's see what changed on the CMake side:

  我们正在克隆图书馆的另一个分支。 *optional_fmt* 分支包含代码中的一些更改。让我们看看在 CMake 方面发生了什么变化:

.. code-block:: cmake
    :caption: **CMakeLists.txt**
    :emphasize-lines: 8-12

    cmake_minimum_required(VERSION 3.15)
    project(hello CXX)

    add_library(hello src/hello.cpp)
    target_include_directories(hello PUBLIC include)
    set_target_properties(hello PROPERTIES PUBLIC_HEADER "include/hello.h")

    if (WITH_FMT)
        find_package(fmt)
        target_link_libraries(hello fmt::fmt)
        target_compile_definitions(hello PRIVATE USING_FMT=1)
    endif()

    install(TARGETS hello)

As you can see, we use the ``WITH_FMT`` we injected in the
:ref:`CMakeToolchain<conan_tools_cmaketoolchain>`. Depending on the value we will try to find
the fmt library and link our hello library with it. Also, check that we add the
``USING_FMT=1`` compile definition that we use in the source code depending on whether we
choose to add support for ``fmt`` or not.

如您所见，我们使用注入到 :ref:`CMakeToolchain<conan_tools_cmaketoolchain>` 中的 ``WITH_FMT``。
根据这个值，我们将尝试找到 ``fmt`` 库并将 hello 库与它链接起来。另外，根据我们是否选择添加对 fmt 的支持，
检查我们是否添加了 ``USING_FMT=1`` 编译定义，我们在源代码中使用这个定义。

.. code-block:: cpp
    :caption: **hello.cpp**
    :emphasize-lines: 4,9

    #include <iostream>
    #include "hello.h"

    #if USING_FMT == 1
    #include <fmt/color.h>
    #endif

    void hello(){
        #if USING_FMT == 1
            #ifdef NDEBUG
            fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "hello/1.0: Hello World Release! (with color!)\n");
            #else
            fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "hello/1.0: Hello World Debug! (with color!)\n");
            #endif
        #else
            #ifdef NDEBUG
            std::cout << "hello/1.0: Hello World Release! (without color)" << std::endl;
            #else
            std::cout << "hello/1.0: Hello World Debug! (without color)" << std::endl;
            #endif
        #endif
    }

Let's build the package from sources first using ``with_fmt=True`` and then
``with_fmt=False``. When *test_package* runs it will show different messages depending
on the value of the option.

让我们从源代码构建包，首先使用 ``with_fmt=True``，然后使用 ``with_fmt=False``。
当 *test_package* 运行时，它将根据选项的值显示不同的消息。


.. code-block:: bash

    $ conan create . --build=missing -o with_fmt=True
    -------- Exporting the recipe ----------
    ...

    -------- Testing the package: Running test() ----------
    hello/1.0 (test package): Running test()
    hello/1.0 (test package): RUN: ./example
    hello/1.0: Hello World Release! (with color!)

    $ conan create . --build=missing -o with_fmt=False
    -------- Exporting the recipe ----------
    ...

    -------- Testing the package: Running test() ----------
    hello/1.0 (test package): Running test()
    hello/1.0 (test package): RUN: ./example
    hello/1.0: Hello World Release! (without color)

This is just a simple example of how to use the ``generate()`` method to customize the
toolchain based on the value of one option, but there are lots of other things that you
could do in the ``generate()`` method like:

这只是一个简单的例子，说明如何使用 ``generate()`` 方法根据一个选项的值来定制工具链，
但是还有很多其他事情可以在 ``generate()`` 方法中完成，比如:

* Create a complete custom toolchain based on your needs to use in your build.
  
  根据您在生成中使用的需求创建一个完整的自定义工具链。

* Access to certain information about the package dependencies, like:
    - The configuration accessing the defined
      :ref:`conf_info<conan_conanfile_model_conf_info>`.
    - Accessing the dependencies options.
    - Import files from dependencies using the :ref:`copy tool<conan_tools_files_copy>`.
      You could also import the files create manifests for the package, collecting all
      dependencies versions and licenses.

  访问有关包依赖关系的某些信息，如: 
    - 访问定义的 :ref:`conf_info<conan_conanfile_model_conf_info>` 的配置。
    - 访问依赖项选项。
    - 使用 :ref:`copy tool<conan_tools_files_copy>` 从依赖项导入文件。还可以导入包的文件创建清单，收集所有依赖项版本和许可证。

* Use the :ref:`Environment tools<conan_tools_env_environment_model>` to generate
  information for the system environment.

  使用  :ref:`Environment tools<conan_tools_env_environment_model>` 为系统环境生成信息。

* Adding custom configurations besides *Release* and *Debug*, taking into account the
  settings, like *ReleaseShared* or *DebugShared*.

  除了 *Release* 和  *Debug* 之外添加自定义配置，同时考虑到诸如 *ReleaseShared* 或 *DebugShared* 之类的设置。

Read more
---------

- Use the ``generate()`` method to import files from dependencies.

  使用 ``generate()`` 方法从依赖项导入文件。

- More based on the examples mentioned above ... 

  更基于上面提到的例子...

.. seealso::

    - :ref:`generate() method reference<reference_conanfile_methods_generate>`
