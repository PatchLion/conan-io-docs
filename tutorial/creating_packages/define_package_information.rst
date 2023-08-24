.. _tutorial_creating_define_package_info:

Define information for consumers: the package_info() method
===========================================================

In the previous tutorial section, we explained how to store the headers and binaries of a
library in a Conan package using the :ref:`package
method<creating_packages_package_method>`. Consumers that depend on that package will
reuse those files, but we have to provide some additional information so that Conan can
pass that to the build system and consumers can use the package.

在前面的教程小节中，我们解释了如何使用 :ref:`package method<creating_packages_package_method>`
将库的头和二进制文件存储在 Conan 包中。依赖于该包的消费者将重用这些文件，但是我们必须提供一些附加信息，
以便 Conan 可以将其传递给构建系统，消费者可以使用该包。

For instance, in our example, we are building a static library named *hello* that will
result in a *libhello.a* file in Linux and macOS or a *hello.lib* file in Windows. Also,
we are packaging a header file *hello.h* with the declaration of the library functions.
The Conan package ends up with the following structure in the Conan local cache:

例如，在我们的示例中，我们正在构建一个名为 *hello* 的静态库，它将在 Linux 和 macOS 中生成一个 
*libhello.a* 文件，或者在 Windows 中生成一个 *hello.lib* 文件。
另外，我们将头文件 *hello.h* 与库函数的声明一起打包。Conan 包在 Conan 本地缓存中的结构如下:

.. code-block:: text

    .
    ├── include
    │   └── hello.h
    └── lib
        └── libhello.a

Then, consumers that want to link against this library will need some information:

然后，想要链接到这个库的消费者将需要一些信息:

- The location of the *include* folder in the Conan local cache to search for the
  *hello.h* file.

  在 Conan 本地缓存中搜索 *hello.h* 文件的 *include* 文件夹的位置。

- The name of the library file to link against it (*libhello.a* or *hello.lib*)

  链接到它的库文件的名称(*libhello.a* 或 *hello.lib*)。

- The location of the *lib* folder in the Conan local cache to search for the library
  file.

  在 Conan 本地缓存中搜索库文件的 *lib* 文件夹的位置。

Conan provides an abstraction over all the information consumers may need in the
:ref:`cpp_info<conan_conanfile_model_cppinfo>` attribute of the ConanFile. The information
for this attribute must be set in the :ref:`package_info() method<reference_conanfile_methods_package_info>`. Let's have a look at the
``package_info()`` method of our *hello/1.0* Conan package:

Conan 提供了 ConanFile 的 :ref:`cpp_info<conan_conanfile_model_cppinfo>` 属性中消费者可能需要的所有信息的抽象。
此属性的信息必须在 :ref:`package_info() method<reference_conanfile_methods_package_info>` 方法中设置。
让我们看看 *hello/1.0* Conan 包的 ``package_info()`` 方法:

.. code-block:: python
    :caption: *conanfile.py*

    ...

    class helloRecipe(ConanFile):
        name = "hello"
        version = "1.0"    

        ...

        def package_info(self):
            self.cpp_info.libs = ["hello"]

We can see a couple of things:

我们可以看到一些东西:

- We are adding a *hello* library to the ``libs`` property of the ``cpp_info`` to tell
  consumers that they should link the libraries from that list.

  我们将向 ``cpp_info`` 的 ``libs`` 属性添加一个 *hello* 库，以告诉消费者他们应该链接该列表中的库。

- We are **not adding** information about the *lib* or *include* folders where the
  library and headers files are packaged. The ``cpp_info`` object provides the
  ``.includedirs`` and ``.libdirs`` properties to define those locations but Conan sets
  their value as ``lib`` and ``include`` by default so it's not needed to add those in this
  case. If you were copying the package files to a different location then you have to set
  those explicitly. The declaration of the ``package_info`` method in our Conan package
  would be equivalent to this one:

  我们 **不会添加** 有关打包了库和头文件的  *lib* 或  *include* 文件夹的信息。
  ``cpp_info`` 对象提供了 ``.includedirs``  和 ``.libdirs`` 属性来定义这些位置，
  但是 Conan 将它们的值默认设置为 ``lib`` 和 ``include``，因此在本例中不需要添加这些位置。
  如果要将包文件复制到其他位置，则必须显式地设置这些位置。在我们的 Conan 包中， 
  ``package_info`` 方法的声明与下面这个声明相同:

.. code-block:: python
    :caption: *conanfile.py*

    ...
    
    class helloRecipe(ConanFile):
        name = "hello"
        version = "1.0"    

        ...

        def package_info(self):
            self.cpp_info.libs = ["hello"]
            # conan sets libdirs = ["lib"] and includedirs = ["include"] by default
            self.cpp_info.libdirs = ["lib"]
            self.cpp_info.includedirs = ["include"]


