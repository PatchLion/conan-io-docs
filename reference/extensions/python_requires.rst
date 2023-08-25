.. _reference_extensions_python_requires:

Python requires
===============


Introduction
------------

The ``python_requires`` feature is a very convenient way to share files and code between
different recipes. A python require is a special recipe that does not create packages and
it is just intended to be reused by other recipes.

``python_requires`` 特性是在不同配方之间共享文件和代码的一种非常方便的方法。python require的是
一个不创建包的特殊配方，它只是为了被其他配方重用。

A very simple recipe that we want to reuse could be:

我们希望重复使用的非常简单的配方可以是:

.. code-block:: python
    
    from conan import ConanFile

    myvar = 123

    def myfunct():
        return 234

    class Pkg(ConanFile):
        name = "pyreq"
        version = "0.1"
        package_type = "python-require"

     
And then we will make it available to other packages with ``conan create .``. Note that a ``python-require``
package does not create binaries, it is just the recipe part.

然后我们将使用 ``conan create .`` 将其提供给其他包。.注意， ``python-require`` 包不创建二进制文件，它只是配方部分。

.. code-block:: bash

    $ conan create .
    # It will only export the recipe, but will NOT create binaries
    # python-requires do NOT have binaries


We can reuse the above recipe functionality declaring the dependency in the ``python_requires``
attribute and we can access its members using ``self.python_requires["<name>"].module``:

我们可以重用上面的配方功能，在 ``python_requires`` 属性中声明依赖项，并且可以使用 
``self.python_requires["<name>"].module`` 访问它的成员:

.. code-block:: python
    
    from conan import ConanFile

    class Pkg(ConanFile):
        name = "pkg"
        version = "0.1"
        python_requires = "pyreq/0.1"

        def build(self):  
            v = self.python_requires["pyreq"].module.myvar  # v will be 123
            f = self.python_requires["pyreq"].module.myfunct()  # f will be 234
            self.output.info(f"{v}, {f}")


.. code-block:: bash

    $ conan create . 
    ...
    pkg/0.1: 123, 234


Python requires can also use version ranges, and this can be recommended in many cases if those ``python-requires``
need to evolve over time:

Python requires 也可以使用版本范围，在许多情况下，如果那些 ``python-requires`` 需要随着时间的推移而发展，可以建议这样做:

.. code-block:: python
    
    from conan import ConanFile

    class Pkg(ConanFile):
        python_requires = "pyreq/[>=1.0 <2]"


It is also possible to require more than 1 ``python-requires`` with ``python_requires = "pyreq/0.1", "other/1.2"``

也可能需要多于1个 ``python-requires`` 和 ``python_requires = "pyreq/0.1", "other/1.2"``

Extending base classes
----------------------

A common use case would be to declare a base class with methods we want to reuse in several
recipes via inheritance. We'd write this base class in a python-requires package: 

一个常见的用例是声明一个基类，其中包含我们希望通过继承在多个配方中重用的方法。我们将把这个基类写入一个 python-requires 包:

.. code-block:: python

    from conan import ConanFile

    class MyBase:
        def source(self):
            self.output.info("My cool source!")
        def build(self):
            self.output.info("My cool build!")
        def package(self):
            self.output.info("My cool package!")
        def package_info(self):
            self.output.info("My cool package_info!")

    class PyReq(ConanFile):
        name = "pyreq"
        version = "0.1"
        package_type = "python-require"


And make it available for reuse with:

并且可以通过以下方式重复使用:

.. code-block:: bash

    $ conan create .


Note that there are two classes in the recipe file:

注意，配方文件中有两个类:

 * ``MyBase`` is the one intended for inheritance and doesn't extend ``ConanFile``.

   ``MyBase`` 是用于继承的，不扩展 ``ConanFile``。

 * ``PyReq`` is the one that defines the current package being exported, it is the recipe
   for the reference ``pyreq/0.1``.

   ``PyReq`` 定义导出的当前包，它是引用 ``pyreq/0.1`` 的配方。


Once the package with the base class we want to reuse is available we can use it in other
recipes to inherit the functionality from that base class. We'd need to declare the
``python_requires`` as we did before and we'd need to tell Conan the base classes to use
in the attribute ``python_requires_extend``. Here our recipe will inherit from the
class ``MyBase``:

一旦包含我们想要重用的基类的包可用，我们就可以在其他配方中使用它来继承该基类的功能。
我们需要像前面一样声明  ``python_requires``，并且需要告诉 Conan 在属性 ``python_requires_extend`` 
中使用的基类。在这里，我们的配方将继承 ``MyBase`` 类:

.. code-block:: python
    
    from conan import ConanFile

    class Pkg(ConanFile):
        name = "pkg"
        version = "0.1"
        python_requires = "pyreq/0.1"
        python_requires_extend = "pyreq.MyBase"


The resulting inheritance is equivalent to declare our ``Pkg`` class as ``class Pkg(pyreq.MyBase, ConanFile)``.
So creating the package we can see how the methods from the base class are reused:

由此产生的的继承相当于将我们的  ``Pkg`` 类声明为类 ``class Pkg(pyreq.MyBase, ConanFile)``。
因此，创建这个包时，我们可以看到基类中的方法是如何被重用的:

