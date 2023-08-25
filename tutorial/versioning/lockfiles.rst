.. _tutorial_versioning_lockfiles:

Lockfiles
=========

Lockfiles are a mechanism to achieve reproducible dependencies, even when new versions or revisions
of those dependencies are created.
Let's see it with a practical example, start cloning  the `examples2.0 repository <https://github.com/conan-io/examples2>`_:

Lockfiles 是一种实现可重复依赖性的机制，即使在创建这些依赖性的新版本或修订版时也是如此。让我们看一个实际的例子，开始克隆 
`examples2.0 repository <https://github.com/conan-io/examples2>`_:

.. code-block:: bash

    $ git clone https://github.com/conan-io/examples2.git
    $ cd examples2/tutorial/versioning/lockfiles/intro

In this folder we have a small project, consisting in 3 packages: a ``matrix`` package, emulating some mathematical
library, an ``engine`` package emulating some game engine, and a ``sound32`` package, emulating a sound library for 
some 32bits systems. These packages are actually most empty, they do not build any code, but they are good to learn
the concepts of lockfiles.

在这个文件夹中，我们有一个小项目，由3个包组成: 一个 ``matrix`` 包，模拟一些数学库，一个 ``engine`` 包模拟一些游戏引擎，
和一个 ``sound32`` 包，模拟一些32位系统的声音库。这些包实际上是最空的，它们不构建任何代码，但是它们有助于学习lockfile的概念。

.. graphviz::
    :align: center

    digraph lockfiles {
        node [fillcolor="lightskyblue", style=filled, shape=box]
        rankdir="BT"
        "engine/1.0" -> "matrix/1.0";
        "engine/1.0" -> "sound32/1.0" [label="if arch==x86"];
    }

|

We will start by creating the first ``matrix/1.0`` version:

我们将从创建第一个 ``matrix/1.0`` 版本开始:

.. code-block:: bash

    $ conan create matrix --version=1.0

Now we can check in the ``engine`` folder its recipe:

现在我们可以在 ``engine`` 文件夹中查看它的配方:

.. code-block:: python

    class Engine(ConanFile):
        name = "engine"
        settings = "arch"

        def requirements(self):
            self.requires("matrix/[>=1.0 <2.0]")
            if self.settings.arch == "x86":
                self.requires("sound32/[>=1.0 <2.0]")

Lets move to the ``engine`` folder and install its dependencies:

让我们移动到 ``engine`` 文件夹并安装其依赖项:

.. code-block:: bash

    $ cd engine
    $ conan install .
    ... 
    Requirements
        matrix/1.0#905c3f0babc520684c84127378fefdd0 - Cache
    Resolved version ranges
        matrix/[>=1.0 <2.0]: matrix/1.0

As the ``matrix/1.0`` version is in the valid range, it is resolved and used.
But if someone creates a new ``matrix/1.1`` or ``1.X`` version, it would also be automatically used, because
it is also in the valid range. To avoid this, we will capture a "snapshot" of the current dependencies
creating a ``conan.lock`` lockfile:

由于 ``matrix/1.0`` 版本在有效范围内，因此可以解析和使用它。但是，如果有人创建了一个新的 ``matrix/1.1`` 或 ``1.X`` 版本，
它也将被自动使用，因为它也在有效范围内。为了避免这种情况，我们将捕获创建 ``conan.lock`` lockfile的当前依赖项的“快照”:

.. code-block:: bash

    $ conan lock create .
    $ cat conan.lock
    {
        "version": "0.5",
        "requires": [
            "matrix/1.0#905c3f0babc520684c84127378fefdd0%1675278126.0552447"
        ],
        "build_requires": [],
        "python_requires": []
    }

We can see how the created ``conan.lock`` lockfile contains the ``matrix/1.0`` version
and its revision. But ``sound32/1.0`` is not in the lockfile, because for the default
configuration profile (not ``x86``), this ``sound32`` is not a dependency.

我们可以看到创建的 ``conan.lock`` 文件是如何包含 ``matrix/1.0`` 版本及其修订版的。
但 ``sound32/1.0`` 不在lockfile中，因为对于默认配置概要文件(而不是 ``x86``) ， ``sound32`` 不是一个依赖项。

Now, a new ``matrix/1.1`` version is created:

.. code-block:: bash

    $ cd ..
    $ conan create matrix --version=1.1
    $ cd engine

And see what happens when we issue a new ``conan install`` command for the engine:

然后看看当我们为引擎发出一个新的 ``conan install`` 命令时会发生什么:

.. code-block:: bash

    $ conan install .
    # equivalent to conan install . --lockfile=conan.lock 
    ...
    Requirements
       matrix/1.0#905c3f0babc520684c84127378fefdd0 - Cache

