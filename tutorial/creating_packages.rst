.. _tutorial_creating_packages:

Creating packages
=================

This section shows how to create Conan packages using a Conan recipe. We begin by creating
a basic Conan recipe to package a simple C++ library that you can scaffold using the
:command:`conan new` command. Then, we will explain the different methods that you can
define inside a Conan recipe and the things you can do inside them:

本节展示如何使用 Conan 配方创建 Conan 包。我们首先创建一个基本的 Conan 
配方来打包一个简单的 C++ 库，您可以使用 :command:`conan new` 命令构建这个库。然后，我们将解释不同的方法，
你可以定义一个Conan配方和你可以在它们里面做的事情:

* Using the ``source()`` method to retrieve sources from external repositories and apply
  patches to those sources.

  使用 ``source()`` 方法从外部存储库检索源代码并对这些源代码应用补丁。

* Add requirements to your Conan packages inside the ``requirements()`` method. 

  在 ``requirements()`` 方法中将requirements添加到您的 Conan 包中。

* Use the ``generate()`` method to prepare the package build, and customize the toolchain.

  使用  ``generate()`` 方法准备包构建，并自定义工具链。

* Configure settings and options in the ``configure()`` and ``config_options()``
  methods and how they affect the packages' binary compatibility.

  在 ``configure()`` 和 ``config_options()`` 方法中配置设置和选项，以及它们如何影响包的二进制兼容性。

* Use the ``build()`` method to customize the build process and launch the tests for the
  library you are packaging. 

  使用 ``build()`` 方法可以自定义生成过程并启动要打包的库的测试。

* Select which files will be included in the Conan package using the ``package()`` method.

  使用 ``package()`` 方法选择哪些文件将包含在 Conan 包中。

* Define the package information in the ``package_info()`` method so that consumers
  of this package can use it.

  在 ``package_info()`` 方法中定义包信息，以便此包的使用者可以使用它。

* Use a *test_package* to test that the Conan package can be consumed correctly.

  使用 *test_package* 测试 Conan 包是否可以正确使用。

After this walkthrough around some Conan recipe methods, we will explain some
peculiarities of different types of Conan packages like, for example, header-only
libraries, packages for pre-built binaries, packaging tools for building other packages or
packaging your own applications.

在本文介绍了一些 Conan 配方方法之后，我们将解释不同类型的 Conan 包的一些特性，
例如，仅标头库、用于预构建二进制文件的包、用于构建其他包或打包您自己的应用程序的包。

.. toctree::
   :maxdepth: 2
   :caption: Table of contents
   
   creating_packages/create_your_first_package
   creating_packages/handle_sources_in_packages
   creating_packages/add_dependencies_to_packages
   creating_packages/preparing_the_build
   creating_packages/configure_options_settings
   creating_packages/build_packages
   creating_packages/package_method
   creating_packages/define_package_information
   creating_packages/test_conan_packages
   creating_packages/other_types_of_packages
