# Composition vs Inheritance

* Create a base class called `Shape` that has a method `draw()` that prints "Drawing a shape..."

* Create a derived class called `Circle` that inherits publicly from `Shape` and overrides the `draw()` method to print "Drawing a circle..." after calling the base class `draw()`

* Create another class called `Square` that has a private member variable of type `Shape`. Give `Square` a `draw()` method that calls `shape.draw()` and then prints "Drawing a square...".

This demonstrates:

* Inheritance - `Circle` inheriting behavior from the base `Shape` class and overriding part of it
* Composition - `Square` containing a member variable of `Shape` and reusing its behavior

Once implemented, compare creating objects of `Circle` and `Square` and how `draw()` behaves for each:

* `Circle` shows extending a base class via inheritance
* `Square` shows reusing a class's behavior via composition

* Observe the differences between inheritance and composition, including that composition allows a more flexible relationship between classes.