.. code-block:: bash

    $ conan create .
    ...
    pkg/0.1: My cool source!
    pkg/0.1: My cool build!
    pkg/0.1: My cool package!
    pkg/0.1: My cool package_info!
    ...


In general, base class attributes are not inherited, and should be avoided as much as possible.
There are method alternatives to some of them like ``export()`` or ``set_version()``.
For exceptional situations, see the ``init()`` method documentation for more information to extend inherited attributes.

一般来说，基类属性不会被继承，应该尽可能地避免。有一些方法可以替代其中的一些方法，比如 
``export()`` 或 ``set_version()``。有关扩展继承属性的更多信息，请参见 ``init()`` 方法文档。

It is possible to re-implement some of the base class methods, and also to call the base class 
method explicitly, with the Python ``super()`` syntax:

可以重新实现一些基类方法，也可以使用 Python ``super()`` 语法显式调用基类方法:

.. code-block:: python
    
    from conan import ConanFile

    class Pkg(ConanFile):
        name = "pkg"
        version = "0.1"
        python_requires = "pyreq/0.1"
        python_requires_extend = "pyreq.MyBase"

        def source(self):
            super().source()  # call the base class method
            self.output.info("MY OWN SOURCE") # Your own implementation

It is not mandatory to call the base class method, a full overwrite without calling ``super()`` is possible. Also the call order can be changed, and calling your own code, then ``super()`` is possible.

调用基类方法不是强制性的，可以在不调用 ``super()`` 的情况下进行完全覆盖。此外，可以更改调用顺序，然后调用自己的代码，就可以使用 ``super()``。

Reusing files
-------------

It is possible to access the files exported by a recipe that is used with ``python_requires``.
We could have this recipe, together with a *myfile.txt* file containing the "Hello" text.

可以访问与 ``python_requires`` 一起使用的配方导出的文件。我们可以把这个配方和一个包含“Hello”文本的 *myfile.txt* 文件放在一起。

.. code-block:: python

    from conan import ConanFile

    class PyReq(ConanFile):
        name = "pyreq"
        version = "1.0"
        package_type = "python-require"
        exports = "*"


.. code-block:: bash

    $ echo "Hello" > myfile.txt
    $ conan create .


Now that the python-require has been created, we can access its path (the place where *myfile.txt* is) with the
``path`` attribute:

现在已经创建了 python-require，我们可以使用 ``path`` 属性访问它的路径(*myfile.txt* 所在的位置) :

.. code-block:: python

    import os

    from conan import ConanFile
    from conan.tools.files import load

    class Pkg(ConanFile):
        python_requires = "pyreq/0.1"

        def build(self):
            pyreq_path = self.python_requires["pyreq"].path
            myfile_path = os.path.join(pyreq_path, "myfile.txt")
            content = load(self, myfile_path)  # content = "Hello"
            self.output.info(content)
            # we could also copy the file, instead of reading it


Note that only ``exports`` works for this case, but not ``exports_sources``.

注意，在这种情况下只能 ``exports`` ，而不能 ``exports_sources``。

Testing python-requires
-----------------------

It is possible to test with ``test_package`` a ``python_require``, by adding a ``test_package/conanfile.py``:

通过添加 ``test_package/conanfile.py``，可以使用 ``test_package`` 测试 ``python_require``:

.. code-block:: python
    :caption: conanfile.py

    from conan import ConanFile

    def mynumber():
        return 42

    class PyReq(ConanFile):
        name = "pyreq"
        version = "1.0"
        package_type = "python-require"


.. code-block:: python
    :caption: test_package/conanfile.py

    from conan import ConanFile

    class Tool(ConanFile):
        def test(self):
            pyreq = self.python_requires["common"].module
            mynumber = pyreq.mynumber()
            self.output.info("{}!!!".format(mynumber))


Note that the ``test_package/conanfile.py`` does not need any type of declaration of the ``python_requires``, this is done
automatically and implicitly. We can now create and test it with:

注意， ``test_package/conanfile.py`` 不需要 ``python_requires`` 的任何类型的声明，这是自动隐式地完成的。我们现在可以创建和测试它:

.. code-block:: bash
    
    $ conan create .
    ...
    pyreq/0.1 (test package): 42!!!


Effect in package_id
--------------------

The ``python_requires`` will affect the ``package_id`` of the **consumer packages** using those dependencies.
By default, the policy is ``minor_mode``, which means:

``python_requires`` 将使用这些依赖项影响 **consumer packages** 的 ``package_id``。默认情况下，策略是 ``minor_mode``，这意味着:

- Changes to the **patch** version of the **revision** of a python-require will not affect the package ID. So depending
  on ``"pyreq/1.2.3"`` or ``"pyreq/1.2.4"`` will result in identical package ID (both will be mapped
  to ``"pyreq/1.2.Z"`` in the hash computation). Bump the patch version if you want to change your
  common code, but you don't want the consumers to be affected or to fire a re-build of the dependants.

  对 python-require **revision** 的 **patch** 版本的更改不会影响包 ID。因此，取决于 ``"pyreq/1.2.3"`` 或 ``"pyreq/1.2.4"`` 将导致相同的包 ID 
  (两者都将映射到 ``"pyreq/1.2.Z"`` 散列计算)。如果您希望更改公共代码，但又不希望使用者受到影响或重新生成依赖项，
  则改变补丁版本。   

