.. _tutorial_versioning_conflicts:

Dependencies conflicts
======================

In a dependency graph, when different packages depends on different versions of the
same package, this is called a dependency version conflict. It is relatively easy
to produce one. Let's see it with a practical example, start cloning 
the `examples2.0 repository <https://github.com/conan-io/examples2>`_:

在依赖关系图中，当不同的包依赖于同一包的不同版本时，这称为依赖版本冲突。生产一个相对容易。
让我们看一个实际的例子，开始克隆 `examples2.0 repository <https://github.com/conan-io/examples2>`_:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/versioning/conflicts/versions

In this folder we have a small project, consisting in several packages: ``matrix`` (a math library),
``engine/1.0`` video game engine that depends on ``matrix/1.0``, ``intro/1.0``, a package implementing
the intro credits and functionality for the videogame that depends on ``matrix/1.1`` and finally the
``game`` recipe that depends simultaneously on ``engine/1.0`` and ``intro/1.0``. All these packages
are actually empty, but they are enough to produce the conflicts.

在这个文件夹中，我们有一个小项目，由几个包组成: ``matrix`` (数学库) ， ``engine/1.0`` 视频游戏引擎，依赖于 ``matrix/1.0``, 
``intro/1.0``，为电子游戏提供游戏简介和功能的程序包，依赖于 ``matrix/1.1``，最后是 ``game`` 配方，同时依赖于 ``engine/1.0`` 
和 ``intro/1.0``。 所有这些包实际上都是空的，但它们足以产生冲突。

.. graphviz::
    :align: center

    digraph conflict {
        node [fillcolor="lightskyblue", style=filled, shape=box]
        rankdir="BT"
        "game/1.0" -> "engine/1.0" -> "matrix/1.0";
        "game/1.0" -> "intro/1.0" -> "matrix/1.1";
        "matrix/1.0" [fillcolor="orange"];
        "matrix/1.1" [fillcolor="orange"];
    }

|

Let's create the dependencies:

.. code-block:: bash
    
    $ conan create matrix --version=1.0
    $ conan create matrix --version=1.1  # note this is 1.1!
    $ conan create engine --version=1.0 # depends on matrix/1.0
    $ conan create intro --version=1.0 # depends on matrix/1.1

And when we try to install ``game``, we will get the error:

当我们尝试安装 ``game`` 时，我们会得到错误:

.. code-block:: bash
    
    $ conan install game
    Requirements
        engine/1.0#0fe4e6890766f7b8e21f764f0049aec7 - Cache
        intro/1.0#d639998c2e55cf36d261ab319801c322 - Cache
        matrix/1.0#905c3f0babc520684c84127378fefdd0 - Cache
    Graph error
        Version conflict: intro/1.0->matrix/1.1, game/1.0->matrix/1.0.
    ERROR: Version conflict: intro/1.0->matrix/1.1, game/1.0->matrix/1.0.

This is a version conflict, and Conan will not decide automatically how to
resolve the conflict, but the user should explicitly resolve such conflict.

这是一个版本冲突，Conan 不会自动决定如何解决这个冲突，但是用户应该明确地解决这个冲突。


Resolving conflicts
-------------------

Of course, the most direct and straightforward way to solve such a conflict is
going to the dependencies ``conanfile.py`` and upgrading their ``requirements()``
so they point now to the same version. However this might not be practical in
some cases, or it might be even impossible to fix the dependencies conanfiles. 

当然，解决这种冲突的最直接和最简单的方法是访问依赖关系 ``conanfile.py`` 并升级它们的 ``requirements()`` ，
因此它们现在指向相同的版本。然而，在某些情况下，这可能不实际，或者甚至不可能修复依赖项 conanfiles。

For that case, it should be the consuming ``conanfile.py`` the one that can resolve
the conflict (in this case, ``game``) by explicitly defining which version of the
dependency should be used, with the following syntax:

