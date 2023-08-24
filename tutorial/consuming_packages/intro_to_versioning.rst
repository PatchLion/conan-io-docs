.. _consuming_packages_intro_versioning:

Introduction to versioning
==========================

So far we have been using requires with fixed versions like ``requires = "zlib/1.2.12"``.
But sometimes dependencies evolve, new versions are released and consumers want to update to those versions as easy as possible.

到目前为止，我们一直在使用带有固定版本的 request，比如  ``requires = "zlib/1.2.12"``。
但有时依赖关系会发生变化，新版本会发布，消费者希望尽可能容易地更新到这些版本。

It is always possible to edit the ``conanfiles`` and explicitly update the versions to the new ones, but there are mechanisms in
Conan to allow such updates without even modifying the recipes.

总是可以编辑 ``conanfiles`` 并明确地更新到新的版本，但是在 Conan 有一些机制允许这样的更新，甚至不需要修改配方。

Version ranges
--------------

A ``requires`` can express a dependency to a certain range of versions for a given package, with the syntax ``pkgname/[version-range-expression]``.
Let's see an example, please, first clone the sources to recreate this project. You can find them in the
`examples2.0 repository <https://github.com/conan-io/examples2>`_ in GitHub:

``requires`` 可以用语法 ``pkgname/[version-range-expression]`` 表示对给定包的某个版本范围的依赖关系。
让我们看一个示例，首先克隆源代码来重新创建这个项目。
您可以在 GitHub 的  `examples2.0 repository <https://github.com/conan-io/examples2>`_  中找到它们:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/consuming_packages/versioning

We can see that we have there:

我们可以看到，我们有:

.. code-block:: python
    :caption: **conanfile.py**

    from conan import ConanFile


    class CompressorRecipe(ConanFile):
        settings = "os", "compiler", "build_type", "arch"
        generators = "CMakeToolchain", "CMakeDeps"

        def requirements(self):
            self.requires("zlib/[~1.2]")

That ``requires`` contains the expression ``zlib/[~1.2]``, which means "approximately" ``1.2`` version, that means, it can resolve to
any ``zlib/1.2.8``, ``zlib/1.2.11`` or ``zlib/1.2.12``, but it will not resolve to something like ``zlib/1.3.0``. Among the available
matching versions, a version range will always pick the latest one.

``requires`` 包含表达式  ``zlib/[~1.2]`` ，这意味着“大约” ``1.2`` 版本，这意味着，它可以解析为任何 
``zlib/1.2.8``, ``zlib/1.2.11`` 或 ``zlib/1.2.12``，但它不会解析为类似于 ``zlib/1.3.0`` 的内容。
在可用的匹配版本中，版本范围总是选择最新的版本。

If we do a :command:`conan install`, we would see something like:

如果我们做一个 :command:`conan install`，我们会看到这样的东西:

.. code-block:: bash

    $ conan install .

    Graph root
        conanfile.py: .../conanfile.py
    Requirements
        zlib/1.2.12#87a7211557b6690ef5bf7fc599dd8349 - Downloaded
    Resolved version ranges
        zlib/[~1.2]: zlib/1.2.12

If we tried instead to use ``zlib/[<1.2.12]``, that means that we would like to use a version lower than ``1.2.12``, but that one is excluded,
so the latest one to satisfy the range would be ``zlib/1.2.11``:

如果我们尝试使用 ``zlib/[<1.2.12]`` ，这意味着我们希望使用低于 ``1.2.12`` 的版本，但是这个版本被排除在外，
因此满足范围的最新版本应该是 ``zlib/1.2.11``:

.. code-block:: bash

    $ conan install .

    Resolved version ranges
        zlib/[<1.2.12]: zlib/1.2.11


The same applies to other type of requirements, like ``tool_requires``.
If we add now to the recipe:

这同样适用于其他类型的需求，比如 ``tool_requires``。如果我们现在加入配方:

.. code-block:: python
    :caption: **conanfile.py**

    from conan import ConanFile


    class CompressorRecipe(ConanFile):
        settings = "os", "compiler", "build_type", "arch"
        generators = "CMakeToolchain", "CMakeDeps"

        def requirements(self):
            self.requires("zlib/[~1.2]")
        
        def build_requirements(self):
            self.tool_requires("cmake/[>3.10]")


Then we would see it resolved to the latest available CMake package, with at least version ``3.11``:

然后我们会看到它被解析为最新的可用 CMake 包，至少是 ``3.11`` 版本:

.. code-block:: bash

    $ conan install .
    ...
    Graph root
        conanfile.py: .../conanfile.py
    Requirements
        zlib/1.2.12#87a7211557b6690ef5bf7fc599dd8349 - Cache
    Build requirements
        cmake/3.22.6#f305019023c2db74d1001c5afa5cf362 - Downloaded
    Resolved version ranges
        cmake/[>3.10]: cmake/3.22.6
        zlib/[~1.2]: zlib/1.2.12


Revisions
---------

What happens when a package creator does some change to the package recipe or to the source code, but they don't bump the ``version`` 
to reflect those changes? Conan has an internal mechanism to keep track of those modifications, and it is called the **revisions**.

如果包创建者对包配置或源代码做了一些更改，但他们没有改变 ``version`` 以反映这些更改，
会发生什么情况？Conan 有一个内部机制来跟踪这些修改，这就是所谓的  **revisions**。

The recipe revision is the hash that can be seen together with the package name and version in the form ``pkgname/version#recipe_revision``
or ``pkgname/version@user/channel#recipe_revision``.
The recipe revision is a hash of the contents of the recipe and the source code. So if something changes either in the recipe,
its associated files or in the source code that this recipe is packaging, it will create a new recipe revision.

