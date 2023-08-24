.. _creating_packages_handle_sources_in_packages:

Handle sources in packages
==========================

In the :ref:`previous tutorial section<creating_packages_create_your_first_conan_package>`
we created a Conan package for a "Hello World" C++ library. We used the
``exports_sources`` attribute of the Conanfile to declare the location of the sources for
the library. This method is the simplest way to define the location of the source files
when they are in the same folder as the Conanfile. However, sometimes the source files are
stored in a repository or a file in a remote server, and not in the same location as the
Conanfile. In this section, we will modify the recipe we created previously by adding a
``source()`` method and explain how to:

在 :ref:`previous tutorial section<creating_packages_create_your_first_conan_package>` 
中，我们为 "Hello World" C++ 库创建了一个 Conan 包。我们使用 Conanfile 的 ``exports_sources`` 
属性来声明库的源的位置。当源文件与 Conanfile 位于同一文件夹中时，此方法是定义源文件位置的最简单方法。
但是，有时源文件存储在存储库或远程服务器中的文件中，而不在 Conanfile 的同一位置。在本节中，
我们将通过添加 ``source()`` 方法来修改前面创建的菜谱，并解释如何:

* Retrieve the sources from a *zip* file stored in a remote repository.
 
  从存储在远程存储库中的 *zip* 文件中检索源代码。

* Retrieve the sources from a branch of a *git* repository.

  从 *git* 存储库的分支检索源代码。

Please, first clone the sources to recreate this project. You can find them in the
`examples2.0 repository <https://github.com/conan-io/examples2>`_ on GitHub:

请首先克隆这些源代码来重新创建这个项目，您可以在 GitHub 上的 
`examples2.0 repository <https://github.com/conan-io/examples2>`_  中找到它们:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/creating_packages/handle_sources

The structure of the project is the same as the one in the previous example but without
the library sources:

.. code-block:: text

    .
    ├── CMakeLists.txt
    ├── conanfile.py
    └── test_package
        ├── CMakeLists.txt
        ├── conanfile.py
        └── src
            └── example.cpp

Sources from a *zip* file stored in a remote repository
-------------------------------------------------------

Let's have a look at the changes in the *conanfile.py*:

.. code-block:: python

    from conan import ConanFile
    from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
    from conan.tools.files import get


    class helloRecipe(ConanFile):
        name = "hello"
        version = "1.0"

        ...

        # Binary configuration
        settings = "os", "compiler", "build_type", "arch"
        options = {"shared": [True, False], "fPIC": [True, False]}
        default_options = {"shared": False, "fPIC": True}

        def source(self):
            get(self, "https://github.com/conan-io/libhello/archive/refs/heads/main.zip", 
                      strip_root=True)

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

As you can see, the recipe is the same but instead declaring the ``exports_sources``
attribute as we did previously:

正如您所看到的，配方是相同的，但是声明 ``exports_sources`` 属性与前面一样:

.. code-block:: python

    exports_sources = "CMakeLists.txt", "src/*", "include/*"


We declare a ``source()`` method with this information:

我们使用以下信息声明 ``source()`` 方法:

.. code-block:: python

    def source(self):
        get(self, "https://github.com/conan-io/libhello/archive/refs/heads/main.zip", 
                  strip_root=True)

We used the :ref:`conan.tools.files.get()<conan_tools_files_get>` tool that will first
**download** the *zip* file from the URL that we pass as an argument and then **unzip**
it. Note that we pass the ``strip_root=True`` argument so that if all the unzipped
contents are in a single folder, all the contents are moved to the parent folder (check
the :ref:`conan.tools.files.unzip()<conan_tools_files_unzip>` reference for more details).

我们使用了 :ref:`conan.tools.files.get()<conan_tools_files_get>` 工具，
它首先从作为参数传递的 URL **下载** *zip* 文件， 然后 **解压** 它。
请注意，我们传递 ``strip_root=True`` 参数，这样如果所有解压缩的内容都在一个文件夹中，
那么所有内容都会移动到父文件夹(查看 :ref:`conan.tools.files.unzip()<conan_tools_files_unzip>` 
引用以获得更多细节)。

The contents of the zip file are the same as the sources we previously had beside the
Conan recipe, so if you do a :command:`conan create` the results will be the
same as before.

Zip 文件的内容与我们之前在 Conan 配方旁边使用的源代码相同，因此如果您使用 :command:`conan create`，
结果将与之前相同。