Setting information in the package_info() method
------------------------------------------------

Besides what we explained above about the information you can set in the
``package_info()`` method, there are some typical use cases:

除了我们上面解释的关于您可以在 ``package_info()`` 方法中设置的信息之外，还有一些典型的用例:

- Define information for consumers depending on settings or options
  
  根据设置或选项为使用者定义信息

- Customizing certain information that generators provide to consumers, like the target
  names for CMake or the generated files names for pkg-config for example

  定制生成器提供给消费者的某些信息，例如 CMake 的目标名称或 pkg-config 生成的文件名称

- Propagating configuration values to consumers

  向使用者传播配置值

- Propagating environment information to consumers

  向消费者传播环境信息

- Define components for Conan packages that provide multiple libraries

  为提供多个库的 Conan 包定义组件

Let's see some of those in action. First, clone the project sources if you haven't done so yet. You can
find them in the `examples2.0 repository <https://github.com/conan-io/examples2>`_ on
GitHub:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/creating_packages/package_information


Define information for consumers depending on settings or options
-----------------------------------------------------------------

For this section of the tutorial we introduced some changes in the library and recipe.
Let's check the relevant parts:

在本教程的这一部分中，我们介绍了库和配方中的一些更改，让我们查看相关部分:


Changes introduced in the library sources
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

First, please note that we are using `another branch
<https://github.com/conan-io/libhello/tree/package_info>`_ from the **libhello** library.
Let's check the library's *CMakeLists.txt*:

首先，请注意我们正在使用  **libhello** 库中的 
`another branch <https://github.com/conan-io/libhello/tree/package_info>`_:

.. code-block:: text
    :caption: *CMakeLists.txt*
    :emphasize-lines: 9,11

    cmake_minimum_required(VERSION 3.15)
    project(hello CXX)

    ...

    add_library(hello src/hello.cpp)

    if (BUILD_SHARED_LIBS)
        set_target_properties(hello PROPERTIES OUTPUT_NAME hello-shared)
    else()
        set_target_properties(hello PROPERTIES OUTPUT_NAME hello-static)
    endif()

    ...

As you can see, we are setting the output name for the library depending on whether we are
building the library as static (*hello-static*) or as shared (*hello-shared*). Now let's see
how to translate these changes to the Conan recipe.

正如您所看到的，我们正在设置库的输出名称，这取决于我们是以静态(*hello-static*)还是以
共享(*hello-share*)方式构建库。现在让我们看看如何将这些更改转换为 Conan 配方。


Changes introduced in the recipe
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To update our recipe according to the changes in the library's *CMakeLists.txt* we have to
conditionally set the library name depending on the ``self.options.shared`` option in the
``package_info()`` method:

要根据库的 *CMakeLists.txt* 中的更改更新配方，我们必须根据 ``package_info()`` 
方法中的 ``self.options.shared`` 选项有条件地设置库名称:

.. code-block:: python
    :caption: *conanfile.py*
    :emphasize-lines: 9, 14-17

    class helloRecipe(ConanFile):
        ...

        def source(self):
            git = Git(self)
            git.clone(url="https://github.com/conan-io/libhello.git", target=".")
            # Please, be aware that using the head of the branch instead of an immutable tag
            # or commit is not a good practice in general
            git.checkout("package_info")

        ...

        def package_info(self):
            if self.options.shared:
                self.cpp_info.libs = ["hello-shared"]
            else:
                self.cpp_info.libs = ["hello-static"]


Now, let's create the Conan package with ``shared=False`` (that's the default so no need
to set it explicitly) and check that we are packaging the correct library
(*libhello-static.a* or *hello-static.lib*) and that we are linking the correct library in
the *test_package*.

现在，让我们使用 ``shared=False`` 创建 Conan 包(这是默认值，因此不需要显式设置它) ，
并检查是否打包了正确的库(*libhello-static.a* 或 *hello-static.lib*) ，
并且我们正在链接 *test_package* 中的正确库。