As we can see, the new ``matrix/1.1`` was not used, even if it is in the valid range!
This happens because by default the ``--lockfile=conan.lock`` will be used if the
``conan.lock`` file is found. The locked ``matrix/1.0`` version and revision will be
used to resolve the range, and the ``matrix/1.1`` will be ignored.

正如我们所看到的，新的 ``matrix/1.1`` 没有被使用，即使它在有效范围内！这是因为在默认情况下，
如果找到了 ``conan.lock`` 文件，将使用 ``--lockfile=conan.lock``。锁定 ``matrix/1.0`` 版本和
修订版本将用于解决范围，而 ``matrix/1.1`` 将被忽略。

Likewise, it is possible to issue other Conan commands, and if the ``conan.lock`` is there,
it will be used:

同样，也可以发出其他 Conan 命令，如果存在 ``conan.lock`` ，将使用它:

.. code-block:: bash

    $ conan graph info . --filter=requires # --lockfile=conan.lock is implicit
    # display info for matrix/1.0
    $ conan create . --version=1.0 # --lockfile=conan.lock is implicit
    # creates the engine/1.0 package, using matrix/1.0 as dependency
    
If using a lockfile is intended, like in CI, it is better that the argument ``--lockfile=conan.lock`` explicit.

如果打算使用lockfile，比如在 CI 中，那么最好显式地使用参数 ``--lockfile=conan.lock``。

Multi-configuration lockfiles
-----------------------------

We saw above that the ``engine`` has a conditional dependency to the ``sound32`` package, in case the architecture
is ``x86``. That also means that such ``sound32`` package version was not captured in the above lockfile.

我们在上面看到，如果体系结构是 ``x86``，那么  ``engine``  对 ``sound32`` 包有条件依赖关系。
这也意味着在上面的lockfile中没有捕获这样的 ``sound32`` 包版本。

Lets create the ``sound32/1.0`` package first, then try to install ``engine``:

让我们先创建 ``sound32/1.0`` 包，然后尝试安装  ``engine``:

.. code-block:: bash

    $ cd ..
    $ conan create sound32 --version=1.0
    $ cd engine
    $ conan install . -s arch=x86 # FAILS!
    ERROR: Requirement 'sound32/[>=1.0 <2.0]' not in lockfile

This happens because the ``conan.lock`` lockfile doesn't contain a locked version for ``sound32``. By default
lockfiles are strict, if we are locking dependencies, a matching version inside the lockfile must be found.
We can relax this assumption with the ``--lockfile-partial`` argument:

发生这种情况是因为 ``conan.lock`` lockfile不包含 ``sound32`` 的锁定版本。默认情况下，lockfile是严格的，如果我们要锁定依赖项，
那么必须在lockfile中找到匹配的版本。我们可以用 ``--lockfile-partial`` 参数来放宽这个假设:

.. code-block:: bash

    $ conan install . -s arch=x86 --lockfile-partial
    ...
    Requirements
        matrix/1.0#905c3f0babc520684c84127378fefdd0 - Cache
        sound32/1.0#83d4b7bf607b3b60a6546f8b58b5cdd7 - Cache
    Resolved version ranges
        sound32/[>=1.0 <2.0]: sound32/1.0

This will manage to partially lock to ``matrix/1.0``, and resolve ``sound32`` version range as usual.
But we can do better, we can extend our lockfile to also lock ``sound32/1.0`` version, to avoid
possible disruptions caused by new ``sound32`` unexpected versions:

这将设法部分锁定到 ``matrix/1.0``，并像往常一样解析 ``sound32`` 版本范围。但我们可以做得更好，
我们可以扩展我们的lockfile，也锁定 ``sound32/1.0`` 版本，以避免可能由新的 ``sound32`` 意想不到的版本造成的干扰:


.. code-block:: bash

    $ conan lock create . -s arch=x86
    $ cat conan.lock
    {                                                                         
        "version": "0.5",                                                     
        "requires": [                                                         
            "sound32/1.0#83d4b7bf607b3b60a6546f8b58b5cdd7%1675278904.0791488",
            "matrix/1.0#905c3f0babc520684c84127378fefdd0%1675278900.0103245"  
        ],                                                                    
        "build_requires": [],                                                 
        "python_requires": []                                                 
    }

Now, both ``matrix/1.0`` and ``sound32/1.0`` are locked inside our ``conan.lock`` lockfile.
It is possible to use this lockfile for both configurations (64bits, and x86 architectures),
having versions in a lockfile that are not used for a given configuration is not an issue,
as long as the necessary dependencies for that configuration find a matching version in it.

现在， ``matrix/1.0`` 和 ``sound32/1.0`` 都锁定在我们的 ``conan.lock`` 锁定文件中。
对于两种配置(64位和 x86体系结构)都可以使用这个lockfile，只要配置的必要依赖项在lockfile中找到匹配的版本，
那么在lockfile中使用不用于给定配置的版本就不是问题。

