n1 = int(input('N for a = '))
a = int(input('a = '))
n2 = int(input('N for b = '))
b = int(input('b = '))

if n1 < 1 or n2 < 1 or len(str(abs(a))) > n1 or len(str(abs(b))) > n2:
    print('Invalid input')
else:
    print('a + b =', a + b)
    print('a - b =', a - b)
    print('a * b =', a * b)