.. code-block:: text
    :emphasize-lines: 8-13

    $ conan create .

    ...

    -------- Installing packages ----------

    Installing (downloading, building) binaries...
    hello/1.0: Calling source() in /Users/user/.conan2/p/0fcb5ffd11025446/s/.
    Downloading update_source.zip

    hello/1.0: Unzipping 3.7KB
    Unzipping 100 %                                                       
    hello/1.0: Copying sources to build folder
    hello/1.0: Building your package in /Users/user/.conan2/p/tmp/369786d0fb355069/b

    ...

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

Please, check the highlighted lines with the messages about the download and unzip operation.

请检查有关下载和解压缩操作的消息的高亮显示行。

Sources from a branch in a *git* repository
-------------------------------------------

Now, let's modify the ``source()`` method to bring the sources from a *git* repository
instead of a *zip* file. We show just the relevant parts:

现在，让我们修改 ``source()`` 方法，从 *git* 存储库而不是 *zip* 文件中获取源代码。我们只显示相关部分:

.. code-block:: python

    ...

    from conan.tools.scm import Git


    class helloRecipe(ConanFile):
        name = "hello"
        version = "1.0"

        ...

        def source(self):
            git = Git(self)
            git.clone(url="https://github.com/conan-io/libhello.git", target=".")

        ...


Here, we use the :ref:`conan.tools.scm.Git()<reference>` tool. The ``Git`` class
implements several methods to work with *git* repositories. In this case, we call the clone
method to clone the `<https://github.com/conan-io/libhello.git>`_ repository in the
default branch using the same folder for cloning the sources instead of a subfolder
(passing the ``target="."`` argument). 

在这里，我们使用 :ref:`conan.tools.scm.Git()<reference>` 工具。 ``Git`` 类实现了几种使用 
*git* 存储库的方法。在这种情况下，我们调用克隆方法来克隆 `<https://github.com/conan-io/libhello.git>`_ 
存储库的默认分支，并使用相同的目录而不是子目录(传递 ``target="."`` 参数)。

If we wanted to checkout a commit or tag in the repository we could use the ``checkout()``
method of the Git tool:

如果我们想签出存储库中的提交或标记，我们可以使用 Git 工具的 ``checkout()`` 方法:

.. code-block:: python

    def source(self):
        git = Git(self)
        git.clone(url="https://github.com/conan-io/libhello.git", target=".")
        git.checkout("<branch name>, <tag> or <commit hash>")

For more information about the ``Git`` class methods, please check the
:ref:`conan.tools.scm.Git()<reference>` reference.

Note that it's also possible to run other commands by invoking the ``self.run()`` method.

注意，还可以通过调用  ``self.run()`` 方法运行其他命令。

.. _creating_packages_handle_sources_in_packages_conandata:

Using the conandata.yml file
----------------------------

We can write a file named ``conandata.yml`` in the same folder of the ``conanfile.py``.
This file will be automatically exported and parsed by Conan and we can read that information from the recipe.
This is handy for example to extract the URLs of the external sources repositories, zip files etc.
This is an example of ``conandata.yml``:

我们可以在  ``conanfile.py`` 的同一个文件夹中编写一个名为 ``conandata.yml``  的文件。这个文件将被 Conan 自动导出和解析，
我们可以从配方中读取这些信息。这很方便，例如提取外部源代码库、 zip 文件等的 URL。这是  ``conandata.yml`` 的一个例子:

.. code-block:: yaml

    sources:
      "1.0":
        url: "https://github.com/conan-io/libhello/archive/refs/heads/main.zip"
        sha256: "7bc71c682895758a996ccf33b70b91611f51252832b01ef3b4675371510ee466"
        strip_root: true
      "1.1":
        url: ...
        sha256: ...


The recipe doesn't need to be modified for each version of the code. We can pass all the ``keys`` of the specified version
(``url``, ``sha256``, and ``strip_root``) as arguments to the ``get`` function, that, in this case, allow us to verify that the downloaded
zip file has the correct ``sha256``. So we could modify the source method to this:

不需要为每个版本的代码修改配方。我们可以将指定版本(``url``, ``sha256``, 和 ``strip_root``)的所有 ``keys`` 作为参数传递给 ``get`` 函数，
在本例中，这些参数允许我们验证下载的 zip 文件是否具有正确的 ``sha256``。因此我们可以修改源代码方法:

.. code-block:: python

    def source(self):
        get(self, **self.conan_data["sources"][self.version])
        # Similar to:
        # data = self.conan_data["sources"][self.version]
        # get(self, data["url"], sha256=data["sha256"], strip_root=data["strip_root"])



Read more
---------

- :ref:`Patching sources<examples_tools_files_patches>`
- :ref:`Capturing Git SCM source information<examples_tools_scm_git_capture>` instead of copying sources with ``exports_sources``.
- ...

.. seealso::

    - :ref:`source() method reference<reference_conanfile_methods_source>`
