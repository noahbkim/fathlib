# Fathlib

Python's `pathlib` is slow. Let's make it faster.

```
# 14-inch, 2021 MacBook Pro with Apple M1 Pro, 32 GB RAM
fathlib.Path() is 6.63x faster than pathlib.Path()
  pathlib.Path(): 0.37 s
  fathlib.Path(): 0.06 s
fathlib.Path('/foo') is 6.26x faster than pathlib.Path('/foo')
  pathlib.Path('/foo'): 0.51 s
  fathlib.Path('/foo'): 0.08 s
fathlib.Path('/the', 'quick', 'brown', 'fox') is 3.73x faster than pathlib.Path('/the', 'quick', 'brown', 'fox')
  pathlib.Path('/the', 'quick', 'brown', 'fox'): 0.75 s
  fathlib.Path('/the', 'quick', 'brown', 'fox'): 0.20 s
fath.parent is 5.27x faster than path.parent
  path.parent: 0.88 s
  fath.parent: 0.17 s
fath.name is 1.79x faster than path.name
  path.name: 0.06 s
  fath.name: 0.03 s
pickle.dumps(fath) is 1.25x faster than pickle.dumps(path)
  pickle.dumps(path): 0.97 s
  pickle.dumps(fath): 0.78 s
pickle.loads(fath_data) is 2.51x faster than pickle.loads(path_data)
  pickle.loads(path_data): 1.47 s
  pickle.loads(fath_data): 0.59 s
fathlib.posix_parent('/the/quick/brown/fox') is 3.17x faster than posixpath.dirname('/the/quick/brown/fox')
  posixpath.dirname('/the/quick/brown/fox'): 0.28 s
  fathlib.posix_parent('/the/quick/brown/fox'): 0.09 s
fathlib.windows_parent('C:\\the\\quick\\brown\\fox') is 8.00x faster than ntpath.dirname('C:\\the\\quick\\brown\\fox')
  ntpath.dirname('C:\\the\\quick\\brown\\fox'): 0.77 s
  fathlib.windows_parent('C:\\the\\quick\\brown\\fox'): 0.10 s
```

This library is a work in progress. Its goals is to pass the `pathlib` unit
tests while being faster and lighter. It achieves that by reimplementing path
storage and manipulation in C, avoiding spurious allocations and inefficient
data accesses.
