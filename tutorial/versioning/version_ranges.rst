.. _tutorial_versioning_version_ranges:

Version ranges
==============

In the previous section, we ended with several versions of the ``pkg`` package.
Let's remove them and create the following simple project:

在上一节中，我们以 ``pkg`` 包的几个版本结束。让我们移除它们并创建以下简单的项目:

.. code-block:: python
    :caption: pkg/conanfile.py

    from conan import ConanFile

    class pkgRecipe(ConanFile):
        name = "pkg"

.. code-block:: python
    :caption: app/conanfile.py

    from conan import ConanFile

    class appRecipe(ConanFile):
        name = "app"
        requires = "pkg/1.0"

Let's create ``pkg/1.0`` and install ``app``, to see it requires ``pkg/1.0``:

让我们创建 ``pkg/1.0`` 并安装 ``app``，看看它是否需要 ``pkg/1.0``:

.. code-block:: bash

    $ conan remove "pkg*" -c
    $ conan create pkg --version=1.0
    ... pkg/1.0 ...
    $ conan install app
    ...
    Requirements
        pkg/1.0

Then, if we create a new version of ``pkg/1.1``, it will not automatically be used by ``app``:

然后，如果我们创建一个新版本的 ``pkg/1.1``，它将不会自动被 ``app`` 使用:

.. code-block:: bash

    $ conan create pkg --version=1.1
    ... pkg/1.0 ...
    # Note how this still uses the previous 1.0 version
    $ conan install app
    ...
    Requirements
        pkg/1.0

So we could modify ``app`` conanfile to explictly use the new ``pkg/1.1`` version, but instead of that,
let's use the following version-range expression (introduced by the ``[expression]`` brackets):

因此，我们可以修改 ``app`` conanfile 来显式地使用新的 ``pkg/1.1`` 版本，
但是我们使用以下 version-range 表达式(由 ``[expression]`` 括号引入) :

.. code-block:: python
    :caption: app/conanfile.py

    from conan import ConanFile

    class appRecipe(ConanFile):
        name = "app"
        requires = "pkg/[>=1.0 <2.0]"

When we now install the dependencies of ``app``, it will automatically use the latest version in the
range, even if we create a new one, without needing to modify the ``app`` conanfile:

当我们现在安装 ``app`` 的依赖项时，它会自动使用范围内的最新版本，即使我们创建了一个新版本，也不需要修改 ``app`` 的 conanfile:

.. code-block:: bash

    # this will now use the newer 1.1
    $ conan install app
    ...
    Requirements
        pkg/1.1

    $ conan create pkg --version=1.2
    ... pkg/1.2 ...
    # Now it will automatically use the newest 1.2
    $ conan install app
    ...
    Requirements
        pkg/1.2

This holds as long as the newer version lies within the defined range, if we create a ``pkg/2.0`` version,
``app`` will not use it:

只要新版本在规定的范围内，这一点就成立，如果我们创建 ``pkg/2.0`` 版本， ``app`` 将不会使用它:

.. code-block:: bash
    
    $ conan create pkg --version=2.0
    ... pkg/2.0 ...
    # Conan will use the latest in the range
    $ conan install app
    ...
    Requirements
        pkg/1.2


Version ranges can be defined in several places:

版本范围可以在以下几个地方定义:

- In ``conanfile.py`` recipes ``requires``, ``tool_requires``, ``test_requires``, ``python_requires``

  在 ``conanfile.py`` 配方中 ``requires``, ``tool_requires``, ``test_requires``, ``python_requires``

- In ``conanfile.txt`` files in ``[requires]``, ``[tool_requires]``, ``[test_requires]`` sections

  在  ``conanfile.txt`` 文件中的 ``[requires]``, ``[tool_requires]``, ``[test_requires]`` 部分

- In command line arguments like ``--requires=`` and ``--tool_requires``.

  在命令行参数中，如 ``--requires=`` 和 ``--tool_requires`` 参数。

- In profiles ``[tool_requires]`` section

  在配置文件 ``[tool_requires]`` 部分


Semantic versioning
-------------------

The semantic versioning specification or `semver <https://semver.org/>`_, specifies that packages should
be versioned using always 3 dot-separated digits like ``MAJOR.MINOR.PATCH``, with very specific meanings for each digit.

语义版本规范或 `semver <https://semver.org/>`_ 指定应该使用像 ``MAJOR.MINOR.PATCH`` 总是以3个点分隔的数字对包进行版本控制，
每个数字都有非常具体的含义。

Conan extends the semver specification to any number of digits, and also allows to include letters in it.
This was done because during 1.X a lot of experience and feedback from users was gathered, and it became evident
than in C++ the versioning scheme is often more complex, and users were demanding more flexibility, allowing
versions like ``1.2.3.a.8`` if necessary.

Conan 将 semver 规范扩展到任意数字，并允许在其中包含字母。之所以这样做是因为在1.X 期间收集了大量的用户体验和反馈信息，
这一点比 C++ 更加明显，版本控制方案通常更加复杂，用户需要更大的灵活性，如果需要的话，允许使用 ``1.2.3.a.8`` 这样的版本。

