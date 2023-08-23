.. _consuming_packages_cross_building_with_conan:

How to cross-compile your applications using Conan: host and build contexts
===========================================================================

Please, first clone the sources to recreate this project. You can find them in the
`examples2.0 repository <https://github.com/conan-io/examples2>`_ on GitHub:

请首先复制源代码来重新创建这个项目，您可以在 GitHub 上的 
`examples2.0 repository <https://github.com/conan-io/examples2>`_ 中找到它们:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/consuming_packages/cross_building


In the previous examples, we learned how to use a *conanfile.py* or *conanfile.txt* to
build an application that compresses strings using the *Zlib* and *CMake* Conan packages.
Also, we explained that you can set information like the operating system, compiler or
build configuration in a file called the Conan profile. You can use that profile as an
argument (:command:`--profile`) to invoke the :command:`conan install`. We also explained that
not specifying that profile is equivalent to using the :command:`--profile=default` argument.

在前面的示例中，我们学习了如何使用 *conanfile.py* 或 *conanfile.txt* 来构建使用 *Zlib* 和 *CMake* Conan 
包压缩字符串的应用程序。另外，我们还解释了可以在名为 Conan 配置文件的文件中设置操作系统、
编译器或构建配置等信息。您可以使用该概要文件作为参数(:command:`--profile`)来调用 :command:`conan install`。
我们还解释了不指定 profile 等同于使用  :command:`--profile=default` 参数。

For all those examples, we used the same platform for building and running the
application. But, what if you want to build the application on your machine running Ubuntu
Linux and then run it on another platform like a
Raspberry Pi? Conan can model that case using two different profiles, one for the
machine that **builds** the application (Ubuntu Linux) and another for the machine that
**runs** the application (Raspberry Pi). We will explain this "two profiles" approach in
the next section.

对于所有这些示例，我们使用相同的平台来构建和运行应用程序。但是，如果您希望在运行 Ubuntu Linux 
的机器上构建应用程序，然后在另一个像 Raspberry Pi 这样的平台上运行它，该怎么办呢？Conan 
可以使用两个不同的概要文件对这个案例进行建模，一个用于 **builds** 应用程序的机器(Ubuntu Linux) ，
另一个用于 **runs** 应用程序的机器(Raspberry Pi)。我们将在下一节中解释这种 “两个 profiles” 方法。

Conan two profiles model: build and host profiles
-------------------------------------------------

Even if you specify only one :command:`--profile` argument when invoking Conan, Conan will
internally use two profiles. One for the machine that **builds** the binaries (called the
**build** profile) and another for the machine that **runs** those binaries (called the
**host** profile). Calling this command:

即使在调用 Conan 时只指定一个 :command:`--profile` 参数，Conan 也会在内部使用两个 profile。
一个用于 **builds** 二进制文件的计算机(称为 **builds** 配置文件) ，另一个用于 **runs** 
这些二进制文件的计算机(称为 **host**  配置文件)。调用此命令:

.. code-block:: bash

    $ conan install . --build=missing --profile=someprofile

Is equivalent to:

等同于:

.. code-block:: bash

    $ conan install . --build=missing --profile:host=someprofile --profile:build=default


As you can see we used two new arguments:

正如你所看到的，我们使用了两个新的参数:

* ``profile:host``: This is the profile that defines the platform where the built binaries
  will run. For our string compressor application this profile would be the one applied
  for the *Zlib* library that will run in a **Raspberry Pi**.

  这个概要文件定义了运行构建的二进制文件的平台。对于我们的字符串压缩器应用程序，
  这个概要文件应用于将在 **Raspberry Pi** 中运行的 *Zlib* 库。
  
* ``profile:build``: This is the profile that defines the platform where the binaries will be built. For our string compressor application, this profile would be the one
  used by the *CMake* tool that will compile it on the **Ubuntu Linux** machine.

  这个概要文件定义了构建二进制文件的平台。对于我们的字符串压缩器应用程序，
  这个概要文件将由将在 **Ubuntu Linux** 机器上编译它的 *CMake* 工具使用。

Note that when you just use one argument for the profile ``--profile`` is equivalent to
``--profile:host``. If you don't specify the ``--profile:build`` argument, Conan will use
the *default* profile internally.

请注意，当您只为配置文件使用一个参数时 ``--profile`` 等效于 ``--profile:host``。
如果不指定 ``--profile:build`` 参数，Conan 将在内部使用 *default* 配置文件。

So, if we want to build the compressor application in the Ubuntu Linux machine but run it
in a Raspberry Pi, we should use two different profiles. For the **build** machine we
could use the default profile, that in our case looks like this:

