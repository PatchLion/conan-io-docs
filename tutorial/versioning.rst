.. _tutorial_versioning:


Versioning
==========

This section of the tutorial introduces several concepts about versioning of packages.

本教程的这一部分介绍了有关软件包版本控制的几个概念。

First, explicit version updates and how to define versions of packages is explained.

首先，解释了显式的版本更新以及如何定义包的版本。

Then, it will be introduced how ``requires`` with version ranges can 
help to automate updating to the latest versions.

然后，将介绍版本范围 ``requires`` 如何帮助自动更新到最新版本。

There are some situations when recipes or source code are changed, but the version of the
package is not increased. For those situations, Conan uses automatic ``revisions`` to 
be able to provide traceability and reproducibility of those changes.

在某些情况下，配方或源代码会发生更改，但是包的版本不会增加。对于这些情况，Conan 
使用自动  ``revisions``  来提供这些更改的可跟踪性和可重复性。

Lockfiles are a common mechanism in package managers to be able to reproduce the same
dependency graph later in time, even when new versions or revisions of dependencies are uploaded.
Conan also provides lockfiles to be able to guarantee this reproducibility.

在包管理器中，锁文件是一种常见的机制，它能够在以后及时地重现相同的依赖关系图，
即使在上传新版本或依赖关系修订版本时也是如此。Conan 还提供了锁文件来保证这种可重复性。

Finally, when different branches of a dependency graph ``requires`` different versions of the
same package, that is called a "version conflict". The tutorial will also introduce these
errors and how to address them.

最后，当依赖关系图的不同分支 ``requires`` 同一包的不同版本时，称为“版本冲突”。本教程还将介绍这些错误以及如何解决它们。

.. toctree::
   :maxdepth: 2
   :caption: Table of contents

   versioning/versions
   versioning/version_ranges
   versioning/revisions
   versioning/lockfiles
   versioning/conflicts
