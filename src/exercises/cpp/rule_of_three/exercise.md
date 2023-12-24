# Rule of Three

* Implment a class called `IntVector` which implements a vector of integers of any size.

* Some methods can be (these are *not* all the methods):

```c++
    // Constructor
    IntVector(int size);

    // getting data
    int get_value_at(int location);
    int get_length(void);

    void append(int value);
    void remove(int location);
```

* Do not worry about performance.

* Follow the Rule of Three.

* Demonstrate that your class works and demonstrate what the Rule of Three means for copying the object and assigning to it.

* Add an iterator for your class and make sure that your instances cannot be changed while being iterated. 

* Demontrate that by writing code that tries to modify your instances while iterating them and fails.