.. important::

    Lockfiles contains sorted lists of requirements, ordered by versions and revisions, so
    latest versions and revisions are the ones that are prioritized when resolving against a lockfile.
    A lockfile can contain two or more different versions of the same package, just because different
    version ranges require them. The sorting will provide the right logic so each range resolves to
    each valid versions.

    Lockfiles 包含按版本和修订排序的requirements列表，因此最新版本和修订是根据lockfile进行解析时优先考虑的。
    一个lockfile可以包含同一个包的两个或多个不同版本，只是因为不同的版本范围需要它们。排序将提供正确的逻辑，
    以便每个范围解析为每个有效的版本。
    
    If a version in the lockfile doesn't fit in a valid range, it will not be used. It is not possible
    for lockfiles to force a dependency that goes against what ``conanfile`` requires define, as they 
    are "snapshots" of an existing/realizable dependency graph, but cannot define an "impossible" 
    dependency graph.

    如果lockfile中的某个版本不在有效范围内，则不会使用该版本。lockfile不可能强制依赖于 ``conanfile`` 需要定义的内容，
    因为它们是现有的/可实现的依赖关系图的“快照”，但不能定义“不可能的”依赖关系图。


Evolving lockfiles
------------------

Even if lockfiles enforce and constraint the versions that can be resolved for a graph, it doesn't
mean that lockfiles cannot evolve. Actually, controlled evolution of lockfiles is paramount to
important processes like Continuous Integration, when the effect of one change in the graph wants
to be tested in isolation of other possible concurrent changes.

即使lockfiles强制和限制了可以为图形解析的版本，这并不意味着lockfiles不能进化。实际上，
lockfiles的可控进化对于像持续集成这样的重要过程来说是至关重要的，
因为图中一个变化的影响需要在与其他可能的并发变化隔离的情况下进行测试。

In this section we will introduce some of the basic functionality of lockfiles that allows such
evolution.

在本节中，我们将介绍允许这种演变的lockfiles的一些基本功能。

First, if we would like now to introduce and test the new ``matrix/1.1`` version in our ``engine``, 
without necessarily pulling many other dependencies that could have got new versions too, we could
manually add ``matrix/1.1`` to the lockfile:

首先，如果我们现在想在我们的 ``engine`` 中引入和测试新的  ``matrix/1.1`` 版本，而不需要拉动许多其他可能已经得到新版本的依赖项，
我们可以手动将 ``matrix/1.1`` 添加到lockfile:

.. code-block:: bash

    $ Running: conan lock add --requires=matrix/1.1                              
    $ cat conan.lock  
    {                                                                   
        "version": "0.5",                                                      
        "requires": [                                                          
            "sound32/1.0#83d4b7bf607b3b60a6546f8b58b5cdd7%1675278904.0791488", 
            "matrix/1.1",                                                      
            "matrix/1.0#905c3f0babc520684c84127378fefdd0%1675278900.0103245"   
        ],                                                                     
        "build_requires": [],                                                  
        "python_requires": []                                                  
    }

To be clear: manually adding with ``conan lock add`` is not necessarily a recommended flow, it is
possible to automate the task with other approaches, that will be explained later. This is just
an introduction to the principles and concepts.

需要说明的是: 使用 ``conan lock add`` 手动添加不一定是推荐的流程，
可以使用其他方法自动完成任务，稍后将对此进行解释。这只是一个原则和概念的介绍。

The important idea is that now we got 2 versions of ``matrix`` in the lockfile, and ``matrix/1.1``
is before ``matrix/1.0``, so for the range ``matrix/[>=1.0 <2.0]``, the first one (``matrix/1.1``)
would be prioritized. That means that when now the new lockfile is used, it will resolve to
``matrix/1.1`` version (even if a ``matrix/1.2`` or higher version existed in the system):

重要的是，现在我们在lockfile中得到了 ``matrix`` 的两个版本， ``matrix/1.1`` 在  ``matrix/1.0`` 之前，
所以对于范围 ``matrix/[>=1.0 <2.0]``，第一个(``matrix/1.1``)将被优先考虑。这意味着当现在使用新的lockfile时，
它将解析为 ``matrix/1.1`` 版本(即使系统中存在 ``matrix/1.1`` 或更高版本) :

.. code-block:: bash

    $ conan install . -s arch=x86 --lockfile-out=conan.lock
    Requirements
        matrix/1.1#905c3f0babc520684c84127378fefdd0 - Cache
        sound32/1.0#83d4b7bf607b3b60a6546f8b58b5cdd7 - Cache
    $ cat conan.lock
    {                                                                   
        "version": "0.5",                                                       
        "requires": [                                                           
            "sound32/1.0#83d4b7bf607b3b60a6546f8b58b5cdd7%1675278904.0791488",  
            "matrix/1.1#905c3f0babc520684c84127378fefdd0%1675278901.7527816",   
            "matrix/1.0#905c3f0babc520684c84127378fefdd0%1675278900.0103245"    
        ],                                                                      
        "build_requires": [],                                                   
        "python_requires": []                                                   
    }

