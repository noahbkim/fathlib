from __future__ import annotations

from .core import PurePosixFath as PurePosixFath
from .core import PureWindowsFath as PureWindowsFath

import sys

PosixFath = PurePosixFath
WindowsFath = PureWindowsFath

if sys.platform == "win32":
    Fath = WindowsFath
else:
    Fath = PosixFath

# pathlib API

PosixPath = PosixFath
PurePosixPath = PurePosixFath
WindowsPath = WindowsFath
PureWindowsPath = PureWindowsFath
Path = Fath
PurePath = Fath
