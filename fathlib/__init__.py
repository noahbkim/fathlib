from __future__ import annotations

from .core import PosixFath

import sys

if sys.platform == "win32":
    raise NotImplementedError("Sorry")
else:
    Fath = PosixFath
