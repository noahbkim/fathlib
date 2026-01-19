# Fathlib

Python's `pathlib` is slow. Let's make it faster.

```
# 14-inch, 2021 MacBook Pro with Apple M1 Pro, 32 GB RAM
fathlib.Path() is 6.60x faster than pathlib.Path()
  pathlib.Path(): 0.37 s
  fathlib.Path(): 0.06 s
fathlib.Path('/the/quick/brown/fox') is 4.27x faster than pathlib.Path('/the/quick/brown/fox')
  pathlib.Path('/the/quick/brown/fox'): 0.51 s
  fathlib.Path('/the/quick/brown/fox'): 0.12 s
fathlib.Path('/the/quick/brown/fox') is 5.69x slower than str('/the/quick/brown/fox')
  str('/the/quick/brown/fox'): 0.02 s
  fathlib.Path('/the/quick/brown/fox'): 0.12 s
fathlib.Path('/the', 'quick', 'brown', 'fox') is 3.85x faster than pathlib.Path('/the', 'quick', 'brown', 'fox')
  pathlib.Path('/the', 'quick', 'brown', 'fox'): 0.76 s
  fathlib.Path('/the', 'quick', 'brown', 'fox'): 0.20 s
fath.parent is 5.78x faster than path.parent
  path.parent: 0.89 s
  fath.parent: 0.15 s
fath.name is 1.79x faster than path.name
  path.name: 0.06 s
  fath.name: 0.03 s
pickle.dumps(fath) is 1.28x faster than pickle.dumps(path)
  pickle.dumps(path): 1.00 s
  pickle.dumps(fath): 0.79 s
pickle.loads(fath_data) is 2.52x faster than pickle.loads(path_data)
  pickle.loads(path_data): 1.50 s
  pickle.loads(fath_data): 0.59 s
fathlib.posix_join('/the', 'quick', 'brown', 'fox') is 6.97x faster than posixpath.join('/the', 'quick', 'brown', 'fox')
  posixpath.join('/the', 'quick', 'brown', 'fox'): 0.79 s
  fathlib.posix_join('/the', 'quick', 'brown', 'fox'): 0.11 s
fathlib.windows_join('/the', 'quick', 'brown', 'fox') is 12.76x faster than ntpath.join('/the', 'quick', 'brown', 'fox')
  ntpath.join('/the', 'quick', 'brown', 'fox'): 1.45 s
  fathlib.windows_join('/the', 'quick', 'brown', 'fox'): 0.11 s
fathlib.posix_parent('/the/quick/brown/fox') is 3.28x faster than posixpath.dirname('/the/quick/brown/fox')
  posixpath.dirname('/the/quick/brown/fox'): 0.28 s
  fathlib.posix_parent('/the/quick/brown/fox'): 0.09 s
fathlib.windows_parent('C:\\the\\quick\\brown\\fox') is 5.86x faster than ntpath.dirname('C:\\the\\quick\\brown\\fox')
  ntpath.dirname('C:\\the\\quick\\brown\\fox'): 0.77 s
  fathlib.windows_parent('C:\\the\\quick\\brown\\fox'): 0.13 s
```

This library is a work in progress. Its goals is to pass the `pathlib` unit
tests while being faster and lighter. It achieves that by reimplementing path
storage and manipulation in C, avoiding spurious allocations and inefficient
data accesses.

## Development

To build and install the extension after making changes, run the following:

```
uv build . && uv pip install .
```

For whatever reason, editable installation (using `uv pip install -e .`)
doesn't work with the extension; changes in the C source will not be reflected
at runtime after running `uv build .` alone. Please let me know if I'm doing
something wrong.

To run tests:

```
uv run python -m unittest discover test
```

To run benchmarks:

```
uv run test/benchmark.py
```

## Progress

The following is a rough roadmap for this repository:

  - [x] Skeleton
  - [x] Normalization
  - [ ] Join (need to use `METH_FASTCALL` and fix root parts)
  - [ ] Simple properties (in progress)
  - [ ] Parts
  - [ ] File system methods
  - [ ] Trivia
  - [ ] 3.12 test suite
  - [ ] >3.12 test suites
