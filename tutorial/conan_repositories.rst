.. _conan_repositories:

Working with Conan repositories
===============================

We already :ref:`learned how to download and use packages <tutorial_consuming_packages>`
from `Conan Center <https://conan.io/center>`_ that is the official repository for open
source Conan packages. We also :ref:`learned how to create our own packages
<tutorial_creating_packages>` and store them in the Conan local cache for reusing later.
In this section we cover how you can use the Conan repositories to upload your
recipes and binaries and store them for later use on another machine, project, or for
sharing purposes.

我们已经 :ref:`learned how to download and use packages <tutorial_consuming_packages>` 
(从 `Conan Center <https://conan.io/center>`_ )， `Conan Center <https://conan.io/center>`_ 
是开源 Conan 软件包的官方存储库。我们还 :ref:`learned how to create our own packages <tutorial_creating_packages>`，
并将它们存储在 Conan 本地缓存中，以便以后重用。在本节中，我们将介绍如何使用 Conan 
存储库上传您的配方和二进制文件，并将它们存储起来，以便以后在其他机器、项目或共享目的使用。

First we will cover how you can setup a Conan repository locally (you can skip this part
if you already have a Conan remote configured). Then we will explain how to upload
packages to your own repositories and how to operate when you have multiple Conan remotes
configured. Finally, we will briefly cover how you can contribute to the Conan Center
central repository. 

首先，我们将介绍如何在本地设置 Conan 存储库(如果已经配置了 Conan 远程，则可以跳过这一部分)。然后，
我们将解释如何上传包到您自己的仓库，以及如何操作时，您有多个 Conan 远程配置。最后，我们将简要介绍如何对 
ConanCenter 中心存储库做出贡献。

.. toctree::
   :maxdepth: 2
   :caption: Table of contents
   
   conan_repositories/setting_up_conan_remotes.rst
   conan_repositories/uploading_packages.rst
   conan_repositories/conan_center.rst
