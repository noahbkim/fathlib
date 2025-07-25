from __future__ import annotations

from .core import PurePosixFath as PurePosixFath

import sys

# TODO: disable construction on nt.
PosixFath = PurePosixFath

# TODO: Windows support
PureWindowsFath = None
WindowsFath = PureWindowsFath

if sys.platform == "win32":
    Fath = WindowsFath
else:
    Fath = PosixFath
