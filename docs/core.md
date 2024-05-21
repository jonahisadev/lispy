# Core Library

## Description

There's a list of internal functions that are implemented in native C++ code.
Most are related to lists and vectors. Someday these could potentially be written
in Lispy itself, but the other nice thing about it being native code is that
it's _really_ fast.

## Operators

The operators are implemented in native code and have a shorthand associated with
the function names.

_TODO: implement_

## Functions

### `sqrt/1`

```clojure
(defn sqrt [x:number] ...)
```

Square root the given number. Will be rounded down due to lack of floating point
number support in Lispy.

```clojure
(sqrt 25)
;; => 5
(sqrt 24)
;; => 4
```

### `map/2`

```clojure
(defn map [operator:fn(el) data:vec] ...)
```

Takes a given vector and iterates through each element, passing that element
to the operator function, and returning the result as a new vector.

```clojure
(defn square [n] (* n n))
(map square [1 2 3 4 5])
;; => [ 1 4 9 16 25 ]
```

### `range/2`

```clojure
(defn range [first:number last:number] ...)
```

Creates a vector with numbers between `first` and `last` (inclusive).

```clojure
(range 1 5)
;; => [ 1 2 3 4 5 ]
```

### `size/1`

```clojure
(defn size [data:vec] ...)
```

Returns the length of a given vector.

```clojure
(defn my_data [0 1 2])
(size my_data)
;; => 3
```

### `len/1`

```clojure
(defn len [data:list] ...)
```

Returns the length of a given list. Similar to `size/1`. Because linked-lists
are the selected implementation of Lispy lists, it is required that the entire
list be traversed in order to determine the length. This will be costly for
larger lists.

```clojure
(defn my_list '(0 1 2))
(len my_list)
;; => 3
```

### `head/1`

```clojure
(defn head [data:list] ...)
```

Returns the first item in a list.

```clojure
(defn my_list '(0 1 2))
(head my_list)
;; => 0
```

### `tail/1`

```clojure
(defn tail [data:list] ...)
```

Returns everything after the head of a list.

```clojure
(defn my_list '(0 1 2))
(head my_list)
;; => ( 1 2 )
```

### `nth/2`

```clojure
(defn nth [idx:number data:list] ...)
```

Gets the `n`th element in a list. Again, due to linked-list implementations,
this operation is O(n).

```clojure
(defn my_list '(5 10 15))
(nth 1 my_list)
;; => 10
```

### `rem/2`

```clojure
(defn rem [dividend:number divisor:number] ...)
```

Divides dividend by divisor and returns the whole number remainder.

```clojure
(rem 50 10)
;; => 0
(rem 50 11)
;; => 6 (4 and 6/11)
```

### `filter/2`

```clojure
(defn filter [op:fn(el) data:vec] ...)
```

Takes a given vector and iterates through each element, passing that element
to the operator function, and returning the element in a new vector only if the 
operator function returns true.

```clojure
(defn more_than_one_digit [n] (> n 9))
(filter more_than_one_digit [ 4 9 18 24 5 17 ])
;; => [ 18 24 17 ]
(defn square [n] (* n n))
(filter more_than_one_digit (map square [ 1 2 3 4 5 ]))
;; => [ 16 25 ]
```

### `reduce/2`

```clojure
(defn reduce [op:fn(prev,curr) data:vec] ...)
```

Takes a given vector and iterates through each element. The operator function
is given the previous and current values where previous is the last result
of the operator function, and current is the current element value.

Due to a currently poor implementation, the loop actually starts on the second
element, with previous being the first element, and current being the second.
In the future, an optional argument will be taken to set the starting value, and
then starting the loop from the 0th element.

```clojure
(defn vec_sum [prev curr] (+ prev curr))
(reduce vec_sum [1 2 3 4])
;; => 10
```