因此，如果我们想在 Ubuntu Linux 机器上构建压缩程序，但是在 Raspberry Pi 中运行它，
我们应该使用两个不同的配置文件。对于 **build** 机器，我们可以使用默认的配置文件，在我们的例子中如下所示:

.. code-block:: bash
    :caption: <conan home>/profiles/default

    [settings]
    os=Linux
    arch=x86_64
    build_type=Release
    compiler=gcc
    compiler.cppstd=gnu14
    compiler.libcxx=libstdc++11
    compiler.version=9

And the profile for the Raspberry Pi that is the **host** machine:

还有 Raspberry Pi 的 profile，也就是 **host** 机器:

.. code-block:: bash
    :caption: <local folder>/profiles/raspberry
    :emphasize-lines: 9-12

    [settings]
    os=Linux
    arch=armv7hf
    compiler=gcc
    build_type=Release
    compiler.cppstd=gnu14
    compiler.libcxx=libstdc++11
    compiler.version=9
    [buildenv]
    CC=arm-linux-gnueabihf-gcc-9
    CXX=arm-linux-gnueabihf-g++-9
    LD=arm-linux-gnueabihf-ld

.. important::

    Please, take into account that in order to build this example successfully, you should
    have installed a toolchain that includes the compiler and all the tools to build the
    application for the proper architecture. In this case the host machine is a Raspberry
    Pi 3 with *armv7hf* architecture operating system and we have the
    *arm-linux-gnueabihf* toolchain installed in the Ubuntu machine.

    请考虑到，为了成功地构建这个示例，您应该已经安装了一个工具链，
    其中包含了编译器和构建适当体系结构的应用程序的所有工具。在这种情况下，
    主机是一个 Raspberry Pi 3和 *armv7hf* 体系结构操作系统，
    我们在 Ubuntu 机器上安装了 *arm-linux-gnueabihf* 工具链。

If you have a look at the *raspberry* profile, there is a section named
``[buildenv]``. This section is used to set the environment variables that are needed to
build the application. In this case we declare the ``CC``, ``CXX`` and ``LD`` variables
pointing to the cross-build toolchain compilers and linker, respectively. Adding this
section to the profile will invoke the VirtualBuildEnv generator everytime we do a
:command:`conan install`. This generator will add that environment information to the
``conanbuild.sh`` script that we will source before building with CMake so that it can use
the cross-build toolchain.

如果你看了 *raspberry* 的配置文件，有一个部分命名为 ``[buildenv]``。本节用于设置构建应用程序所需的环境变量。
在本例中，我们分别声明指向跨构建工具链编译器和链接器的 ``CC``, ``CXX`` 和 ``LD`` 变量。
将此部分添加到配置文件中将在每次执行 :command:`conan install` 时调用 VirtualBuildEnv 生成器。
这个生成器将把这个环境信息添加到 ``conanbuild.sh`` 脚本中，我们将在使用 CMake 构建之前提供这个脚本，
这样它就可以使用交叉构建工具链。

Build and host contexts
^^^^^^^^^^^^^^^^^^^^^^^

Now that we have our two profiles prepared, let's have a look at our *conanfile.py*:

现在我们已经准备好了两个配置文件，让我们看一下 *conanfile.py*:

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

As you can see, this is practically the same *conanfile.py* we used in the :ref:`previous
example<consuming_packages_flexibility_of_conanfile_py>`. We will require **zlib/1.2.11**
as a regular dependency and **cmake/3.22.6** as a tool needed for building the
application.

正如您所看到的，这实际上与我们在 :ref:`previous example<consuming_packages_flexibility_of_conanfile_py>` 
中使用的 *conanfile.py* 相同。我们需要 **zlib/1.2.11** 作为常规依赖项，而 **cmake/3.22.6** 作为构建应用程序所需的工具。

We will need the application to build for the Raspberry Pi with the cross-build
toolchain and also link the **zlib/1.2.11** library built for the same platform. On the
other side, we need the **cmake/3.22.6** binary to run in Ubuntu Linux. Conan manages this
internally in the dependency graph differentiating between what we call the "build
context" and the "host context":

我们需要使用跨构建工具链为 Raspberry Pi 构建应用程序，还需要链接为同一平台构建的 **zlib/1.2.11** 库。
另一方面，我们需要在 Ubuntu Linux 中运行 **cmake/3.22.6** 二进制文件。Conan在依赖关系图中对此进行了内部管理，
这个依赖关系图区分了我们所说的“构建上下文”和“主机上下文”:

