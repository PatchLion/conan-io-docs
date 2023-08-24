.. _editable_packages:

Packages in editable mode
=========================

The normal way of working with Conan packages is to run a ``conan create`` or ``conan
export-pkg`` to store them in the local cache, so that consumers use the packages stored
in the cache. In some cases, when you want to consume these packages while developing
them, it can be tedious to run ``conan create`` each time you make changes to the package.
For those cases, you can put your package in editable mode, and consumers will be able to
find the headers and artifacts in your local working directory, eliminating the need for
packaging.

使用 Conan 包的一般方法是运行一个 ``conan create`` 或 ``conan export-pkg`` 来将它们存储在本地缓存中，
以便使用者使用存储在缓存中的包。在某些情况下，当您希望在开发这些包时使用它们时，
每次对包进行更改时都要运行 ``conan create`` 可能会非常繁琐。对于这些情况，您可以将包放在可编辑模式下，
消费者将能够在您的本地工作目录中找到标题和工件，从而消除了打包的需要。

Let's see how we can put a package in editable mode and consume it from the local working
directory.

让我们看看如何将一个包放在可编辑模式下，并从本地工作目录使用它。

Please, first of all, clone the sources to recreate this project. You can find them in the
`examples2.0 repository <https://github.com/conan-io/examples2>`_ in GitHub:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/developing_packages/editable_packages

There are 2 folders inside this project:

..  code-block:: text

    .
    ├── hello
    │   ├── CMakeLists.txt
    │   ├── conanfile.py
    │   └── src
    │       └── hello.cpp
    └── say
        ├── CMakeLists.txt
        ├── conanfile.py
        ├── include
        │   └── say.h
        └── src
            └── say.cpp


- A "say" folder containing a fully fledge package, with its ``conanfile.py`` and its source
  code.

  一个"say"文件夹，其中包含一个完整的软件包，以及它的 ``conanfile.py`` 和源代码。

- A "hello" folder containing a simple consumer project with a ``conanfile.py`` and its
  source code, which depends on the ``say/1.0`` requirement.
  
  一个"hello"文件夹，包含一个带有 ``conanfile.py`` 及其源代码的简单消费者项目，依赖 ``say/1.0``。

We will put ``say/1.0`` in editable mode and show how the ``hello`` consumer can find ``say/1.0``
headers and binaries in its local working directory.

我们将把 ``say/1.0`` 设置为可编辑模式，并展示 ``hello`` 用户如何在其本地工作目录中找到  ``say/1.0`` 头文件和二进制文件。

Put say/1.0 package in editable mode
------------------------------------

To avoid creating the package ``say/1.0`` in the cache for every change, we are going to
put that package in editable mode, creating **a link from the reference in the cache to
the local working directory**:

为了避免为每次更改在缓存中创建包  ``say/1.0`` ，我们将该包置于可编辑模式，创建 **从缓存中的引用到本地工作目录的链接**:

.. code-block:: bash

    $ conan editable add say --name=say --version=1.0
    $ conan editable list
    say/1.0
        Path: /Users/.../examples2/tutorial/developing_packages/editable_packages/say/conanfile.py


From now on, every usage of ``say/1.0`` by any other Conan package or project will be
redirected to the
``/Users/.../examples2/tutorial/developing_packages/editable_packages/say/conanfile.py``
user folder instead of using the package from the Conan cache.

从现在开始，任何其他 Conan 软件包或项目使用 ``say/1.0`` 的所有用法都将被重定向到 
``/Users/.../examples2/tutorial/developing_packages/editable_packages/say/conanfile.py`` 用户文件夹，
而不是使用来自 Conan 缓存的软件包。

Note that the key of editable packages is a correct definition of the ``layout()`` of the
package. Read the :ref:`package layout() section <reference_conanfile_methods_layout>` to learn more
about this method. 

注意，可编辑包的键是包的 ``layout()`` 的正确定义。阅读 
:ref:`package layout() section <reference_conanfile_methods_layout>` 了解有关此方法的更多信息。

In this example, the ``say`` ``conanfile.py`` recipe is using the predefined
``cmake_layout()`` which defines the typical CMake project layout that can be different
depending on the platform and generator used.

在这个示例中， ``say`` 的 ``conanfile.py`` 配方使用预定义的 ``cmake_layout()`` ，它定义了典型的 CMake 项目布局，
该布局可以根据所使用的平台和生成器的不同而有所不同。

