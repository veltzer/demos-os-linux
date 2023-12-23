# Static and Dynamic cast

* Create a base class called Shape and derived classes called Circle, Square, and Triangle that inherit from Shape. Give Shape a virtual method called getArea() that returns an area. Override this method in the derived classes to return the area specific to that shape.

* Next, create a function that takes a pointer to Shape called printArea. Inside this, call getArea() and print the result.

* Call printArea with objects of type Circle, Square, and Triangle. It should display the proper area for each when calling the virtual method on the Shape pointer.

* Now, try static casting the Shapes to various derived classes, then call the methods belonging to those classes. Note where it succeeds and fails.

* Then, try the equivalent dynamic casts. Note where these succeed or fail instead.

* Observe how static cast performs a compile time conversion, which may be valid or invalid, depending on how smart the compiler is at compile time, while dynamic cast will return NULL and fail if the object is not actually an instance of the type trying to cast to.

* Show where static cast fails to compile and where it succeeds.

* Show where dynamic cast fails to compile and where it succeeds.

* Analyze where each is appropriate and the tradeoffs of using one over the other. This will demonstrate the distinction between `static_cast` and `dynamic_cast` and when to properly use each.