对于这种情况，应该是消费 ``conanfile.py`` ，它可以通过显式定义应该使用哪个版本的依赖项来解决冲突
(在本例中是 ``game``) ，其语法如下:

.. code-block:: python
    :caption: game/conanfile.py
    :emphasize-lines: 8

    class Game(ConanFile):
        name = "game"
        version = "1.0"
        
        def requirements(self):
            self.requires("engine/1.0")
            self.requires("intro/1.0")
            self.requires("matrix/1.1", override=True)

This is called an ``override``. The ``game`` package do not directly depend on ``matrix``, this
``requires`` declaration will not introduce such a a direct dependency. But the ``matrix/1.1``
version will be propagated upstream in the dependency graph, overriding the ``requires`` of
packages that do depend on any ``matrix`` version, forcing the consistency of the graph, as all
upstream packages will now depend on ``matrix/1.1``:

这叫做 ``override``。 ``game`` 包不直接依赖 ``matrix``， ``requires`` 声明不会引入这样的直接依赖关系。
但是， ``matrix/1.1`` 版本将在依赖关系图的上游传播，覆盖那些确实依赖于任何 ``matrix`` 版本的包的 ``requires``，
从而强制图的一致性，因为所有上游包现在都将依赖 ``matrix/1.1``:

.. code-block:: bash

    $ conan install game
    ...
    Requirements
        engine/1.0#0fe4e6890766f7b8e21f764f0049aec7 - Cache
        intro/1.0#d639998c2e55cf36d261ab319801c322 - Cache
        matrix/1.1#905c3f0babc520684c84127378fefdd0 - Cache

.. graphviz::
    :align: center

    digraph conflict {
        node [fillcolor="lightskyblue", style=filled, shape=box]
        rankdir="BT"
        "game/1.0" -> "engine/1.0" -> "matrix/1.1";
        "game/1.0" -> "intro/1.0" -> "matrix/1.1";
        {
            rank = same;
            edge[ style=invis];
            "matrix/1.1" -> "matrix/1.0" ;
            rankdir = LR;
        }
    }

|

.. note::

    In this case, a new binary for ``engine/1.0`` was not necessary, but in some situations the above could
    fail with a ``engine/1.0`` "binary missing error". Because previously ``engine/1.0`` binaries were
    built against ``matrix/1.0``. If the ``package_id`` rules and configuration define that ``engine`` should
    be rebuilt when minor versions of the dependencies change, then it will be necessary to build a new
    binary for ``engine/1.0`` that builds and links against the new ``matrix/1.1`` dependency.

    在这种情况下，不需要为 ``engine/1.0`` 添加新的二进制文件，但是在某些情况下，上述操作可能会因为 ``engine/1.0``
    "二进制文件丢失错误"而失败。因为之前的 ``engine/1.0`` 二进制文件是根据 ``matrix/1.0`` 构建的。如果 ``package_id``
    规则和配置定义了当依赖项的次要版本发生变化时应该重新构建 ``engine``，那么就有必要为 ``engine/1.0`` 构建一个新的二进制文件，
    该文件构建并链接到新的 ``matrix/1.1`` 依赖项。


What happens if ``game`` had a direct dependency to ``matrix/1.2``? Lets create the version:

如果 ``game`` 直接依赖于 ``matrix/1.2`` 会发生什么? 让我们创建一个版本:

.. code-block:: bash
    
    $ conan create matrix --version=1.2

Now lets modify ``game/conanfile.py`` to introduce this as a direct dependency:

现在让我们修改 ``game/conanfile.py``，将其作为一个直接依赖项引入:

.. code-block:: python
    :caption: game/conanfile.py

    class Game(ConanFile):
        name = "game"
        version = "1.0"
        
        def requirements(self):
            self.requires("engine/1.0")
            self.requires("intro/1.0")
            self.requires("matrix/1.2")