.. code-block:: bash
    :emphasize-lines: 4,14,22

    $ conan create . --build=missing
    ...
    -- Install configuration: "Release"
    -- Installing: /Users/user/.conan2/p/tmp/a311fcf8a63f3206/p/lib/libhello-static.a
    -- Installing: /Users/user/.conan2/p/tmp/a311fcf8a63f3206/p/include/hello.h
    hello/1.0 package(): Packaged 1 '.h' file: hello.h
    hello/1.0 package(): Packaged 1 '.a' file: libhello-static.a
    hello/1.0: Package 'fd7c4113dad406f7d8211b3470c16627b54ff3af' created
    ...
    -- Build files have been written to: /Users/user/.conan2/p/tmp/a311fcf8a63f3206/b/build/Release
    hello/1.0: CMake command: cmake --build "/Users/user/.conan2/p/tmp/a311fcf8a63f3206/b/build/Release" -- -j16
    hello/1.0: RUN: cmake --build "/Users/user/.conan2/p/tmp/a311fcf8a63f3206/b/build/Release" -- -j16
    [ 25%] Building CXX object CMakeFiles/hello.dir/src/hello.cpp.o
    [ 50%] Linking CXX static library libhello-static.a
    [ 50%] Built target hello
    [ 75%] Building CXX object tests/CMakeFiles/test_hello.dir/test.cpp.o
    [100%] Linking CXX executable test_hello
    [100%] Built target test_hello
    hello/1.0: RUN: tests/test_hello
    ...
    [ 50%] Building CXX object CMakeFiles/example.dir/src/example.cpp.o
    [100%] Linking CXX executable example
    [100%] Built target example

    -------- Testing the package: Running test() --------
    hello/1.0 (test package): Running test()
    hello/1.0 (test package): RUN: ./example
    hello/1.0: Hello World Release! (with color!)

As you can see both the tests for the library and the Conan *test_package* linked against
the *libhello-static.a* library successfully.

您可以看到库的测试和成功链接到 *libhello-static.a* 库的 Conan *test_package*。

.. _tutorial_creating_define_package_info_properties:

Properties model: setting information for specific generators
-------------------------------------------------------------

The :ref:`CppInfo<conan_conanfile_model_cppinfo_attributes>` object provides the
``set_property`` method to set information specific to each generator. For example, in
this tutorial, we use the :ref:`CMakeDeps<conan_tools_cmakedeps>` generator to generate the
information that CMake needs to build a project that requires our library. ``CMakeDeps``,
by default, will set a target name for the library using the same name as the Conan
package. If you have a look at that *CMakeLists.txt* from the *test_package*:

:ref:`CppInfo<conan_conanfile_model_cppinfo_attributes>` 对象提供 ``set_property`` 
方法来设置特定于每个生成器的信息。例如，在本教程中，我们使用 :ref:`CMakeDeps<conan_tools_cmakedeps>` 
生成器生成 CMake 构建需要库的项目所需的信息。缺省情况下， ``CMakeDeps`` 将使用与 Conan 
包相同的名称为库设置目标名称。如果您看到 *test_package* 中的 *CMakeLists.txt*:

.. code-block:: cmake
    :caption: test_package *CMakeLists.txt*
    :emphasize-lines: 7

    cmake_minimum_required(VERSION 3.15)
    project(PackageTest CXX)

    find_package(hello CONFIG REQUIRED)

    add_executable(example src/example.cpp)
    target_link_libraries(example hello::hello)

You can see that we are linking with the target name ``hello::hello``. Conan sets this
target name by default, but we can change it using the *properties model*. Let's try to
change it to the name ``hello::myhello``. To do this, we have to set the property
``cmake_target_name`` in the package_info method of our *hello/1.0* Conan package:

您可以看到我们正在链接目标名 ``hello::hello``。Conan 默认设置这个目标名称，
但是我们可以使用 *properties model* 更改它。让我们试着将它改为名称 ``hello::myhello``。为此，
我们必须在 *hello/1.0* Conan 包的 ``package_info`` 方法中设置属性 ``cmake_target_name``:

.. code-block:: python
    :caption: *conanfile.py*
    :emphasize-lines: 10

    class helloRecipe(ConanFile):
        ...

        def package_info(self):
            if self.options.shared:
                self.cpp_info.libs = ["hello-shared"]
            else:
                self.cpp_info.libs = ["hello-static"]

            self.cpp_info.set_property("cmake_target_name", "hello::myhello")


Then, change the target name we are using in the *CMakeLists.txt* in the *test_package*
folder to ``hello::myhello``:

然后，将 *test_package* 文件夹中的 *CMakeLists.txt* 中使用的目标名称更改为 ``hello::myhello``:

.. code-block:: cmake
    :caption: test_package *CMakeLists.txt*
    :emphasize-lines: 4

    cmake_minimum_required(VERSION 3.15)
    project(PackageTest CXX)
    # ...
    target_link_libraries(example hello::myhello)

And re-create the package:

.. code-block:: bash
    :emphasize-lines: 14

    $ conan create . --build=missing
    Exporting the recipe
    hello/1.0: Exporting package recipe
    hello/1.0: Using the exported files summary hash as the recipe revision: 44d78a68b16b25c5e6d7e8884b8f58b8 
    hello/1.0: A new conanfile.py version was exported
    hello/1.0: Folder: /Users/user/.conan2/p/a8cb81b31dc10d96/e
    hello/1.0: Exported revision: 44d78a68b16b25c5e6d7e8884b8f58b8
    ...
    -------- Testing the package: Building --------
    hello/1.0 (test package): Calling build()
    ...
    -- Detecting CXX compile features
    -- Detecting CXX compile features - done
    -- Conan: Target declared 'hello::myhello'
    ...
    [100%] Linking CXX executable example
    [100%] Built target example

    -------- Testing the package: Running test() --------
    hello/1.0 (test package): Running test()
    hello/1.0 (test package): RUN: ./example
    hello/1.0: Hello World Release! (with color!)

