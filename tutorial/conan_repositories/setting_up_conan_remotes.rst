.. _setting_up_conan_remotes:

Setting up a Conan remote
=========================

There are several options to set-up a Conan repository:

配置 Conan 存储库有几种选择:

**For private development:**

- :ref:`Artifactory Community Edition for C/C++ <artifactory_ce_cpp>`: Artifactory
  Community Edition (CE) for C/C++ is a completely free Artifactory server that implements 
  both Conan and generic repositories. It is the recommended server for companies and
  teams wanting to host their own private repository. It has a web UI, advanced
  authentication and permissions, very good performance and scalability, a REST API, and
  can host generic artifacts (tarballs, zips, etc). Check :ref:`artifactory_ce_cpp` for
  more information.

  ArtiFactory Community Edition (CE) for C/C++ 是一个完全免费的 ArtiFactory 服务器, 它实现了 Conan 和通用存储库。
  它是希望托管自己的私有存储库的公司和团队的推荐服务器。它有一个 web UI、高级认证和权限、非常好的性能和可伸缩性、
  一个 REST API，并且可以托管通用工件(tarball、 zip 等)。有关更多信息，请查看  :ref:`artifactory_ce_cpp` 。

- :ref:`Conan server <conan_server>`: Simple, free and open source, MIT
  licensed server that is part of the `conan-io organization
  <https://github.com/conan-io>`_ project. Check
  :ref:`conan_server` for more information.

  简单、免费和开源的 MIT 许可服务器，是  `conan-io organization <https://github.com/conan-io>`_ 的一部分。
  有关更多信息，请检查 :ref:`conan_server`。


**Enterprise solutions:**

- **Artifactory Pro**: Artifactory is the binary repository manager for all major
  packaging formats. It is the recommended remote type for enterprise and professional
  package management. Check the `Artifactory Documentation
  <https://www.jfrog.com/confluence/display/JFROG/JFrog+Artifactory>`_ for more
  information. For a comparison between Artifactory editions, check the `Artifactory
  Comparison Matrix
  <https://www.jfrog.com/confluence/display/JFROG/Artifactory+Comparison+Matrix>`_.

  ArtiFactory 是所有主要打包格式的二进制存储库管理器。它是企业推荐的远程类型和专业的包管理。
  有关更多信息，请查看  `Artifactory Documentation <https://www.jfrog.com/confluence/display/JFROG/JFrog+Artifactory>`_。
  对于Artifactory版本之间的比较，查看 `Artifactory Comparison Matrix <https://www.jfrog.com/confluence/display/JFROG/Artifactory+Comparison+Matrix>`_。

.. toctree::
   :maxdepth: 2
   :hidden:
   
   setting_up_conan_remotes/artifactory/artifactory_ce_cpp.rst
   setting_up_conan_remotes/conan_server.rst