.. graphviz::
    :align: center

    digraph conflict {
        node [fillcolor="lightskyblue", style=filled, shape=box]
        rankdir="BT"
        "game/1.0" -> "engine/1.0" -> "matrix/1.0";
        "game/1.0" -> "intro/1.0" -> "matrix/1.1";
        "game/1.0" -> "matrix/1.2";
        "matrix/1.0" [fillcolor="orange"];
        "matrix/1.1" [fillcolor="orange"];
        "matrix/1.2" [fillcolor="orange"];
        {
            rank = same;
            edge[ style=invis];
            "matrix/1.1" -> "matrix/1.2" ;
            rankdir = LR;
        }
    }

|

So intalling it will raise a conflict error again:

因此，安装它将再次引发一个冲突错误:

.. code-block:: bash

    $ conan install game
    ...
    ERROR: Version conflict: engine/1.0->matrix/1.0, game/1.0->matrix/1.2.

As this time, we want to respect the direct dependency between ``game`` and ``matrix``, we will
define the ``force=True`` requirement trait, to indicate that this dependency version will also
be forcing the overrides upstream:

这一次，我们想要尊重 ``game`` 和 ``matrix`` 之间的直接依赖关系，我们将定义 ``force=True`` 需求特征，
以表明这个依赖关系版本也将强制上游进行覆盖：

.. code-block:: python
    :caption: game/conanfile.py

    class Game(ConanFile):
        name = "game"
        version = "1.0"
        
        def requirements(self):
            self.requires("engine/1.0")
            self.requires("intro/1.0")
            self.requires("matrix/1.2", force=True)


And that will now solve again the conflict (as commented above, note that in real applications this could mean that binaries
for ``engine/1.0`` and ``intro/1.0`` would be missing, and need to be built to link against the new forced
``matrix/1.2`` version):

现在这将再次解决冲突(如上所述，注意在实际的应用程序中，这可能意味着 ``engine/1.0`` 和 ``intro/1.0``  的二进制文件将丢失，
并且需要建立链接到新的强制 ``matrix/1.2`` 版本) :

.. code-block:: bash

    $ conan install game
    Requirements
        engine/1.0#0fe4e6890766f7b8e21f764f0049aec7 - Cache
        intro/1.0#d639998c2e55cf36d261ab319801c322 - Cache
        matrix/1.2#905c3f0babc520684c84127378fefdd0 - Cache

.. graphviz::
    :align: center

    digraph conflict {
        node [fillcolor="lightskyblue", style=filled, shape=box]
        rankdir="BT"
        "game/1.0" -> "engine/1.0" -> "matrix/1.2";
        "game/1.0" -> "intro/1.0" -> "matrix/1.2";
        "game/1.0" -> "matrix/1.2";
        {
            rank = same;
            edge[ style=invis];
            "matrix/1.2" -> "matrix/1.0" -> "matrix/1.1" ;
            rankdir = LR;
        }
    }

|

.. note::

    **Best practices**

    Resolving version conflicts by overrides/forces should in general be the exception and avoided when possible,
    applied as a temporary workaround. The real solution is to move forward the dependencies ``requires`` so
    they naturally converge to the same versions of upstream dependencies.


    通过重写/强制解决版本冲突通常应该是例外情况，并尽可能避免，作为临时解决方案应用。
    真正的解决方案是向前推进所需的依赖关系，使它们自然地聚合到相同版本的上游依赖关系。



Overriding options
------------------

It is possible that when there are diamond structures in a dependency graph, like the one seen above, different
recipes might be defining different values for the upstream ``options``. In this case, this is not directly 
causing a conflict, but instead the first value to be defined is the one that will be prioritized and will
prevail.

当依赖关系图中有菱形结构时，如上图所示，不同的配方可能会为上游 ``options`` 定义不同的值。在这种情况下，这并不会直接导致冲突，
相反，要定义的第一个值是将被优先排序并占优势的值。

