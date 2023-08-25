.. _other_important_features:

Other important Conan features
==============================

python_requires
---------------

It is possible to reuse code from other recipes using the :ref:`python_requires feature<reference_extensions_python_requires>`.

可以使用 :ref:`python_requires feature<reference_extensions_python_requires>` 重用来自其他配方的代码。

If you maintain many recipes for different packages that share some common logic and you don't want to repeat the code in every recipe, you can put that common code in a Conan ``conanfile.py``, upload it to your server, and have other recipe conanfiles do a ``python_requires = "mypythoncode/version"`` to depend on it and reuse it.

如果你为不同的包维护许多配方，这些配方共享一些共同的逻辑，你不想在每个配方中重复这些代码，你可以把这些代码放到 Conan ``conanfile.py`` 中，上传到你的服务器上，然后让其他的配方 conanfiles 做一个 ``python_requires = "mypythoncode/version"`` 来依赖它并重用它。

Packages lists
--------------

It is possible to manage a list of packages, recipes and binaries together with the "packages-list" feature. 
Several commands like ``upload``, ``download``, and ``remove`` allow receiving a list of packages file as an input, and they can do their operations over that list.
A typical use case is to "upload to the server the packages that have been built in the last ``conan create``", which can be done with:

可以通过"packages-list"特性管理包、配方和二进制文件的列表。像 ``upload``, ``download``, 和 ``remove`` 这样的命令允许接收包文件列表作为输入，
并且它们可以对该列表执行操作。一个典型的用例是“将在上一个 ``conan create`` 中构建的包上传到服务器”，这可以通过以下方法完成:

.. code:: bash

    $ conan create . --format=json > build.json
    $ conan list --graph=build.json --graph-binaries=build --format=json > pkglist.json
    $ conan upload --list=pkglist.json -r=myremote -c

See the :ref:`examples in this section<examples_commands_pkglists>`.
