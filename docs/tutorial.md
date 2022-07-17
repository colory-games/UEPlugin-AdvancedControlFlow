# Tutorial

## MultiBranch

MultiBranch node realizes multiple conditional branches (like if-elseif-else statement).  
Comparision to the implementation on the vanila Unreal Engine is as follows.

@@@

### Usage

1. Search and place MultiBranch node on the Blueprint editor.
2. Click [Add Pin] to add a pin pair (condition and execution).
3. Build a logic by connecting among the nodes.

### Comparison to C++ code

@@@

Above Blueprint is same as below code in C++.

```cpp
if (Cond1) {
    Exec1();
} else if (Cond2) {
    Exec2();
} else {
    Default();
}
```

### Additional Info

* Some useful menu for adding/removing pins by right mouse click on the MultiBranch node.

## Conditional Sequence

MultiBranch node execute all relevant execution pins if the conditional pin is true.  
Comparision to the implementation on the vanila Unreal Engine is as follows.

@@@

### Usage

1. Search and place `Conditional Sequence` node on the Blueprint editor.
2. Click [Add Pin] to add a pin pair (condition and execution).
3. Build a logic by connecting among the nodes.

### Comparison to C++ code

@@@

Above Blueprint is same as below code in C++.

```cpp
if (Cond1) {
    Exec1();
}
if (Cond2) {
    Exec2();
}
Default();
```

### Additional Info

* Some useful menu for adding/removing pins by right mouse click on the Conditional Sequence node.