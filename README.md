# Fathlib

Python's `pathlib` is slow. Let's make it faster.

```
# 14-inch, 2021 MacBook Pro with Apple M1 Pro, 32 GB RAM
fathlib.Path() is 6.69x faster than pathlib.Path()
  pathlib.Path(): 0.39021120800316567 s
  fathlib.Path(): 0.058284459002607036 s
fathlib.Path('/foo') is 6.36x faster than pathlib.Path('/foo')
  pathlib.Path('/foo'): 0.5253628749997006 s
  fathlib.Path('/foo'): 0.08258587499585701 s
fathlib.Path('/the', 'quick', 'brown', 'fox') is 3.80x faster than pathlib.Path('/the', 'quick', 'brown', 'fox')
  pathlib.Path('/the', 'quick', 'brown', 'fox'): 0.7783009999984642 s
  fathlib.Path('/the', 'quick', 'brown', 'fox'): 0.2047802919987589 s
fath.parent is 6.45x faster than path.parent
  path.parent: 0.9034737499969197 s
  fath.parent: 0.14014754099480342 s
fath.name is 1.82x faster than path.name
  path.name: 0.0628354170039529 s
  fath.name: 0.03457466600229964 s
```

This library is a work in progress. Its goals is to pass the `pathlib` unit
tests while being faster and lighter. It achieves that by reimplementing path
storage and manipulation in C, avoiding spurious allocations and inefficient
data accesses.
