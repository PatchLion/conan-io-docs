Revisions
=========

This sections introduces how doing modifications to a given recipe or source code without explicitly
creating new versions, will still internally track those changes with a mechanism called revisions.

本节介绍如何在不显式创建新版本的情况下修改给定的配方或源代码，仍然可以通过一种称为修订的机制在内部跟踪这些更改。


Creating different revisions
----------------------------

Let's start with a basic "hello" package:

让我们从一个基本的 “hello” 包开始:

.. code-block:: bash

    $ mkdir hello && cd hello
    $ conan remove hello* -c # clean possible existing ones
    $ conan new cmake_lib -d name=hello -d version=1.0
    $ conan create .
    hello/1.0: Hello World Release!
    ...

We can now list the existing recipe revisions in the cache:

现在我们可以在缓存中列出现有的配方修订:

.. code-block:: bash

    $ conan list hello/1.0#*
    Local Cache
      hello
        hello/1.0
          revisions
            2475ece651f666f42c155623228c75d2 (2023-01-31 23:08:08 UTC)

If we now edit the ``src/hello.cpp`` file, to change the output message from
"Hello" to "Bye"

如果我们现在编辑 ``src/hello.cpp`` 文件，将输出消息从 “Hello” 更改为 “Bye”

.. code-block:: cpp
    :caption: hello/src/hello.cpp

    void hello(){
    
        #ifdef NDEBUG
        std::cout << "hello/1.0: Bye World Release!\n";
        ...

So if we create the package again, without changing the version ``hello/1.0``, we will 
get a new output:

因此，如果我们再次创建这个包，而不更改 ``hello/1.0`` 版本，我们将得到一个新的输出:

.. code-block:: bash

    $ conan create .
    hello/1.0: Bye World Release!
    ...

But even if the version is the same, internally a new revision ``2b547b7f20f5541c16d0b5cbcf207502`` 
has been created.

但是，即使版本相同，也会在内部创建一个新的修订版 ``2b547b7f20f5541c16d0b5cbcf207502``。

.. code-block:: bash
    
    $ conan list hello/1.0#*
    Local Cache
      hello
        hello/1.0
          revisions
            2475ece651f666f42c155623228c75d2 (2023-01-31 23:08:08 UTC)
            2b547b7f20f5541c16d0b5cbcf207502 (2023-01-31 23:08:25 UTC)

This recipe **revision**  is the hash of the contents of the recipe, including the ``conanfile.py``,
and the exported sources (``src/main.cpp``, ``CMakeLists.txt``, etc., that is, all files exported
in the recipe).

这个配方 **revision** 是配方内容(包括 ``conanfile.py``)和导出源(``src/main.cpp``、 ``CMakeLists.txt`` 等，即配方中导出的所有文件)的散列。

We can now edit the ``conanfile.py``, to define the ``licence`` value:

现在我们可以编辑 ``conanfile.py`` 来定义 ``licence`` 值:

.. code-block:: python
    :caption: hello/conanfile.py

    class helloRecipe(ConanFile):
        name = "hello"
        version = "1.0"

        # Optional metadata
        license = "MIT"
        ...


So if we create the package again, the output will be the same, but we will also get a new
revision, as the ``conanfile.py`` changed:

因此，如果我们再次创建包，输出将是相同的，但是我们也会得到一个新的修订，因为 ``conanfile.py`` 发生了变化:

.. code-block:: bash

    $ conan create .
    hello/1.0: Bye World Release!
    ...
    $ conan list hello/1.0#*
    Local Cache
      hello
        hello/1.0
          revisions
            2475ece651f666f42c155623228c75d2 (2023-01-31 23:08:08 UTC)
            2b547b7f20f5541c16d0b5cbcf207502 (2023-01-31 23:08:25 UTC)
            1d674b4349d2b1ea06aa6419f5f99dd9 (2023-01-31 23:08:34 UTC)


.. important::

    The recipe **revision** is the hash of the contents. It can be changed to be the
    Git commit hash with ``revision_mode = "scm"``. But in any case it is critical
    that every revision represents an immutable source, including the recipe and the source code:

    配方 **revision** 是内容的散列表。可以通过 ``revision_mode = "scm"`` 将其更改为 Git 提交散列。
    但在任何情况下，每个修订都代表一个不可变的源，包括配方和源代码，这一点至关重要:

    - If the sources are managed with ``exports_sources``, then they will be automatically
      be part of the hash

      如果使用 ``exports_sources`` 管理源，那么它们将自动成为散列的一部分

    - If the sources are retrieved from a external location, like a downloaded tarball or a git
      clone, that should guarantee uniqueness, by forcing the checkout of a unique
      immutable tag, or a commit. Moving targets like branch names or HEAD would be
      broken, as revisions are considered immutable.

      如果源是从外部位置(如下载的 tarball 或 git 克隆)检索的，
      那么应该通过强制签出唯一的不可变标记或提交来保证唯一性。
      像分支名称或 HEAD 这样的移动目标将被破坏，因为修订被认为是不可变的。
    
    Any change in source code or in recipe should always imply a new revision.

    源代码或配方中的任何更改应该总是意味着一个新的修订。


Using revisions
---------------

The recipe revisions are resolved by default to the latest revision for every
given version. In the case above, we could have a ``chat/1.0`` package that 
consumes the above ``hello/1.0`` package:

默认情况下，配方修订版被解析为每个给定版本的最新修订版。
在上面的例子中，我们可以有一个 ``chat/1.0`` 包，它使用上面的 ``hello/1.0`` 包:

.. code-block:: bash

    $ cd ..
    $ mkdir chat && cd chat
    $ conan new cmake_lib -d name=chat -d version=1.0 -d requires=hello/1.0
    $ conan create .
    ...
    Requirements
    chat/1.0#17b45a168519b8e0ed178d822b7ad8c8 - Cache
    hello/1.0#1d674b4349d2b1ea06aa6419f5f99dd9 - Cache
    ...
    hello/1.0: Bye World Release!
    chat/1.0: Hello World Release!

We can see that by default, it is resolving to the latest revision ``1d674b4349d2b1ea06aa6419f5f99dd9``,
so we also see the ``hello/1.0: Bye World`` modified message.

我们可以看到，默认情况下，它解析为最新版本 ``1d674b4349d2b1ea06aa6419f5f99dd9``，因此我们还可以看到 ``hello/1.0: Bye World`` 修改后的消息。

It is possible to explicitly depend on a given revision in the recipes, so it is possible
to modify the ``chat/1.0`` recipe to define it requires the first created revision:

可以明确地依赖配方中给定的修订，所以可以修改 ``chat/1.0`` 配方来定义它需要创建的第一个修订:


.. code-block:: python
    :caption: chat/conanfile.py

    def requirements(self):
        self.requires("hello/1.0#2475ece651f666f42c155623228c75d2")


So creating ``chat`` will now force the first revision:

因此，创建 ``chat`` 将迫使第一次修订:

.. code-block:: bash

    $ conan create .
    ...
    Requirements
    chat/1.0#12f87e1b8a881da6b19cc7f229e16c76 - Cache
    hello/1.0#2475ece651f666f42c155623228c75d2 - Cache
    ...
    hello/1.0: Hello World Release!
    chat/1.0: Hello World Release!


Uploading revisions
-------------------

The upload command will upload only the latest revision by default:

默认情况下，上传命令只会上传最新版本:

.. code-block:: bash

    # upload latest revision only, all package binaries
    $ conan upload hello/1.0 -c -r=myremote

If for some reason we want to upload all existing revisions, it is possible with:

如果出于某种原因，我们想上传所有现有的修订版本，可以使用:

.. code-block:: bash

    # upload all revisions, all binaries for each revision
    $ conan upload hello/1.0#* -c -r=myremote

In the server side, the latest uploaded revision becomes the latest one, and the
one that will be resolved by default. For this reason, the above command uploads
the different revisions in order (from older revision to latest revision), so the
relative order of revisions is respected in the server side.

在服务器端，最新上传的修订将成为最新的修订，默认情况下将解析该修订。出于这个原因，
上面的命令按顺序上传不同的修订(从较早的修订到最新的修订) ，因此服务器端尊重修订的相对顺序。

Note that if another machine decides to upload a revision that was created some time
ago, it will still become the latest in the server side, because it is created in the 
server side with that time.

请注意，如果另一台机器决定上传一段时间前创建的修订，它仍然会成为服务器端的最新版本，
因为它是在服务器端创建的。

Package revisions
-----------------
Package binaries when created also compute the hash of their contents, forming the
**package revision**.  But they are very different in nature to **recipe revisions**.
Recipe revisions are naturally expected, every change in source code or in the recipe
would cause a new recipe revision. But package binaries shouldn't have more than one 
**package revision**, because binaries variability would be already encoded in a unique
``package_id``. Put in other words, if the recipe revision is the same (exact same 
input recipe and source code) and the ``package_id`` is the same (exact same configuration
profile, settings, etc.), then that binary should be built only once.

创建包二进制文件时也计算其内容的散列，形成 **package revision**。但是它们在本质上与 **recipe revisions** 是非常不同的。
配方修订是自然而然的事情，源代码或配方中的每一个变化都会导致一个新的配方修订。
但是包二进制文件不应该有多个 **package revision**，因为二进制文件的可变性已经被编码到一个惟一的 ``package_id`` 中。
换句话说，如果配方修订版本是相同的(完全相同的输入配方和源代码) ， ``package_id`` 是相同的(完全相同的配置文件、设置等) ，
那么该二进制文件应该只构建一次。

As C and C++ build are not deterministic, it is possible that subsequents builds of the
same package, without modifying anything will be creating new package revisions:

由于 C 和 C++ 构建不是确定的，所以同一个包的后续构建可能会创建新的包修订版本，而不需要修改任何内容:

.. code-block:: bash

    # Build again 2 times the latest
    $ conan create .
    $ conan create .

In some OSs like Windows, this build will not be reproducible, and the resulting 
artifacts will have different checksums, resulting in new package revisions:

在一些像 Windows 这样的操作系统中，这种构建是不可重复的，
并且产生的工件会有不同的校验和，从而导致新的包修订:

.. code-block:: bash

    $ conan list hello/1.0:*#*
    Local Cache
      hello
        hello/1.0
          revisions
            1d674b4349d2b1ea06aa6419f5f99dd9 (2023-02-01 00:03:29 UTC)
              packages
                2401fa1d188d289bb25c37cfa3317e13e377a351
                  revisions
                    8b8c3deef5ef47a8009d4afaebfe952e (2023-01-31 23:08:40 UTC)
                    8e8d380347e6d067240c4c00132d42b1 (2023-02-01 00:03:12 UTC)
                    c347faaedc1e7e3282d3bfed31700019 (2023-02-01 00:03:35 UTC)
                  info
                    settings
                    arch: x86_64
                    build_type: Release
                    ...

By default, the package revision will also be resolved to the latest one. However, 
it is not possible to pin a package revision explicitly in recipes, recipes can 
only require down to the recipe revision as we defined above.

默认情况下，包修订版也将解析为最新版本。但是，不可能在配方中明确地固定一个包修订，配方只能按照我们上面定义的配方修订。

.. warning::

    **Best practices**

    Having more than 1 package revision for any given recipe revision + ``package_id``
    is a smell or a potential bad practice. It means that something was rebuilt when 
    it was not necessary, wasting computing and storage resources. There are ways to
    avoid doing it, like ``conan create . --build=missing:hello*`` will only build that
    package binary if it doesn't exist already (or running ``conan graph info`` can 
    also return information of what needs to be built.)

    对于任何给定的配方修订 +  ``package_id``，拥有一个以上的包修订都是一种不好的做法或潜在的坏习惯。
    这意味着在不需要的时候重新构建，浪费了计算和存储资源。有很多方法可以避免这样做，比如 
    ``conan create . --build=missing:hello*`` 只会在包二进制文件不存在的情况下构建它
    (或者运行 ``conan graph info`` 也可以返回需要构建的信息)
