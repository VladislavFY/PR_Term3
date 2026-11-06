TASK 15. MINIMUM DISTANCE BETWEEN TWO POLYGONS

Files:
task15.py       - the program;
polygons.txt    - a sample input file; the expected answer is 3;
test_task15.py  - automated tests;
README.txt     - these instructions.

Python 3.8 or newer is required. No additional packages are needed.
The program uses ordinary functions, loops and lists, without custom classes.

1. SUPPORTED POLYGONS

A polygon includes its boundary and interior. If the polygons intersect,
touch, or one lies inside the other, their distance is 0.
Both convex and concave simple polygons are supported. Holes are not supported.

The vertices of each polygon must follow its boundary in order.
Either clockwise or counterclockwise order is allowed. Unordered point sets
do not uniquely determine arbitrary polygons, so boundary order is part of
the input format.

2. INPUT FILE FORMAT

First write an integer n >= 3: the number of vertices of the first polygon.
Then write n lines, each containing exactly two numbers x y.
Next write an integer m >= 3 and m coordinate pairs for the second polygon.

Blank lines are allowed and ignored. Each vertex count must be on a separate
line. Separate coordinates with spaces or tabs. Use a decimal point.
Scientific notation is allowed: 1.5e3, -2e-7. Do not add comments to the file.

The program closes each polygon automatically, so repeating the first vertex
at the end is optional. If you repeat it, include that extra coordinate line
in n or m. The repeated closing vertex is removed before validation.

Contents of the sample polygons.txt file:

4
0 0
2 0
2 2
0 2

4
5 0
7 0
7 2
5 2

These are two squares. The distance between the right edge of the first
and the left edge of the second is 5 - 2 = 3.

3. RUNNING THE PROGRAM

Open a terminal in the program directory and run:

python3 task15.py polygons.txt

Expected output:

Minimum distance: 3

You can also run the program without an argument:

python3 task15.py

The program will ask for the file name. Relative and absolute paths, spaces,
non-Latin characters in file names, and paths starting with ~/ are supported.
When passing a path through the command line, put it in quotes if it contains
spaces:

python3 task15.py "My data/two polygons.txt"

When entering the name at the program prompt, outer quotes are optional.
The input must be a UTF-8 text file. UTF-8 with a BOM is also supported.
An error produces a readable message and exit code 1.
Successful calculation produces exit code 0.

All program comments, prompts, results and error messages are in English.
Test file names containing Cyrillic characters are intentional input data;
they verify support for non-Latin paths.

4. INPUT VALIDATION

The program checks whether the file exists and can be read, the file name,
text encoding, vertex counts, the number of coordinates on each line, and
extra data after the second polygon. An empty file is an error.

NaN, Infinity, inf, malformed numeric values and numbers outside the accepted
range are rejected. Zero coordinates are allowed.

For each polygon, the program checks repeated vertices, overlapping adjacent
edges, intersecting or touching nonadjacent edges, and zero area.
Extra vertices on a straight edge are allowed if the boundary does not
reverse direction along that edge. Two valid polygons may intersect:
their minimum distance is then 0.

5. NUMERIC RANGE AND OVERFLOW

The accepted range is based on finite float values:
the maximum magnitude is approximately 1.7976931348623157e308;
the smallest positive magnitude, including subnormal values, is approximately
4.9406564584124654e-324.
The program obtains the exact limits from sys.float_info rather than from
these rounded decimal descriptions. Range checks are exact.

Input coordinates are first read as Decimal and then converted to Fraction,
an exact rational number with an integer numerator and denominator.
For example, 0.1 is stored as 1/10 without first rounding it to float.
The range restriction applies to the magnitude of each nonzero coordinate.
The number of decimal places is not restricted to float precision.

Differences, products, intersection checks and squared distances are
calculated exactly. Large intermediate values do not become infinity, and
small values are not rounded to zero. No arbitrary EPS is needed for the
geometric predicates.

Before taking the square root, the final squared distance is checked.
If the distance exceeds the maximum finite float value, the program reports
overflow. If a positive distance is below the smallest positive float value,
it reports a separate error instead of returning zero.
Intermediate values may exceed the float range if the final distance fits.

The square root is computed with Decimal using 40 significant digits.
The result is displayed with 15 significant digits. It is not converted to
float, and its range is checked before rounding it for display.

6. ALGORITHM

1) Check whether a vertex of either polygon belongs to the other polygon.
   Cast a horizontal ray and count edge crossings. An odd number means the
   point is inside. Boundary membership is checked separately.
   If the point belongs to the other polygon, the distance is 0.

2) Iterate over all pairs of edges, one from each polygon.
   If two edges intersect or touch, the distance is 0.

3) For disjoint segments AB and CD, take the minimum of four distances:
   A to CD, B to CD, C to AB, and D to AB.
   Distances between vertices alone are insufficient: the nearest point
   may lie inside an edge at the foot of a perpendicular.

4) For the distance from P to AB, use v = B - A and w = P - A.
   If the dot product w*v <= 0, the nearest point is A.
   If w*v >= v*v, the nearest point is B.
   Otherwise, the perpendicular projection is inside AB, and the squared
   distance is:

   (v_x*w_y - v_y*w_x)^2 / (v_x^2 + v_y^2).

5) Find the minimum squared distance over all edge pairs, then take the
   square root once.

After validating the polygons, the distance algorithm needs O(n*m)
geometric checks. Self-intersection validation needs O(n^2 + m^2) checks.
The cost of rational arithmetic also depends on the length of the numeric
input, so this program is intended for educational data of moderate size.

7. AUTOMATED TESTS

python3 -m unittest -v test_task15.py

The tests cover ordinary distances, intersections, touching, containment,
concave polygons, boundary order, invalid data and geometry, large and small
coordinates, result overflow, file names and command-line execution.
Random rectangles are compared with an independent rectangle-distance formula.

Documentation for the Python standard-library tools used:
https://docs.python.org/3.12/library/fractions.html
https://docs.python.org/3.12/library/decimal.html
