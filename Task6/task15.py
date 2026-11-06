import re
import sys
from decimal import Decimal, InvalidOperation, localcontext
from fractions import Fraction
from pathlib import Path


MAX_NUMBER = Decimal.from_float(sys.float_info.max)
MIN_NUMBER = Decimal.from_float(sys.float_info.min * sys.float_info.epsilon)
NUMBER_PATTERN = r"[+-]?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)(?:[eE][+-]?[0-9]+)?"


def read_number(text, line_number):
    try:
        number = Decimal(text)
    except InvalidOperation:
        raise ValueError(f"Line {line_number}: invalid number {text!r}.") from None

    if not number.is_finite():
        raise ValueError(f"Line {line_number}: NaN and infinity are not allowed.")

    if re.fullmatch(NUMBER_PATTERN, text) is None:
        raise ValueError(f"Line {line_number}: invalid numeric format {text!r}.")

    absolute = number.copy_abs()

    if absolute > MAX_NUMBER:
        raise ValueError(f"Line {line_number}: the number exceeds the maximum float value.")

    if number != 0 and absolute < MIN_NUMBER:
        raise ValueError(f"Line {line_number}: the nonzero number is too small for float.")

    if number == 0:
        return Fraction(0)

    # Preserve the original decimal value without rounding it to float.
    return Fraction(number)


def cross(a, b, c):
    return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])


def point_on_segment(point, a, b):
    return (cross(a, b, point) == 0
            and min(a[0], b[0]) <= point[0] <= max(a[0], b[0])
            and min(a[1], b[1]) <= point[1] <= max(a[1], b[1]))


def segments_intersect(a, b, c, d):
    first = cross(a, b, c)
    second = cross(a, b, d)
    third = cross(c, d, a)
    fourth = cross(c, d, b)

    if first * second < 0 and third * fourth < 0:
        return True

    return (point_on_segment(c, a, b)
            or point_on_segment(d, a, b)
            or point_on_segment(a, c, d)
            or point_on_segment(b, c, d))


def check_polygon(points, polygon_number):
    # Allow the first vertex to be repeated at the end of the list.
    if len(points) > 1 and points[0] == points[-1]:
        points = points[:-1]

    n = len(points)
    prefix = f"Polygon {polygon_number}: "

    if n < 3:
        raise ValueError(prefix + "at least three distinct vertices are required.")

    if len(set(points)) != n:
        raise ValueError(prefix + "vertices are repeated.")

    for i in range(n):
        a = points[i - 1]
        b = points[i]
        c = points[(i + 1) % n]

        first_x = a[0] - b[0]
        first_y = a[1] - b[1]
        second_x = c[0] - b[0]
        second_y = c[1] - b[1]

        if cross(a, b, c) == 0 and first_x * second_x + first_y * second_y > 0:
            raise ValueError(prefix + "adjacent edges overlap.")

    for i in range(n):
        for j in range(i + 1, n):
            if j == i + 1 or (i == 0 and j == n - 1):
                continue

            if segments_intersect(points[i], points[(i + 1) % n],
                                  points[j], points[(j + 1) % n]):
                raise ValueError(prefix + "nonadjacent edges intersect or touch.")

    area = 0

    for i in range(n):
        a = points[i]
        b = points[(i + 1) % n]
        area += a[0] * b[1] - a[1] * b[0]

    if area == 0:
        raise ValueError(prefix + "the area is zero.")

    return points


def read_polygon(lines, position, polygon_number):
    if position >= len(lines):
        raise ValueError(f"Data for polygon {polygon_number} are missing.")

    line_number, text = lines[position]

    if re.fullmatch(r"[0-9]+", text) is None:
        raise ValueError(f"Line {line_number}: an integer vertex count is expected.")

    try:
        n = int(text)
    except ValueError:
        raise ValueError(f"Line {line_number}: the vertex count contains too many digits.") from None

    if n < 3:
        raise ValueError(f"Line {line_number}: the vertex count must be at least 3.")

    position += 1

    if n > len(lines) - position:
        raise ValueError(f"Not enough coordinates for polygon {polygon_number}.")

    points = []

    for i in range(n):
        line_number, text = lines[position]
        values = text.split()

        if len(values) != 2:
            raise ValueError(f"Line {line_number}: exactly two coordinates x y are expected.")

        x = read_number(values[0], line_number)
        y = read_number(values[1], line_number)
        points.append((x, y))
        position += 1

    return check_polygon(points, polygon_number), position


