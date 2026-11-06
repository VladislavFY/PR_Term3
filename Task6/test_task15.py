import io
import random
import subprocess
import sys
import tempfile
import unittest
from contextlib import redirect_stderr, redirect_stdout
from decimal import Decimal
from fractions import Fraction
from pathlib import Path
from unittest.mock import patch

import task15


SAMPLE = "4\n0 0\n2 0\n2 2\n0 2\n\n4\n5 0\n7 0\n7 2\n5 2\n"


def points(values):
    return [(Fraction(str(x)), Fraction(str(y))) for x, y in values]


def rectangle(x1, y1, x2, y2):
    return points([(x1, y1), (x2, y1), (x2, y2), (x1, y2)])


class GeometryTests(unittest.TestCase):
    def test_point_projection_inside_segment(self):
        p, a, b = points([(1, 2), (0, 0), (3, 0)])
        self.assertEqual(task15.point_distance_squared(p, a, b), 4)

    def test_point_projection_before_segment(self):
        p, a, b = points([(-2, 3), (0, 0), (3, 0)])
        self.assertEqual(task15.point_distance_squared(p, a, b), 13)

    def test_point_projection_after_segment(self):
        p, a, b = points([(5, 3), (0, 0), (3, 0)])
        self.assertEqual(task15.point_distance_squared(p, a, b), 13)

    def test_zero_length_segment(self):
        p, a = points([(3, 4), (0, 0)])
        self.assertEqual(task15.point_distance_squared(p, a, a), 25)

    def test_crossing_segments(self):
        a, b, c, d = points([(0, 0), (4, 4), (0, 4), (4, 0)])
        self.assertEqual(task15.segment_distance_squared(a, b, c, d), 0)

    def test_collinear_disjoint_segments(self):
        a, b, c, d = points([(0, 0), (1, 0), (3, 0), (5, 0)])
        self.assertEqual(task15.segment_distance_squared(a, b, c, d), 4)

    def test_parallel_segments(self):
        a, b, c, d = points([(0, 0), (4, 0), (1, 3), (3, 3)])
        self.assertEqual(task15.segment_distance_squared(a, b, c, d), 9)

    def test_separate_squares(self):
        first = rectangle(0, 0, 2, 2)
        second = rectangle(5, 0, 7, 2)
        self.assertEqual(task15.polygon_distance_squared(first, second), 9)

    def test_diagonal_gap(self):
        first = rectangle(0, 0, 1, 1)
        second = rectangle(3, 4, 4, 5)
        self.assertEqual(task15.polygon_distance_squared(first, second), 13)

    def test_crossing_without_contained_vertices(self):
        first = rectangle(-3, -1, 3, 1)
        second = rectangle(-1, -3, 1, 3)
        self.assertEqual(task15.polygon_distance_squared(first, second), 0)

    def test_containment(self):
        first = rectangle(0, 0, 10, 10)
        second = rectangle(2, 2, 3, 3)
        self.assertEqual(task15.polygon_distance_squared(first, second), 0)
        self.assertEqual(task15.polygon_distance_squared(second, first), 0)

    def test_touching_edges(self):
        self.assertEqual(task15.polygon_distance_squared(
            rectangle(0, 0, 2, 2), rectangle(2, 0, 4, 2)), 0)

    def test_touching_vertices(self):
        self.assertEqual(task15.polygon_distance_squared(
            rectangle(0, 0, 2, 2), rectangle(2, 2, 4, 4)), 0)

    def test_identical_polygons(self):
        first = rectangle(0, 0, 2, 2)
        self.assertEqual(task15.polygon_distance_squared(first, first[::-1]), 0)

    def test_concave_polygon_notch(self):
        first = points([(0, 0), (4, 0), (4, 1), (1, 1), (1, 4), (0, 4)])
        second = rectangle(2, 2, 3, 3)
        self.assertEqual(task15.polygon_distance_squared(first, second), 1)

    def test_nearest_point_inside_slanted_edge(self):
        first = points([(0, 0), (4, 0), (2, 4)])
        second = points([(4, 2), (5, 2), (4, 3)])
        self.assertEqual(task15.polygon_distance_squared(first, second), Fraction(4, 5))

    def test_winding_order_and_start_vertex(self):
        first = points([(0, 0), (4, 0), (2, 4)])
        second = points([(4, 2), (5, 2), (4, 3)])

        for i in range(len(first)):
            for j in range(len(second)):
                a = first[i:] + first[:i]
                b = second[j:] + second[:j]
                self.assertEqual(task15.polygon_distance_squared(a[::-1], b), Fraction(4, 5))

    def test_exact_decimal_gap(self):
        first = rectangle(0, 0, "0.1", 1)
        second = rectangle("0.1000000000000000000000000001", 0, 1, 1)
        squared = task15.polygon_distance_squared(first, second)
        self.assertEqual(squared, Fraction(1, 10 ** 56))
        self.assertEqual(task15.get_distance(squared), Decimal("1e-28"))

    def test_large_intermediate_values(self):
        first = rectangle("1e308", 0, "1.1e308", "1e307")
        second = rectangle("1.2e308", 0, "1.3e308", "1e307")
        squared = task15.polygon_distance_squared(first, second)
        self.assertEqual(task15.get_distance(squared), Decimal("1e307"))

    def test_small_intermediate_values(self):
        first = rectangle(0, 0, "1e-300", "1e-300")
        second = rectangle("3e-300", 0, "4e-300", "1e-300")
        squared = task15.polygon_distance_squared(first, second)
        self.assertEqual(task15.get_distance(squared), Decimal("2e-300"))

    def test_result_overflow(self):
        first = points([("-1.7e308", 0), ("-1.6e308", 0), ("-1.7e308", 1)])
        second = points([("1.6e308", 0), ("1.7e308", 0), ("1.6e308", 1)])

        with self.assertRaises(OverflowError):
            task15.get_distance(task15.polygon_distance_squared(first, second))

    def test_result_underflow(self):
        first = rectangle(0, 0, 1, 1)
        second = rectangle("1." + "0" * 324 + "1", 0, 2, 1)

        with self.assertRaisesRegex(ArithmeticError, "too small"):
            task15.get_distance(task15.polygon_distance_squared(first, second))

    def test_result_range_boundaries(self):
        for value in [Fraction(task15.MAX_NUMBER), Fraction(task15.MIN_NUMBER)]:
            self.assertTrue(task15.get_distance(value * value).is_finite())

    def test_random_rectangles_against_independent_formula(self):
        generator = random.Random(15)

        for i in range(300):
            x1 = generator.randint(-100, 100)
            y1 = generator.randint(-100, 100)
            x2 = x1 + generator.randint(1, 20)
            y2 = y1 + generator.randint(1, 20)
            x3 = generator.randint(-100, 100)
            y3 = generator.randint(-100, 100)
            x4 = x3 + generator.randint(1, 20)
            y4 = y3 + generator.randint(1, 20)

            dx = max(x3 - x2, x1 - x4, 0)
            dy = max(y3 - y2, y1 - y4, 0)
            expected = dx * dx + dy * dy

            with self.subTest(case=i):
                actual = task15.polygon_distance_squared(
                    rectangle(x1, y1, x2, y2), rectangle(x3, y3, x4, y4))
                self.assertEqual(actual, expected)


