.. _developing_packages:

Developing packages locally
===========================

As we learned in :ref:`previous sections <tutorial_creating_packages>` of the tutorial,
the most straightforward way to work when developing a Conan package is to run a
:command:`conan create`. This means that every time it is run, Conan performs a series of
costly operations in the Conan cache, such as downloading, decompressing, copying sources,
and building the entire library from scratch. Sometimes, especially with large libraries,
while we are developing the recipe, these operations cannot be performed every time.

正如我们在本教程的 :ref:`previous sections <tutorial_creating_packages>` 中所了解的，
在开发 Conan 包时，最直接的工作方式是运行一个 :command:`conan create`。这意味着每次运行 Conan 时，
Conan 都会在 Conan 缓存中执行一系列代价高昂的操作，比如下载、解压缩、复制源代码，以及从头构建整个库。
有时，特别是在大型库中，当我们开发配方时，不能每次都执行这些操作。

This section will first show the **Conan local development flow**, that is, working on
packages in your local project directory without having to export the contents of the
package to the Conan cache first.

本节将首先展示  **Conan local development flow**，即在本地项目目录中处理包，
而不必首先将包的内容导出到 Conan 缓存。

We will also cover how other packages can consume packages under development using the
**editable mode**.

我们还将介绍其他软件包如何使用 **editable mode** 使用正在开发的软件包。

Finally, we will explain the **Conan package layouts** in depth, the key feature that
makes it possible to work with Conan packages in the Conan cache or locally without making
any changes.

最后，我们将深入解释 **Conan package layouts**，这个关键特性使得在 Conan 缓存中或本地使用 Conan 
包而不做任何更改成为可能。

.. toctree::
   :maxdepth: 1

   developing_packages/local_package_development_flow
   developing_packages/editable_packages
   developing_packages/package_layout
