.. note::

    We use CMake presets in this example. This requires CMake >= 3.23 because the
    "include" from ``CMakeUserPresets.json`` to ``CMakePresets.json`` is only supported
    since that version. If you prefer not to use presets you can use something like:

    我们在这个例子中使用 CMake 预设置。这需要 CMake>=3.23，因为从该版本开始支持 ``CMakeUserPresets.json`` 
    到 ``CMakePresets.json`` 的 "include" 。如果你不喜欢使用预设，你可以使用这样的东西:

    .. code-block:: bash

        cmake <path> -G <CMake generator> -DCMAKE_TOOLCHAIN_FILE=<path to
        conan_toolchain.cmake> -DCMAKE_BUILD_TYPE=Release

    Conan will show the exact CMake command everytime you run ``conan install`` in case
    you can't use the presets feature.

    每次运行 ``conan install`` 时，Conan都会显示确切的 CMake 命令，以防您不能使用预置特性。