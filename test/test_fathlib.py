import pathlib
import unittest

import fathlib

ARGS = (
    ("",),
    ("/",),
    ("//",),
    ("///",),
    ("////",),
    ("a",),
    ("a/",),
    ("a//",),
    ("/a",),
    ("/a/",),
    ("/a//",),
    ("//a",),
    ("//a/",),
    ("//a//",),
    ("///a",),
    ("///a/",),
    ("///a//",),
    ("////a",),
    ("////a/",),
    ("////a//",),
    ("a/b",),
    ("a//b",),
    ("a/b/",),
    ("a//b/",),
    ("a/b//",),
    ("a//b//",),
    ("/a/b",),
    ("/a//b",),
    ("/a/b/",),
    ("/a//b/",),
    ("/a/b//",),
    ("/a//b//",),
    ("//a/b",),
    ("//a//b",),
    ("//a/b/",),
    ("//a//b/",),
    ("//a/b//",),
    ("//a//b//",),
    ("///a/b",),
    ("///a//b",),
    ("///a/b/",),
    ("///a//b/",),
    ("///a/b//",),
    ("///a//b//",),
    (".",),
    ("./",),
    ("/.",),
    ("/./",),
    ("/.//",),
    ("//.",),
    ("//./",),
    ("//.//",),
    ("///.",),
    ("///./",),
    ("///.//",),
    ("////.",),
    ("////./",),
    ("////.//",),
    ("./b",),
    (".//b",),
    ("./b/",),
    (".//b/",),
    ("./b//",),
    (".//b//",),
    ("/./b",),
    ("/.//b",),
    ("/./b/",),
    ("/.//b/",),
    ("/./b//",),
    ("/.//b//",),
    ("//./b",),
    ("//.//b",),
    ("//./b/",),
    ("//.//b/",),
    ("//./b//",),
    ("//.//b//",),
    ("///./b",),
    ("///.//b",),
    ("///./b/",),
    ("///.//b/",),
    ("///./b//",),
    ("///.//b//",),
    ("./.",),
    (".//.",),
    ("././",),
    (".//./",),
    ("././/",),
    (".//.//",),
    ("/./.",),
    ("/.//.",),
    ("/././",),
    ("/.//./",),
    ("/././/",),
    ("/.//.//",),
    ("//./.",),
    ("//.//.",),
    ("//././",),
    ("//.//./",),
    ("//././/",),
    ("//.//.//",),
    ("///./.",),
    ("///.//.",),
    ("///././",),
    ("///.//./",),
    ("///././/",),
    ("///.//.//",),
)

EXPRESSIONS = (
    "str({})",
    # "repr({})",
    # "{}.name",
    # "{}.root",
    # "str({}.parent)",
)


class TestConformance(unittest.TestCase):
    def test_conformance(self) -> None:
        for p_type, q_type in (
            (fathlib.PurePosixPath, pathlib.PurePosixPath),
            (fathlib.PureWindowsPath, pathlib.PureWindowsPath),
        ):
            for args in ARGS:
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


if __name__ == "__main__":
    unittest.main()