配方修订是可以与包名和版本一起看到的hash，其格式为 ``pkgname/version#recipe_revision`` 或 ``pkgname/version@user/channel#recipe_revision``。
配方修订版是配置内容和源代码的hash表。因此，如果在配方、其相关文件或该配方打包的源代码中发生了变化，它将创建一个新的配方修订版。

You can list existing revisions with the :command:`conan list` command:

您可以使用 :command:`conan list` 命令列出现有的修订版本:

.. code-block:: bash

    $ conan list zlib/1.2.12#* -r=conancenter

    conancenter
      zlib
        zlib/1.2.12
          revisions
            82202701ea360c0863f1db5008067122 (2022-03-29 15:47:45 UTC)
            bd533fb124387a214816ab72c8d1df28 (2022-05-09 06:59:58 UTC)
            3b9e037ae1c615d045a06c67d88491ae (2022-05-13 13:55:39 UTC)
            ...


Revisions always resolve to the latest (chronological order of creation or upload to the server) revision.
Though it is not a common practice, it is possible to explicitly pin a given recipe revision directly in the ``conanfile``, like:

版本总是解析为最新版本(创建或上传到服务器的时间顺序)。虽然这不是一种常见的做法，但是可以将给定的配方修订直接固定在 ``conanfile`` 中，比如:

.. code-block:: python

    def requirements(self):
        self.requires("zlib/1.2.12#87a7211557b6690ef5bf7fc599dd8349")

This mechanism can however be tedious to maintain and update when new revisions are created, so probably in the general case, this
shouldn't be done.

然而，在创建新的修订版本时，这种机制对于维护和更新来说可能是乏味的，因此在一般情况下，不应该这样做。

.. _tutorial_consuming_packages_versioning_lockfiles:

Lockfiles
---------

The usage of version ranges, and the possibility of creating new revisions of a given package without bumping the version allows
to do automatic faster and more convenient updates, without need to edit recipes. 

版本范围的使用，以及在不影响版本的情况下创建给定软件包的新修订的可能性，允许自动更快和更方便的更新，而不需要编辑配方。

But in some occassions, there is also a need to provide an immutable and reproducible set of dependencies. This process is known
as "locking", and the mechanism to allow it is "lockfile" files. A lockfile is a file that contains a fixed list of dependencies,
specifying the exact version and exact revision. So, for example, a lockfile will never contain a version range with an expression,
but only pinned dependencies. 

但是在某些情况下，还需要提供一组不可变的、可重现的依赖项。这个过程称为“锁定”，允许它的机制是“lockfile”文件。
lockfile是一个文件，它包含一个固定的依赖项列表，指定确切的版本和确切的修订。因此，
例如，lockfile将永远不会包含带有表达式的版本范围，而只包含固定的依赖项。

A lockfile can be seen as a snapshot of a given dependency graph at some point in time.
Such snapshot must be "realizable", that is, it needs to be a state that can be actually reproduced from the conanfile recipes.
And this lockfile can be used at a later point in time to force that same state, even if there are new created package versions.

lockfile可以看作是给定依赖关系图在某个时间点的快照。这样的快照必须是“可实现的”，也就是说，
它需要是一个可以从 conanfile 配方实际重现的状态。即使有新创建的软件包版本，也可以在以后使用这个lockfile来强制执行相同的状态。

Let's see lockfiles in action. First, let's pin the dependency to ``zlib/1.2.11`` in our example:

让我们来看一下lockfiles的运行情况。首先，在我们的示例中，让我们将依赖关系固定到 zlib/1.2.11:

.. code-block:: python

    def requirements(self):
        self.requires("zlib/1.2.11")

And let's capture a lockfile:

我们来捕捉一个lockfile:

.. code-block:: bash

    conan lock create .

    -------- Computing dependency graph ----------
    Graph root
        conanfile.py: .../conanfile.py
    Requirements
        zlib/1.2.11#4524fcdd41f33e8df88ece6e755a5dcc - Cache

    Generated lockfile: .../conan.lock

Let's see what the lockfile ``conan.lock`` contains:

让我们看看lockfile ``conan.lock`` 包含了什么:

.. code-block:: json

    {
        "version": "0.5",
        "requires": [
            "zlib/1.2.11#4524fcdd41f33e8df88ece6e755a5dcc%1650538915.154"
        ],
        "build_requires": [],
        "python_requires": []
    }

Now, let's restore the original ``requires`` version range:

现在，让我们恢复原始的 ``requires`` 版本范围:

.. code-block:: python

    def requirements(self):
        self.requires("zlib/[~1.2]")


And run :command:`conan install .`, which by default will find the ``conan.lock``, and run the equivalent :command:`conan install . --lockfile=conan.lock`

然后运行  :command:`conan install .`，默认情况下会找到 ``conan.lock``，并运行等效的 :command:`conan install . --lockfile=conan.lock`

.. code-block:: bash

    conan install .

    Graph root
        conanfile.py: .../conanfile.py
    Requirements
        zlib/1.2.11#4524fcdd41f33e8df88ece6e755a5dcc - Cache


Note how the version range is no longer resolved, and it doesn't get the ``zlib/1.2.12`` dependency, even if it is the 
allowed range ``zlib/[~1.2]``, because the ``conan.lock`` lockfile is forcing it to stay in ``zlib/1.2.11`` and that exact revision too.

请注意，版本范围不再被解析，并且它不能获得 ``zlib/1.2.12`` 依赖项，即使它是允许的范围  ``zlib/[~1.2]``，因为 ``conan.lock``  
文件强制它保留在 ``zlib/1.2.11`` 中，并且也是精确的修订版本。

Read more
---------

- :ref:`Introduction to Versioning<tutorial_versioning>`

