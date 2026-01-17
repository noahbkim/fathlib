import pathlib
import pickle
import timeit
from collections.abc import Callable

import fathlib


def main(f: Callable[[], object]) -> Callable[[], object]:
    if __name__ == "__main__":
        f()
    return f


def compare(
    old: str,
    new: str,
    *,
    number: int = 1_000_000,
    globals: dict[str, object] | None = None,
) -> None:
    old_time = timeit.timeit(old, number=number, globals=globals)
    new_time = timeit.timeit(new, number=number, globals=globals)
    if old_time > new_time:
        comparator = "faster"
        difference = old_time / new_time
    else:
        comparator = "slower"
        difference = new_time / old_time

    print(f"{new} is {difference:.2f}x {comparator} than {old}")
    print(f"  {old}: {old_time} s")
    print(f"  {new}: {new_time} s")


@main
def compare_construct_empty():
    compare(
        "pathlib.Path()",
        "fathlib.Path()",
        globals=globals(),
    )


@main
def compare_construct_single():
    compare(
        "pathlib.Path('/foo')",
        "fathlib.Path('/foo')",
        globals=globals(),
    )


@main
def compare_construct_multiple():
    compare(
        "pathlib.Path('/the', 'quick', 'brown', 'fox')",
        "fathlib.Path('/the', 'quick', 'brown', 'fox')",
        globals=globals(),
    )


@main
def compare_construct_parent():
    path = pathlib.Path("/the", "quick", "brown", "fox")
    fath = fathlib.Path("/the", "quick", "brown", "fox")
    compare(
        "path.parent",
        "fath.parent",
        globals=globals() | locals(),
    )


@main
def compare_name():
    path = pathlib.Path("/the", "quick", "brown", "fox")
    fath = fathlib.Path("/the", "quick", "brown", "fox")
    compare(
        "path.name",
        "fath.name",
        globals=globals() | locals(),
    )


@main
def compare_pickle():
    path = pathlib.Path("/the/quick/brown/fox")
    fath = fathlib.Path("/the/quick/brown/fox")
    compare(
        "pickle.dumps(path)",
        "pickle.dumps(fath)",
        globals=globals() | locals(),
    )


@main
def compare_unpickle():
    path_data = pickle.dumps(pathlib.Path("/the/quick/brown/fox"))
    fath_data = pickle.dumps(fathlib.Path("/the/quick/brown/fox"))
    compare(
        "pickle.loads(path_data)",
        "pickle.loads(fath_data)",
        globals=globals() | locals(),
    )