Now that the ``say/1.0`` package is in editable mode, let's build it locally:

现在 ``say/1.0`` 包处于可编辑模式，让我们在本地构建它:

.. include:: ../cmake_presets_note.inc

.. code-block:: bash

    $ cd say

    # Windows: we will build 2 configurations to show multi-config
    $ conan install . -s build_type=Release
    $ conan install . -s build_type=Debug
    $ cmake --preset conan-default
    $ cmake --build --preset conan-release
    $ cmake --build --preset conan-debug

    # Linux, MacOS: we will only build 1 configuration
    $ conan install .
    $ cmake --preset conan-release
    $ cmake --build --preset conan-release


Using say/1.0 package in editable mode
--------------------------------------

Consuming a package in editable mode is transparent from the consumer perspective.
In this case we can build the ``hello`` application as usual:

从使用者的角度来看，以可编辑模式使用包是透明的。在这种情况下，
我们可以像往常一样构建 hello 应用程序:

.. code-block:: bash

    $ cd ../hello

    # Windows: we will build 2 configurations to show multi-config
    $ conan install . -s build_type=Release
    $ conan install . -s build_type=Debug
    $ cmake --preset conan-default
    $ cmake --build --preset conan-release
    $ cmake --build --preset conan-debug
    $ build\Release\hello.exe
    say/1.0: Hello World Release!
    ...
    $ build\Debug\hello.exe
    say/1.0: Hello World Debug!
    ...

    # Linux, MacOS: we will only build 1 configuration
    $ conan install .
    $ cmake --preset conan-release
    $ cmake --build --preset conan-release
    $ ./build/Release/hello
    say/1.0: Hello World Release!

As you can see, ``hello`` can successfully find ``say/1.0`` header and library files.

如您所见， ``hello`` 可以成功地查找 ``say/1.0`` 头文件和库文件。

Working with editable packages
------------------------------

Once the above steps have been completed, you can work with your build system or IDE
without involving Conan and make changes to the editable packages. The consumers will use
those changes directly. Let's see how this works by making a change in the ``say`` source
code:

一旦完成了上述步骤，您就可以在不涉及 Conan 的情况下使用构建系统或 IDE，
并对可编辑包进行更改。消费者将直接使用这些更改。让我们通过对 ``say`` 源代码进行更改来看看这是如何工作的:

.. code-block:: bash

    $ cd ../say
    # Edit src/say.cpp and change the error message from "Hello" to "Bye"

    # Windows: we will build 2 configurations to show multi-config
    $ cmake --build --preset conan-release
    $ cmake --build --preset conan-debug

    # Linux, MacOS: we will only build 1 configuration
    $ cmake --build --preset conan-release


And build and run the "hello" project:

.. code-block:: bash

    $ cd ../hello

    # Windows
    $ cd build
    $ cmake --build --preset conan-release
    $ cmake --build --preset conan-debug
    $ Release\hello.exe
    say/1.0: Bye World Release!
    $ Debug\hello.exe
    say/1.0: Bye World Debug!

    # Linux, MacOS
    $ cmake --build --preset conan-release
    $ ./hello
    say/1.0: Bye World Release!


In this manner, you can develop both the ``say`` library and the ``hello`` application
simultaneously without executing any Conan command in between. If you have both open in
your IDE, you can simply build one after the other.

通过这种方式，您可以同时开发 ``say`` 库和 ``hello`` 应用程序，而不必在其间执行任何 
Conan 命令。如果您的 IDE 中都打开了这两个版本，那么您可以简单地构建一个接一个的版本。

Revert the editable mode
------------------------

In order to revert the editable mode just remove the link using:

要恢复可编辑模式，只需删除链接，使用:

.. code-block:: bash

    $ conan editable remove --refs=say/1.0

It will remove the link (the local directory won't be affected) and all the packages consuming this
requirement will get it from the cache again.

它将删除链接(本地目录不会受到影响) ，所有使用此需求的包将再次从缓存中获取该链接。

.. warning::

    Packages that are built while consuming an editable package in their upstreams can
    generate binaries and packages that are incompatible with the released version of the
    editable package. Avoid uploading these packages without re-creating them with the
    in-cache version of all the libraries.

    在上游使用可编辑包时构建的包可能会生成与可编辑包的发布版本不兼容的二进制文件和包。
    避免上传这些包，而不用所有库的缓存版本重新创建它们。