- Changes to the **minor** version will produce a different package ID. So if you depend
  on ``"pyreq/1.2.3"``, and you bump the version to ``"pyreq/1.3.0"``, then, you will need to build
  new binaries that are using that new python-require. Bump the minor or major version if you want to
  make sure that packages requiring this python-require will be built using these changes in the code.

  对 **minor** 版本的更改将生成不同的包 ID。因此，如果您依赖于 ``"pyreq/1.2.3"``，并将版本提升到 ``"pyreq/1.3.0"``，那么，
  您将需要构建使用新的  python-require 的新二进制文件。如果您想确保使用代码中的这些更改来构建需要这个 python-require 的包，
  那么请改变次要版本或主要版本。

In most cases using a version-range ``python_requires = "pyreq/[>=1.0 <2.0]"`` is the right approach, because that means
the **major** version bumps are not included because they would require changes in the consumers themselves. It is then
possible to release a new major version of the ``pyreq/2.0``, and have consumers gradually change their requirements to
``python_requires = "pyreq/[>=2.0 <3.0]"``, fix the recipes, and move forward without breaking the whole project.

在大多数情况下，使用版本范围的 ``python_requires = "pyreq/[>=1.0 <2.0]"`` 是正确的方法，因为这意味着不包括 **major** 版本冲突，
因为它们需要对消费者本身进行更改。然后就可以发布一个新的 ``pyreq/2.0`` 主版本，让用户逐渐将他们的需求更改为 ``python_requires = "pyreq/[>=2.0 <3.0]"``，
修复配方，并在不破坏整个项目的情况下继续前进。

As with the regular ``requires``, this default can be customized with the ``core.package_id:default_python_mode`` configuration. 

与常规  ``requires`` 一样，可以使用 ``core.package_id:default_python_mode`` 配置定制这个默认值。

It is also possible to customize the effect of ``python_requires`` per package, using the ``package_id()``
method:

还可以使用 ``package_id()`` 方法自定义每个包的 ``python_requires`` 效果:

  .. code-block:: python

    from conan import ConanFile

    class Pkg(ConanFile):
        python_requires ="pyreq/[>=1.0]"
        def package_id(self):
            self.info.python_requires.patch_mode()



Resolution of python_requires
-----------------------------

There are few important things that should be taken into account when using ``python_requires``:

在使用 ``python_requires`` 时，几乎没有什么重要的事情需要考虑:

- Python requires recipes are loaded by the interpreter just once, and they are common to
  all consumers. Do not use any global state in the ``python_requires`` recipes.

  Python requires解释器只加载一次配方，并且它们对所有使用者都是通用的。不要在 ``python_requires`` 配方中使用任何全局状态。

- Python requires are private to the consumers. They are not transitive. Different consumers
  can require different versions of the same ``python-require``. Being private, they cannot
  be overriden from downstream in any way.

  Python 要求对消费者是私有的。它们不是传递性的。不同的消费者可能需要相同的  ``python-require`` 的不同版本。作为私有，它们不能以任何方式从下游被覆盖。

- ``python_requires`` cannot use regular ``requires`` or ``tool_requires``.

  ``python_requires`` 不能使用常规的 ``requires`` 或 ``tool_requires``。

- ``python_requires`` cannot be "aliased".
  
  ``python_requires`` 不能被“别名化”。
  
- ``python_requires`` can use native python ``import`` to other python files, as long as these are
  exported together with the recipe.

  ``python_requires`` 可以将原生 python ``import`` 到其他 python 文件，只要这些文件与配方一起导出。

- ``python_requires`` can be used as editable packages too.

  ``python_requires`` 也可以用作可编辑的包。

- ``python_requires`` are locked in lockfiles, to guarantee reproducibility, in the same way that other ``requires`` and ``tool_requires`` are locked.

  为了保证可重复性，``python_requires`` 被锁定在lockfiles中，其方式与锁定其他 ``requires`` 和 ``tool_requires`` 的方式相同。


.. note:: 

  **Best practices**

  - Even if ``python-requires`` can ``python_requires`` transitively other ``python-requires`` recipes, this is discouraged. Multiple level inheritance and reuse can become quite complex and difficult to manage, it is recommended to keep the hierarchy flat. 
    
    即使 ``python-requires`` 可以 ``python_requires`` 传递其他 ``python-requires`` 的配方，也不鼓励这样做。多级继承和重用可能变得非常复杂和难以管理，建议保持层次结构的平坦性。

  - Do not try to mix Python inheritance with ``python_requires_extend`` inheritance mechanisms, they are incompatible and can break.

    不要尝试将 Python 继承与 ``python_requires_extend`` 继承机制混合使用，它们是不兼容的，并且可能会中断。
    
  - Do not use multiple inheritance for ``python-requires``

    不要使用多重继承
