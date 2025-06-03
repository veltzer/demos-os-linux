# Boost Pool Library exercise

One trick in memory allocation is not to allocate memory at runtime but instead
allocate a pool of `N` objects at startup time, and making sure that I never use
more than `N` objects simultaneously.

First create class of type `Person` with various getters\setters for `name`, `age`, etc.

Now use Boost::Pool to create a pool of 256 People at startup time and show how you get
a hold of a Person object by getting one from the pool and how to return one to the pool.

here is rough code for how this should work:
```C++
Pool<Person> p(count=100);
Person* p1=p.get();
// do something with p1 of type Person
// return to the pool
p.put(p1)
```
