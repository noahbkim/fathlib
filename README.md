# Fathlib

Python's `pathlib` is slow and has weird semantics. This is not a drop-in replacement, but instead a reimagining.

```python console
# Python 3.13.3 (main, Apr  8 2025, 13:54:08) [Clang 17.0.0 (clang-1700.0.13.3)] on darwin
# Type "help", "copyright", "credits" or "license" for more information.
>>> from pathlib import Path
>>> from fathlib import Fath
>>> p = Path("/the/quick/brown/fox.txt")
>>> f = Fath("/the/quick/brown/fox.txt")
>>> p.parent
PosixPath('/the/quick/brown')
>>> f.parent
PosixFath('/the/quick/brown')
>>> import timeit
>>> timeit.timeit("p.parent", globals=globals())
0.9437695409869775
>>> timeit.timeit("f.parent", globals=globals())
0.06657808303134516
```

