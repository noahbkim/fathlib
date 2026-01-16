from __future__ import annotations

import ntpath
import os
import posixpath

from .core import PosixFath as PosixFath
from .core import WindowsFath as WindowsFath
from .core import posix_normalize_dot as posix_normalize_dot
from .core import posix_normalize_slash as posix_normalize_slash
from .core import posix_normalize as posix_normalize


class PurePosixPath(PosixFath):
    __slots__ = ()
    _flavour = posixpath


class PureWindowsPath(WindowsFath):
    __slots__ = ()
    _flavour = ntpath


class PosixPath(PurePosixPath):
    __slots__ = ()

    if os.name == "nt":

        def __new__(cls, *args, **kwargs):
            raise NotImplementedError(
                f"cannot instantiate {cls.__name__!r} on your system"
            )


class WindowsPath(PureWindowsPath):
    __slots__ = ()

    if os.name != "nt":

        def __new__(cls, *args, **kwargs):
            raise NotImplementedError(
                f"cannot instantiate {cls.__name__!r} on your system"
            )


if os.name == "nt":
    Path = WindowsPath
    PureFath = PureWindowsPath
else:
    Path = PosixPath
    PurePath = PurePosixPath