def read_polygons(file_name):
    lines = []

    with open(file_name, "r", encoding="utf-8-sig") as file:
        for line_number, line in enumerate(file, 1):
            text = line.strip()

            if text:
                lines.append((line_number, text))

    if not lines:
        raise ValueError("The file is empty.")

    first, position = read_polygon(lines, 0, 1)
    second, position = read_polygon(lines, position, 2)

    if position != len(lines):
        raise ValueError(f"Line {lines[position][0]}: extra data after the second polygon.")

    return first, second


def point_inside(point, polygon):
    inside = False
    n = len(polygon)

    for i in range(n):
        a = polygon[i]
        b = polygon[(i + 1) % n]

        if point_on_segment(point, a, b):
            return True

        if (a[1] > point[1]) != (b[1] > point[1]):
            x = a[0] + (point[1] - a[1]) * (b[0] - a[0]) / (b[1] - a[1])

            if point[0] < x:
                inside = not inside

    return inside


def point_distance_squared(point, a, b):
    dx = b[0] - a[0]
    dy = b[1] - a[1]
    px = point[0] - a[0]
    py = point[1] - a[1]

    length_squared = dx * dx + dy * dy
    projection = px * dx + py * dy

    if length_squared == 0 or projection <= 0:
        return px * px + py * py

    if projection >= length_squared:
        px = point[0] - b[0]
        py = point[1] - b[1]
        return px * px + py * py

    # The perpendicular projection lies inside the segment.
    product = dx * py - dy * px
    return product * product / length_squared


def segment_distance_squared(a, b, c, d):
    if segments_intersect(a, b, c, d):
        return Fraction(0)

    return min(point_distance_squared(a, c, d),
               point_distance_squared(b, c, d),
               point_distance_squared(c, a, b),
               point_distance_squared(d, a, b))


def polygon_distance_squared(first, second):
    if point_inside(first[0], second) or point_inside(second[0], first):
        return Fraction(0)

    answer = None
    n = len(first)
    m = len(second)

    for i in range(n):
        for j in range(m):
            current = segment_distance_squared(first[i], first[(i + 1) % n],
                                               second[j], second[(j + 1) % m])

            if current == 0:
                return Fraction(0)

            if answer is None or current < answer:
                answer = current

    return answer


def get_distance(squared):
    maximum = Fraction(MAX_NUMBER)
    minimum = Fraction(MIN_NUMBER)

    if squared > maximum * maximum:
        raise OverflowError("The distance exceeds the maximum finite float value.")

    if 0 < squared < minimum * minimum:
        raise ArithmeticError("A positive distance is too small for float; it must not be rounded to 0.")

    # Compute the square root only once, after finding the minimum.
    with localcontext() as context:
        context.prec = 40
        result = (Decimal(squared.numerator) / Decimal(squared.denominator)).sqrt()

    if not result.is_finite():
        raise OverflowError("A non-finite result was produced.")

    return result


def prepare_filename(text):
    file_name = text.strip()

    if len(file_name) >= 2 and file_name[0] in ("'", '"') and file_name[-1] == file_name[0]:
        file_name = file_name[1:-1]

    if not file_name:
        raise ValueError("The file name must not be empty.")

    if "\0" in file_name:
        raise ValueError("The file name contains an invalid null character.")

    try:
        return Path(file_name).expanduser()
    except RuntimeError:
        raise ValueError("Cannot determine the home directory for a path starting with ~.") from None


def main():
    try:
        if len(sys.argv) > 2:
            raise ValueError('Usage: python3 task15.py "file name". Put paths containing spaces in quotes.')

        if len(sys.argv) == 2:
            text = sys.argv[1]
        else:
            text = input("Enter the file name: ")

        file_name = prepare_filename(text)
        first, second = read_polygons(file_name)
        squared = polygon_distance_squared(first, second)
        answer = get_distance(squared)

        print("Minimum distance:", format(answer, ".15g"))
        return 0

    except FileNotFoundError as error:
        message = f"File not found: {error.filename}"
    except IsADirectoryError:
        message = "The path refers to a directory; a file is required."
    except PermissionError:
        message = "Permission denied: the file cannot be read."
    except UnicodeError:
        message = "The file must contain UTF-8 text."
    except OverflowError as error:
        message = "Overflow during calculation. " + str(error)
    except ArithmeticError as error:
        message = "Calculation error: " + str(error)
    except ValueError as error:
        message = str(error)
    except OSError as error:
        message = f"Cannot open or read the file (OS error code {error.errno})."
    except MemoryError:
        message = "Not enough memory to process the data."
    except (EOFError, KeyboardInterrupt):
        message = "Input cancelled."

    print("Error:", message, file=sys.stderr)
    return 1


if __name__ == "__main__":
    sys.exit(main())
