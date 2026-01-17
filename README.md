# Fathlib

Python's `pathlib` is slow. Let's make it faster.

```
# 14-inch, 2021 MacBook Pro with Apple M1 Pro, 32 GB RAM
fathlib.Path() is 6.59x faster than pathlib.Path()
  pathlib.Path(): 0.3834155840013409 s
  fathlib.Path(): 0.0581387909987825 s
fathlib.Path('/foo') is 6.14x faster than pathlib.Path('/foo')
  pathlib.Path('/foo'): 0.5106039580059587 s
  fathlib.Path('/foo'): 0.08309558299515629 s
fathlib.Path('/the', 'quick', 'brown', 'fox') is 3.98x faster than pathlib.Path('/the', 'quick', 'brown', 'fox')
  pathlib.Path('/the', 'quick', 'brown', 'fox'): 0.7786027919937624 s
  fathlib.Path('/the', 'quick', 'brown', 'fox'): 0.19582845800323412 s
fath.parent is 6.42x faster than path.parent
  path.parent: 0.8850902499980293 s
  fath.parent: 0.1378634169959696 s
fath.name is 1.74x faster than path.name
  path.name: 0.06076312500226777 s
  fath.name: 0.034932916998513974 s
pickle.dumps(fath) is 1.28x faster than pickle.dumps(path)
  pickle.dumps(path): 1.0065801669989014 s
  pickle.dumps(fath): 0.7871034170020721 s
pickle.loads(fath_data) is 2.46x faster than pickle.loads(path_data)
  pickle.loads(path_data): 1.463799791999918 s
  pickle.loads(fath_data): 0.5946308330021566 s
```

This library is a work in progress. Its goals is to pass the `pathlib` unit
tests while being faster and lighter. It achieves that by reimplementing path
storage and manipulation in C, avoiding spurious allocations and inefficient
data accesses.
