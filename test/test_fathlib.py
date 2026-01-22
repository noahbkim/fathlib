import pathlib
import itertools
import ntpath
import unittest

import fathlib

ARGS = (
    "",
    "/",
    "//",
    "///",
    "////",
    "a",
    "a/",
    "a//",
    "/a",
    "/a/",
    "/a//",
    "//a",
    "//a/",
    "//a//",
    "///a",
    "///a/",
    "///a//",
    "////a",
    "////a/",
    "////a//",
    "a/b",
    "a//b",
    "a/b/",
    "a//b/",
    "a/b//",
    "a//b//",
    "/a/b",
    "/a//b",
    "/a/b/",
    "/a//b/",
    "/a/b//",
    "/a//b//",
    "//a/b",
    "//a//b",
    "//a/b/",
    "//a//b/",
    "//a/b//",
    "//a//b//",
    "///a/b",
    "///a//b",
    "///a/b/",
    "///a//b/",
    "///a/b//",
    "///a//b//",
    ".",
    "./",
    "/.",
    "/./",
    "/.//",
    "//.",
    "//./",
    "//.//",
    "///.",
    "///./",
    "///.//",
    "////.",
    "////./",
    "////.//",
    "./b",
    ".//b",
    "./b/",
    ".//b/",
    "./b//",
    ".//b//",
    "/./b",
    "/.//b",
    "/./b/",
    "/.//b/",
    "/./b//",
    "/.//b//",
    "//./b",
    "//.//b",
    "//./b/",
    "//.//b/",
    "//./b//",
    "//.//b//",
    "///./b",
    "///.//b",
    "///./b/",
    "///.//b/",
    "///./b//",
    "///.//b//",
    "./.",
    ".//.",
    "././",
    ".//./",
    "././/",
    ".//.//",
    "/./.",
    "/.//.",
    "/././",
    "/.//./",
    "/././/",
    "/.//.//",
    "//./.",
    "//.//.",
    "//././",
    "//.//./",
    "//././/",
    "//.//.//",
    "///./.",
    "///.//.",
    "///././",
    "///.//./",
    "///././/",
    "///.//.//",
)

EXPRESSIONS = (
    "str({})",
    "repr({})",
    # "{}.is_absolute()",
    # "{}.name",
    # "{}.root",
    # "str({}.parent)",
)

WINDOWS_ARGS = (
    "C:",
    "\\",
    "/",
    "?",
    ".",
    "UNC",
    "part",
)


class TestConformance(unittest.TestCase):
    def test_path_conformance(self) -> None:
        for p_type, q_type in (
            (fathlib.PurePosixPath, pathlib.PurePosixPath),
            (fathlib.PureWindowsPath, pathlib.PureWindowsPath),
        ):
            for n in 0, 1:
                for args in itertools.product(ARGS, repeat=n):
                    p_repr = f"{p_type.__name__}{args!r}"
                    q_repr = f"{q_type.__name__}{args!r}"
                    p = p_type(*args)
                    q = q_type(*args)
                    for expression in EXPRESSIONS:
                        p_expr = expression.format(p_repr)
                        q_expr = expression.format(q_repr)
                        with self.subTest(p_expr):
                            p_value = eval(expression.format("p"), {}, {"p": p})
                            q_value = eval(expression.format("q"), {}, {"q": q})
                            self.assertEqual(p_value, q_value, f"{p_expr} != {q_expr}")

    def test_windows_drive_conformance(self) -> None:
        for n in range(1, 6):
            for args in itertools.product(WINDOWS_ARGS, repeat=n):
                for sep in "\\", "/":
                    path = sep.join(args)
                    with self.subTest(path):
                        self.assertEqual(
                            fathlib.windows_drive(path),
                            ntpath.splitdrive(path)[0],
                            repr(path),
                        )
        for path in ARGS:
            with self.subTest(path):
                self.assertEqual(
                    fathlib.windows_drive(path),
                    ntpath.splitdrive(path)[0],
                    repr(path),
                )

    def test_windows_is_absolute_conformance(self) -> None:
        for n in range(1, 6):
            for args in itertools.product(WINDOWS_ARGS, repeat=n):
                for sep in "\\", "/":
                    path = sep.join(args)
                    with self.subTest(path):
                        self.assertEqual(
                            fathlib.windows_is_absolute(path),
                            ntpath.isabs(path),
                            repr(path),
                        )
        for path in ARGS:
            with self.subTest(path):
                self.assertEqual(
                    fathlib.windows_is_absolute(path),
                    ntpath.isabs(path),
                    repr(path),
                )


class TextTestResult(unittest.TextTestResult):
    subTestCount = 0
    subTestSuccessCount = 0

    def addSubTest(self, test, subtest, err):
        self.subTestCount += 1
        self.subTestSuccessCount += not err
        return super().addSubTest(test, subtest, err)


class TextTestRunner(unittest.TextTestRunner):
    resultclass = TextTestResult

    def run(self, test):
        result = super().run(test)
        if isinstance(result, TextTestResult):
            successes = result.subTestSuccessCount
            failures = result.subTestCount - successes
            self.stream.writeln(f"SUBTEST ({successes=}, {failures=})")
        return result


if __name__ == "__main__":
    unittest.main(testRunner=TextTestRunner)
