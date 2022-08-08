# Tutorial

**Introduction Video**

[![](https://img.youtube.com/vi/c9moKtYbnvA/0.jpg)](https://www.youtube.com/watch?v=c9moKtYbnvA)

## MultiBranch

MultiBranch node realizes multiple conditional branches (like if-elseif-else statement).  
Comparision to the implementation on the vanilla Unreal Engine is as follows.

![Vanila vs MultiBranch](images/tutorial/vanilla_vs_multibranch.png)

### Usage

1. Search and place MultiBranch node on the Blueprint editor.
2. Click [Add Pin] to add a pin pair (condition and execution).
3. Build a logic by connecting among the nodes.

### Comparison to C++ code

![MultiBranch](images/tutorial/multibranch.png)

Above Blueprint is same as below code in C++.

```cpp
if (Condition_0) {
    UKismetSystemLibrary::PrintString(GEngine->GetWorld(), "Condition 0");
} else if (Condition_1) {
    UKismetSystemLibrary::PrintString(GEngine->GetWorld(), "Condition 1");
} else {
    UKismetSystemLibrary::PrintString(GEngine->GetWorld(), "Default");
}
```

### Additional Info

* Some useful menu for adding/removing pins by right mouse click on the MultiBranch node.

## Conditional Sequence

Conditional Sequence node execute each relevant execution pins if each conditional pin is true.  
Comparision to the implementation on the vanilla Unreal Engine is as follows.

![Vanila vs Conditional Sequence](images/tutorial/vanilla_vs_conditional-sequence.png)

### Usage

1. Search and place `Conditional Sequence` node on the Blueprint editor.
2. Click [Add Pin] to add a pin pair (condition and execution).
3. Build a logic by connecting among the nodes.

### Comparison to C++ code

![Conditional Sequence](images/tutorial/conditional-sequence.png)

Above Blueprint is same as below code in C++.

```cpp
if (Condition_0) {
    UKismetSystemLibrary::PrintString(GEngine->GetWorld(), "Condition 0");
}
if (Condition_1) {
    UKismetSystemLibrary::PrintString(GEngine->GetWorld(), "Condition 1");
}
UKismetSystemLibrary::PrintString(GEngine->GetWorld(), "Default");
```

### Additional Info

* Some useful menu for adding/removing pins by right mouse click on the Conditional Sequence node.