class ValidationTests(unittest.TestCase):
    def test_invalid_polygons(self):
        invalid = [
            [(0, 0), (1, 1)],
            [(0, 0), (2, 0), (0, 2), (2, 0)],
            [(0, 0), (1, 0), (2, 0)],
            [(0, 0), (4, 4), (0, 4), (3, 0)],
            [(0, 0), (3, 0), (1, 0), (1, 2), (0, 2)],
            [(0, 0), (4, 0), (4, 4), (2, 0), (0, 4)],
        ]

        for values in invalid:
            with self.subTest(points=values), self.assertRaises(ValueError):
                task15.check_polygon(points(values), 1)

    def test_repeated_closing_vertex(self):
        first = rectangle(0, 0, 2, 2)
        self.assertEqual(task15.check_polygon(first + [first[0]], 1), first)

    def test_collinear_forward_vertices(self):
        first = points([(0, 0), (1, 0), (2, 0), (2, 2), (0, 2)])
        self.assertEqual(task15.check_polygon(first, 1), first)

    def test_invalid_numbers(self):
        invalid = ["abc", "1,5", "1_0", "1__0", "--1", "NaN", "sNaN",
                   "inf", "-Infinity", "1e309", "-1e309", "1e-400", "1e1000000"]

        for text in invalid:
            with self.subTest(number=text), self.assertRaises(ValueError):
                task15.read_number(text, 7)

    def test_valid_numbers(self):
        for text in ["0", "-0", "+2", ".5", "2.", "-1.25e+2", "1e308", "5e-324", "0e-999999999"]:
            with self.subTest(number=text):
                self.assertEqual(task15.read_number(text, 1), Fraction(Decimal(text)))

    def test_exact_input_range_boundaries(self):
        for value in [task15.MAX_NUMBER, task15.MIN_NUMBER]:
            self.assertEqual(task15.read_number(str(value), 1), Fraction(value))

        above_max = Fraction(task15.MAX_NUMBER) + 1
        with self.assertRaises(ValueError):
            task15.read_number(str(above_max.numerator), 1)

    def test_invalid_file_contents(self):
        invalid = [
            "",
            "\n \n",
            "2\n0 0\n1 1\n",
            "3.0\n0 0\n1 0\n0 1\n",
            "1000000000000000000000000000000000000000000\n0 0\n",
            "3\n0 0\n1 0\n0 1\n",
            SAMPLE.replace("0 0\n", "0 0 0\n", 1),
            SAMPLE.replace("0 0\n", "0\n", 1),
            SAMPLE.replace("0 0\n", "text 0\n", 1),
            SAMPLE + "99 99\n",
        ]

        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "input.txt"

            for text in invalid:
                path.write_text(text, encoding="utf-8")
                with self.subTest(text=text), self.assertRaises(ValueError):
                    task15.read_polygons(path)

    def test_file_with_bom_and_blank_lines(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "input.txt"
            path.write_text("\n" + SAMPLE + "\n", encoding="utf-8-sig")
            first, second = task15.read_polygons(path)
            self.assertEqual(task15.polygon_distance_squared(first, second), 9)

    # Cyrillic file names below are intentional input data.
    def test_filename_quotes_spaces_and_cyrillic(self):
        name = "Мои данные/два многоугольника.txt"
        self.assertEqual(task15.prepare_filename('"' + name + '"'), Path(name))
        self.assertEqual(task15.prepare_filename("'" + name + "'"), Path(name))
        self.assertEqual(task15.prepare_filename(name), Path(name))
        self.assertEqual(task15.prepare_filename('"  данные.txt  "'), Path("  данные.txt  "))

    def test_filename_home(self):
        self.assertEqual(task15.prepare_filename("~/polygons.txt"), Path.home() / "polygons.txt")

    def test_invalid_filename(self):
        for name in ["", "   ", '""', "file\0.txt"]:
            with self.subTest(name=name), self.assertRaises(ValueError):
                task15.prepare_filename(name)


class InterfaceTests(unittest.TestCase):
    def run_program(self, arguments, text=None):
        program = Path(task15.__file__).resolve()
        return subprocess.run([sys.executable, str(program)] + arguments,
                              input=text, text=True, capture_output=True, timeout=10)

    def test_command_line_path_with_spaces_and_cyrillic(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "два многоугольника.txt"
            path.write_text(SAMPLE, encoding="utf-8")
            result = self.run_program([str(path)])
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(result.stdout.strip(), "Minimum distance: 3")

    def test_interactive_filename(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "полигон 15.txt"
            path.write_text(SAMPLE, encoding="utf-8")
            result = self.run_program([], '"' + str(path) + '"\n')
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn("Minimum distance: 3", result.stdout)

    def test_missing_file(self):
        with tempfile.TemporaryDirectory() as directory:
            result = self.run_program([str(Path(directory) / "missing.txt")])
            self.assertEqual(result.returncode, 1)
            self.assertIn("File not found", result.stderr)
            self.assertNotIn("Traceback", result.stderr)

    def test_directory_instead_of_file(self):
        with tempfile.TemporaryDirectory() as directory:
            result = self.run_program([directory])
            self.assertEqual(result.returncode, 1)
            self.assertIn("a directory", result.stderr)

    def test_wrong_encoding(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "input.txt"
            path.write_bytes(b"\xff\xfe\x00")
            result = self.run_program([str(path)])
            self.assertEqual(result.returncode, 1)
            self.assertIn("UTF-8", result.stderr)

    def test_too_many_arguments(self):
        result = self.run_program(["first.txt", "second.txt"])
        self.assertEqual(result.returncode, 1)
        self.assertIn("quotes", result.stderr)

    def test_end_of_input(self):
        result = self.run_program([], "")
        self.assertEqual(result.returncode, 1)
        self.assertIn("Input cancelled", result.stderr)

    def test_access_denied(self):
        error_output = io.StringIO()

        with patch.object(sys, "argv", ["task15.py", "input.txt"]):
            with patch.object(task15, "read_polygons", side_effect=PermissionError):
                with redirect_stderr(error_output), redirect_stdout(io.StringIO()):
                    self.assertEqual(task15.main(), 1)

        self.assertIn("Permission denied", error_output.getvalue())

    def test_output_for_overflow(self):
        first = points([("-1.7e308", 0), ("-1.6e308", 0), ("-1.7e308", 1)])
        second = points([("1.6e308", 0), ("1.7e308", 0), ("1.6e308", 1)])
        error_output = io.StringIO()

        with patch.object(sys, "argv", ["task15.py", "input.txt"]):
            with patch.object(task15, "read_polygons", return_value=(first, second)):
                with redirect_stderr(error_output), redirect_stdout(io.StringIO()):
                    self.assertEqual(task15.main(), 1)

        self.assertIn("Overflow", error_output.getvalue())


if __name__ == "__main__":
    unittest.main()
