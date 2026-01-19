# Fathlib

Python's `pathlib` is slow. Let's make it faster.

```
# 14-inch, 2021 MacBook Pro with Apple M1 Pro, 32 GB RAM
fathlib.Path() is 7.03x faster than pathlib.Path()
  pathlib.Path(): 0.40 s
  fathlib.Path(): 0.06 s
fathlib.Path('/foo') is 5.87x faster than pathlib.Path('/foo')
  pathlib.Path('/foo'): 0.51 s
  fathlib.Path('/foo'): 0.09 s
fathlib.Path('/the', 'quick', 'brown', 'fox') is 3.88x faster than pathlib.Path('/the', 'quick', 'brown', 'fox')
  pathlib.Path('/the', 'quick', 'brown', 'fox'): 0.76 s
  fathlib.Path('/the', 'quick', 'brown', 'fox'): 0.20 s
fath.parent is 5.77x faster than path.parent
  path.parent: 0.88 s
  fath.parent: 0.15 s
fath.name is 1.80x faster than path.name
  path.name: 0.06 s
  fath.name: 0.03 s
pickle.dumps(fath) is 1.26x faster than pickle.dumps(path)
  pickle.dumps(path): 1.00 s
  pickle.dumps(fath): 0.79 s
pickle.loads(fath_data) is 2.47x faster than pickle.loads(path_data)
  pickle.loads(path_data): 1.49 s
  pickle.loads(fath_data): 0.61 s
fathlib.posix_parent('/the/quick/brown/fox') is 3.35x faster than posixpath.dirname('/the/quick/brown/fox')
  posixpath.dirname('/the/quick/brown/fox'): 0.29 s
  fathlib.posix_parent('/the/quick/brown/fox'): 0.09 s
fathlib.windows_parent('C:\\the\\quick\\brown\\fox') is 5.87x faster than ntpath.dirname('C:\\the\\quick\\brown\\fox')
  ntpath.dirname('C:\\the\\quick\\brown\\fox'): 0.78 s
  fathlib.windows_parent('C:\\the\\quick\\brown\\fox'): 0.13 s
```

This library is a work in progress. Its goals is to pass the `pathlib` unit
tests while being faster and lighter. It achieves that by reimplementing path
storage and manipulation in C, avoiding spurious allocations and inefficient
data accesses.