You can see how Conan now declares the ``hello::myhello`` instead of the default
``hello::hello`` and the *test_package* builds successfully.

您可以看到 Conan 现在如何声明 ``hello::myhello`` 而不是默认的 
``hello::hello``，并且 *test_package* 成功构建。

The target name is not the only property you can set in the CMakeDeps generator. For a
complete list of properties that affect the CMakeDeps generator behaviour, please check
the :ref:`reference<CMakeDeps Properties>`. 

目标名称不是您可以在 CMakeDeps 生成器中设置的唯一属性。有关影响 CMakeDeps 生成器行为的属性的完整列表，
请检查 :ref:`reference<CMakeDeps Properties>`。

Propagating environment or configuration information to consumers
-----------------------------------------------------------------

You can provide environment information to consumers in the ``package_info()``. To do so,
you can use the ConanFile's :ref:`runenv_info<conan_conanfile_attributes_runenv_info>` and
:ref:`buildenv_info<conan_conanfile_attributes_buildenv_info>` properties:

您可以在 ``package_info()`` 中向使用者提供环境信息。为此，可以使用 ConanFile 的
:ref:`runenv_info<conan_conanfile_attributes_runenv_info>`  和 
:ref:`buildenv_info<conan_conanfile_attributes_buildenv_info>` 属性:

* ``runenv_info`` :ref:`Environment<conan_tools_env_environment_model>` object that
  defines environment information that consumers that use the package may need when
  **running**. 

  ``runenv_info`` :ref:`Environment<conan_tools_env_environment_model>` 对象，该对象定义使用包的消费者在 
  **运行** 时可能需要的环境信息。

* ``buildenv_info`` :ref:`Environment<conan_tools_env_environment_model>` object that
  defines environment information that consumers that use the package may need when
  **building**. 

  ``buildenv_info`` :ref:`Environment<conan_tools_env_environment_model>` 对象，该对象定义使用包的消费者在 
  **构建** 时可能需要的环境信息。

Please note that it's not necessary to add ``cpp_info.bindirs`` to ``PATH`` or
``cpp_info.libdirs`` to ``LD_LIBRARY_PATH``, those are automatically added by the
:ref:`VirtualBuildEnv<conan_tools_env_virtualbuildenv>` and
:ref:`VirtualRunEnv<conan_tools_env_virtualrunenv>`.

请注意，没有必要添加 ``cpp_info.bindirs`` 到 ``PATH`` 或 ``cpp_info.libdirs`` 添加到 
``LD_LIBRARY_PATH``，这些是由 :ref:`VirtualBuildEnv<conan_tools_env_virtualbuildenv>` 和
:ref:`VirtualRunEnv<conan_tools_env_virtualrunenv>` 自动添加的。

You can also define configuration values in the ``package_info()`` so that consumers can
use that information. To do this, set the
:ref:`conf_info<conan_conanfile_model_conf_info>` property of the ConanFile.

您还可以在 ``package_info()`` 中定义配置值，以便使用者可以使用该信息。为此，请设置 ConanFile 的 
:ref:`conf_info<conan_conanfile_model_conf_info>` 属性。

To know more about this use case, please check the :ref:`corresponding
example<examples_conanfile_package_info_conf_and_env>`.

要了解更多关于这个用例的信息，请查看 :ref:`corresponding example<examples_conanfile_package_info_conf_and_env>`。

Define components for Conan packages that provide multiple libraries
--------------------------------------------------------------------

There are cases in which a Conan package may provide multiple libraries, for these cases
you can set the separate information for each of those libraries using the components
attribute from the :ref:`CppInfo<conan_conanfile_model_cppinfo_attributes>` object.

在某些情况下，Conan 包可能提供多个库，对于这些情况，您可以使用 
:ref:`CppInfo<conan_conanfile_model_cppinfo_attributes>` 对象的组件属性为每个库设置单独的信息。

To know more about this use case, please check the :ref:`components
example<examples_conanfile_package_info_components>` in the examples section.

要了解更多关于这个用例的信息，请查看示例部分中的 :ref:`components example<examples_conanfile_package_info_components>` 。

Read more
---------

.. container:: examples

    - :ref:`Propagating environment and configuration information to consumers example<examples_conanfile_package_info_conf_and_env>`
    - :ref:`Define components for Conan packages that provide multiple libraries example<examples_conanfile_package_info_components>`


.. seealso::

    - :ref:`package_info() reference<reference_conanfile_methods_package_info>`