The ordering of versions when necessary (for example to decide which is the latest version in a version range)
is done by comparing individually each dot-separated entity in the version, from left to right. Digits will be
compared numerically, so 2 < 11, and entries containing letters will be compared alphabetically (even if they
also contain some numbers).

必要时版本的排序(例如决定哪个是版本范围内的最新版本)是通过从左到右逐个比较版本中每个点分隔的实体来完成的。
数字将进行数字比较，因此2 < 11，包含字母的条目将按字母顺序进行比较(即使它们也包含一些数字)。

Similarly to the semver specification, Conan can manage **prereleases** and **builds** in the form: 
``VERSION-prerelease+build``.
Conan will also order pre-releases and builds according to the same rules, and each one of them can also
contain an arbitrary number of items, like ``1.2.3-pre.1.2.1+build.45.a``.
Note that the semver standard does not apply any ordering to builds, but Conan does, with the same logic that
is used to order the main version and the pre-releases.

与 semver 规范类似，Conan 可以以 ``VERSION-prerelease+build`` 的形式管理 **prereleases** 和 **builds**。
Conan 还将根据相同的规则对预发布和构建进行排序，每个规则还可以包含任意数量的条目，比如 ``1.2.3-pre.1.2.1+build.45.a``。
请注意，semver 标准不对构建应用任何排序，但 Conan 可以，其逻辑与用于对主版本和预发布版本进行排序的逻辑相同。


.. important::

    Note that the ordering of pre-releases can be confusing at times. A pre-release happens earlier in
    time than the release it is qualifying. So ``1.1-alpha.1`` is older than ``1.1``, not newer.

    请注意，预发布的顺序有时可能会令人困惑。预发布发生的时间早于它所限定的发布时间。所以 ``1.1-alpha.1`` 比 ``1.1`` 
    更老，不是更新。


Range expressions
-----------------

Range expressions can have comparison operators for the lower and higher bounds, separated with a space.
Also, lower bounds and upper bounds in isolation are permitted, though they are generally not recommended
under normal versioning schemes, specially the lower bound only. ``requires = "pkg/[>=1.0 <2.0]"`` will 
include versions like 1.0, 1.2.3 and 1.9, but will not include 0.3, 2.0 or 2.1 versions.

范围表达式可以具有用空格分隔的下界和上界的比较运算符。此外，虽然在正常的版本控制方案下通常不推荐使用下限和上限，
特别是仅允许使用下限，但是允许使用隔离的下限和上限。 ``requires = "pkg/[>=1.0 <2.0]"`` 将包含像1.0、1.2.3和1.9
这样的版本，但不包含0.3、2.0或2.1版本。


The tilde ``~`` operator can be used to define an "approximately" equal version range. ``requires = "pkg/[~1]"``
will include versions 1.3 and 1.8.1, but will exclude versions like 0.8 or 2.0. Likewise
``requires = "pkg/[~2.5]"`` will include 2.5.0 and 2.5.3, but exclude 2.1, 2.7, 2.8.

波浪 ``~`` 运算符可以用来定义一个“近似”相等的版本范围。 ``requires = "pkg/[~1]"`` 
将包含版本1.3和1.8.1，但不包含像0.8或2.0这样的版本。同样地， ``requires = "pkg/[~2.5]"`` 将包括2.5.0和2.5.3，
但不包括2.1、2.7和2.8。

The caret ``^`` operator is very similar to the tilde, but allowing variability over the last defined digit.
``requires = "pkg/[^1.2]"`` will include 1.2.1, 1.3 and 1.51, but will exclude 1.0, 2, 2.0.

插入 ``^`` 运算符与波浪线非常相似，但允许最后定义的数字的可变性。 ``requires = "pkg/[^1.2]"``  
将包括1.2.1、1.3和1.51，但不包括1.0、2、2.0。

It is also possible to apply multiple conditions with the OR operator, like ``requires = "pkg/[>1 <2.0 || ^3.2]"``
but this kind of complex expressions is not recommended in practice and should only be used in very extreme cases.

使用 OR 运算符也可以应用多个条件，比如 ``requires = "pkg/[>1 <2.0 || ^3.2]"``，但是在实际中不推荐使用这种复杂的表达式，
而且只应该在非常极端的情况下使用。

Finally, note that pre-releases are not resolved by default. The way to include them in the range is to
explicitly define it like: ``requires = "pkg/[>1- <2.0]"`` or more explicitly with 
``requires = "pkg/[>1 <2, include_prerelease=True]"``. This will include 1.5.1-pre1, but exclude 2.0-pre1.

最后，请注意，预发布在默认情况下是不解析的。将它们包含在范围内的方法是显式地定义它，比如 ``requires = "pkg/[>1- <2.0]"``  
或者更显式地使用 ``requires = "pkg/[>1 <2, include_prerelease=True]"``。这将包括1.5.1-pre1，但不包括2.0-pre1。

For more information about valid range expressions go to :ref:`Requires reference <version_ranges_reference>`

有关有效范围表达式的详细信息，请转到 :ref:`Requires reference <version_ranges_reference>`