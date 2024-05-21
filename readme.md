# Lispy

Lispy is a toy programming language. It is an interpreted language, and uses a
LISP-like dialect. I mostly wrote this as a proof-of-concept and as a challenge
to myself.

I am publishing it publicly on GitHub for portfolio purposes, as well as
educational for anyone who may come across it. I would of course not recommend
that anyone seriously use it for anything other than playing around or learning.

If you like the idea of using a LISP-style programming language, please take a
look at [Clojure](https://github.com/clojure/clojure).

## How to use

_For information regarding the "core library" please visit [docs/core.md](docs/core.md)._

### Building

```bash
$ mkdir .build
$ meson setup .build
$ cd .build && ninja
```

You should have an executable called `lisp` ready to use!

### Basic Usage

Running `lisp` will open a [REPL](https://en.wikipedia.org/wiki/Read%E2%80%93eval%E2%80%93print_loop)
where you can write Lispy code. State is maintained throughout the session until
an EOL (Ctrl+D) signal is sent to the program, at which point it will exit.

Here are some example inputs to help you get started:

```clojure
(+ 1 2)
;; => 3
```

```clojure
(defn square [x] (* x x))
(square 5)
;; => 25
```

```clojure
(defn sum [a b] (+ a b))
(sum 12 30)
;; => 42
```

```clojure
(defn square [x] (* x x))
(map square [1 2 3])
;; => [ 1 4 9 ]
```

```clojure
(defn more_than_one_digit [n] (> n 9))
(filter more_than_one_digit [4 9 18 42 3])
;; => [ 18 42 ]
```

```clojure
(filter more_than_one_digit (map square [1 2 3 4 5]))
;; => [ 16 25 ]
```

```clojure
(def the_answer 42)
(if (= the_answer 42) (true) (false))
;; => true
```

Right now, the interpreter does not allow multiline input. I may add that in the
future because the below program is about as complex as one can reasonably program
in one line. Also loading a file would be nice.

```clojure
(defn game [guess answer] (if (= answer guess) (0) (if (> guess answer) (1) (-1))))
(game 50 42)
;; => 1 (too high)
(game 24 42)
;; => -1 (too low)
(game 42 42)
;; => 0 (just right!)
```

### Summary

There are some cool things that the interpreter can do! This is due to the fact
that the parser is really easy to build because of the simplistic nature of
LISP-style code, but also because everything is just a stack. Outside of a key-
value store for globals, it's all the stack. Which makes it super easy to reuse
code all over the place because you really only need one implementation
everywhere.
