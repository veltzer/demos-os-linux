# Singleton exercise

* Write a singleton class.
* Make sure only one instance of your object can be created (make the constructor private).
* Add a static method `get_instance` to get your instance.
* Make your `get_instance` lazy.
* Use a static member to store the instance.
* Use a pointer to store the instance and not a reference.

## Stage 2

* Use reference and not a pointer.

## Stage 3

* make sure your solution can be used in a multi-threaded context.

## Stage 4

* Delete the copy constructor and operator= so that the user will not be able to make a copy of your singleton.