In the above example, if ``matrix/1.0`` can be both a static and a shared library, and ``engine`` decides to
define that it should be a static library (not really necessary, because that is already the default):

在上面的例子中，如果 ``matrix/1.0`` 既可以是一个静态库，又可以是一个共享库，而 ``engine`` 决定定义它应该是一个静态库
(不是真的必要，因为这已经是默认的了) :

.. code-block:: python
    :caption: engine/conanfile.py
    
    class Engine(ConanFile):
        name = "engine"
        version = "1.0"
        # Not strictly necessary because this is already the matrix default
        default_options = {"matrix*:shared": False}

And also ``intro`` recipe would do the same, but instead define that it wants a shared library, and adds a
``validate()`` method, because for some reason the ``intro`` package can only be built against shared libraries
and otherwise crashes:

同样， ``intro`` 配方也会做同样的事情，但是它会定义它想要一个共享库，并添加一个 ``validate()`` 方法，
因为出于某种原因， ``intro`` 包只能针对共享库构建，否则就会崩溃:

.. code-block:: python
    :caption: intro/conanfile.py

    class Intro(ConanFile):
        name = "intro"
        version = "1.0"
        default_options = {"matrix*:shared": True}

        def requirements(self):
            self.requires("matrix/1.0")

        def validate(self):
            if not self.dependencies["matrix"].options.shared:
                raise ConanInvalidConfiguration("Intro package doesn't work with static matrix library")

Then, this will cause an error, because as the first one to define the option value is ``engine`` (it is 
declared first in the ``game`` conanfile ``requirements()`` method).
In the examples2 repository, go to the "options" folder, and create the different packages:

然后，这将导致一个错误，因为第一个定义选项值的是 ``engine`` (在 ``game`` conanfile ``requirements()`` 方法中首先声明它)。
在 examples2 存储库中，转到 "options" 文件夹，并创建不同的包:

.. code-block:: text

    $ cd ../options
    $ conan create matrix
    $ conan create matrix -o matrix/*:shared=True
    $ conan create engine
    $ conan create intro
    $ conan install game  # FAILS!
    ...
    -------- Installing (downloading, building) binaries... --------
    ERROR: There are invalid packages (packages that cannot exist for this configuration):
    intro/1.0: Invalid: Intro package doesn't work with static matrix library


Following the same principle, the downstream consumer recipe, in this case ``game`` conanfile.py
can define the options values, and those will be prioritized:

遵循同样的原则，下游消费者配方，在这里是 ``game`` conanfile.py，可以定义选项值，这些选项值将被优先排序:

.. code-block:: python
    :caption: game/conanfile.py

    class Game(ConanFile):
        name = "game"
        version = "1.0"
        default_options = {"matrix*:shared": True}
        
        def requirements(self):
            self.requires("engine/1.0")
            self.requires("intro/1.0")


And that will force now ``matrix`` being a shared library, no matter if ``engine`` defined ``shared=False``,
because the downstream consumers always have priority over the upstream dependencies.

这将迫使现在的 ``matrix`` 成为一个共享库，无论 ``engine`` 是否定义了 ``shared=False``，因为下游消费者总是优先于上游依赖关系。

.. code-block:: bash

    $ conan install game 
    ...
    -------- Installing (downloading, building) binaries... --------
    matrix/1.0: Already installed!
    matrix/1.0: I am a shared-library library!!!
    engine/1.0: Already installed!
    intro/1.0: Already installed!

.. note::

    **Best practices**

    As a general rule, avoid modifying or defining values for dependencies ``options`` in consumers ``conanfile.py``.
    The declared ``options`` defaults should be good for the majority of cases, and variations from those defaults
    can be defined better in profiles better.

    作为一般规则，避免修改或定义使用者 ``conanfile.py`` 中的依赖项 ``options`` 的值。已声明的 ``options`` 缺省值对于大多数情况应该是有益的，
    并且可以在概要文件中更好地定义这些缺省值的变体。