Note that now ``matrix/1.1`` was resolved, and it also got its ``revision`` stored in
the lockfile (because ``--lockfile-out=conan.lock`` was passed as argument).

注意，现在已经解析了 ``matrix/1.1``，并且它的 ``revision`` 也存储在 lockfile 中
(因为 ``--lockfile-out=conan.lock`` 作为参数传递)。

It is true that the former ``matrix/1.0`` version was not used. As said above, having
old versions in the lockfile that are not used is not harmful. However, if we want to 
prune the unused versions and revisions, we could use the ``--lockfile-clean`` for that
purpose:

确实没有使用以前的 ``matrix/1.0`` 版本。如上所述，在lockfile中使用未使用的旧版本是无害的。
但是，如果我们想要删除未使用的版本和修订，我们可以使用 ``--lockfile-clean`` 来实现这个目的:

.. code-block:: bash

    $ conan install . -s arch=x86 --lockfile-out=conan.lock --lockfile-clean
    ...
    Requirements
        matrix/1.1#905c3f0babc520684c84127378fefdd0 - Cache
        sound32/1.0#83d4b7bf607b3b60a6546f8b58b5cdd7 - Cache
    ...
    $ cat conan.lock
    {
        "version": "0.5",
        "requires": [
            "sound32/1.0#83d4b7bf607b3b60a6546f8b58b5cdd7%1675278904.0791488",
            "matrix/1.1#905c3f0babc520684c84127378fefdd0%1675278901.7527816"
        ],
        "build_requires": [],
        "python_requires": []
    }

It is relevant to note that the ``-lockfile-clean`` could remove locked versions in
given configurations. For example, if instead of the above, the ``x86_64`` architecture
is used, the ``--lockfile-clean`` will prune the "unused" ``sound32``, because in that 
configuration is not used. It is possible to evaluate new lockfiles for every different
configuration, and then merge them:

需要注意的是 ``-lockfile-clean`` 可以删除给定配置中的锁定版本。例如，如果使用的是 ``x86_64`` 体系结构，
而不是上面的，那么 ``-lockfile-clean`` 将删除“未使用的” ``sound32``，因为在该配置中没有使用。
可以对每个不同的配置评估新的lockfile，然后合并它们:

.. code-block:: bash

    $ conan lock create . --lockfile-out=64.lock --lockfile-clean
    $ conan lock create . -s arch=x86 --lockfile-out=32.lock --lockfile-clean
    $ cat 64.lock
    {                                                                                                   
        "version": "0.5",                                                                               
        "requires": [                                                                                   
            "matrix/1.1#905c3f0babc520684c84127378fefdd0%1675294635.6049662"                            
        ],                                                                                              
        "build_requires": [],                                                                           
        "python_requires": []                                                                           
    }             
    $ cat 32.lock                                                                                      
    {                                                                                                   
        "version": "0.5",                                                                               
        "requires": [                                                                                   
            "sound32/1.0#83d4b7bf607b3b60a6546f8b58b5cdd7%1675294637.9775107",                          
            "matrix/1.1#905c3f0babc520684c84127378fefdd0%1675294635.6049662"                            
        ],                                                                                              
        "build_requires": [],                                                                           
        "python_requires": []                                                                           
    }               
    $ conan lock merge --lockfile=32.lock --lockfile=64.lock --lockfile-out=conan.lock         
    $ cat conan.lock                                                                               
    {                                                                                                   
        "version": "0.5",                                                                               
        "requires": [                                                                                   
            "sound32/1.0#83d4b7bf607b3b60a6546f8b58b5cdd7%1675294637.9775107",                          
            "matrix/1.1#905c3f0babc520684c84127378fefdd0%1675294635.6049662"                            
        ],                                                                                              
        "build_requires": [],                                                                           
        "python_requires": []                                                                           
    }                                                                                                   

This multiple-clean + merge operation is not something that developers should do, only CI
scripts, and for some advanced CI flows that will be explained later.

这种多重清理 + 合并操作不是开发人员应该做的事情，只有 CI 脚本，以及一些高级 CI 流程，后面将对其进行解释。

Read more
---------
- It is possible to lock down to package revisions, but this would be not recommended for
  most use cases, and should only be used in extreme and problematic cases.

  可以锁定到包的修订，但是不建议在大多数用例中使用，并且应该只在极端和有问题的情况下使用。

- Continuous Integrations links.

  持续集成链接。