* The **host context** is populated with the root package (the one specified in the
  :command:`conan install` or :command:`conan create` command) and all its requirements
  added via ``self.requires()``. In this case, this includes the compressor application
  and the **zlib/1.2.11** dependency.

  **host context** 使用根包(在 :command:`conan install` 或 :command:`conan create` 命令中指定的那个)
  和通过 ``self.requires()``  添加的所有 requirements 来填充。在本例中，
  这包括压缩程序应用程序和 **zlib/1.2.11** 依赖项。

* The **build context** contains the tool requirements used in the build machine. This
  category typically includes all the developer tools like CMake, compilers and linkers.
  In this case, this includes the **cmake/3.22.6** tool.

  **build context** 包含生成计算机中使用的工具requirements。此类别通常包括所有开发人员工具，
  如 CMake、编译器和链接器。在本例中，这包括 **cmake/3.22.6** 工具。

These contexts define how Conan will manage each one of the dependencies. For example, as
**zlib/1.2.11** belongs to the **host context**, the ``[buildenv]`` build environment we
defined in the **raspberry** profile (profile host) will only apply to the **zlib/1.2.11**
library when building and won't affect anything that belongs to the **build context** like
the **cmake/3.22.6** dependency.

这些上下文定义了 Conan 将如何管理每个依赖项。例如，由于 **zlib/1.2.11** 属于 **host context**，
所以我们在 **raspberry** 配置文件(配置文件主机)中定义的 ``[buildenv]`` 构建环境只会在构建时应用于 
**zlib/1.2.11** 库，不会影响任何属于 **build context** 的东西，比如 **cmake/3.22.6** 依赖项。

Now, let's build the application. First, call :command:`conan install` with the
profiles for the build and host platforms. This will install the  **zlib/1.2.11**
dependency built for *armv7hf* architecture and a **cmake/3.22.6** version that runs for
64-bit architecture.

现在，让我们构建应用程序。首先，使用构建和主机平台的profiles调用 :command:`conan install` 。
这将安装为 *armv7hf* 架构构建的 **zlib/1.2.11** 依赖项和运行于64位架构的 **cmake/3.22.6** 版本。

.. code-block:: bash
    
    $ conan install . --build missing -pr:b=default -pr:h=./profiles/raspberry

Then, let's call CMake to build the application. As we did in the previous example we have
to activate the **build environment** running ``source Release/generators/conanbuild.sh``. That will
set the environment variables needed to locate the cross-build toolchain and build the
application.

然后，让我们调用 CMake 来构建应用程序。正如我们在前面的示例中所做的那样，
我们必须激活运行 ``source Release/generators/conanbuild.sh`` 的 **build environment**。
这将设置定位交叉构建工具链和构建应用程序所需的环境变量。

.. code-block:: bash

    $ cd build
    $ source Release/generators/conanbuild.sh
    Capturing current environment in deactivate_conanbuildenv-release-armv7hf.sh
    Configuring environment variables    
    $ cmake .. -DCMAKE_TOOLCHAIN_FILE=Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
    $ cmake --build .
    ...
    -- Conan toolchain: C++ Standard 14 with extensions ON
    -- The C compiler identification is GNU 9.4.0
    -- Detecting C compiler ABI info
    -- Detecting C compiler ABI info - done
    -- Check for working C compiler: /usr/bin/arm-linux-gnueabihf-gcc-9 - skipped
    -- Detecting C compile features
    -- Detecting C compile features - done    [100%] Built target compressor
    ...
    $ source Release/generators/deactivate_conanbuild.sh

You could check that we built the application for the correct architecture by running the
``file`` Linux utility:

您可以通过运行 ``file``  Linux 实用程序来检查我们是否为正确的体系结构构建了应用程序:

.. code-block:: bash
    :emphasize-lines: 2

    $ file compressor
    compressor: ELF 32-bit LSB shared object, ARM, EABI5 version 1 (SYSV), dynamically
    linked, interpreter /lib/ld-linux-armhf.so.3,
    BuildID[sha1]=2a216076864a1b1f30211debf297ac37a9195196, for GNU/Linux 3.2.0, not
    stripped


Read more
---------

.. container:: examples

    - :ref:`Cross building to Android with the NDK<examples_cross_build_android_ndk>`
    - :ref:`VirtualBuildEnv reference <conan_tools_env_virtualbuildenv>`
    - Cross-build using a tool_requires
    - How to require test frameworks like gtest: using ``test_requires``
    - Using Conan to build for iOS
