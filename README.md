# Static Mediator
## Basic Concept
Sometimes one wants to let interact several Objects whose types are known at compiletime with each other using the mediator pattern. 
This little header-only framework lets you create such settings. 
It can also come in handy as a low-cost(not no-cost however) abstraction to refactor big classes in a way that parts of these classes go into subclasses that still have access to the main class and all other subclasses.

## How to use
Derive the Mediator using the CRPT-Pattern from IMediatior such as shown in test.cpp. Derive every Subscriber from ISubscriber using a forward declaration of the derived Mediator. The test.cpp contains a fully working minimal example.
Then define accessors in the mediator if you don't want to use the predefined getter every time. For the subscribers: either make a using declaration such as
```cpp
using Base = ISubscriber<...>;
using Base::getMediator;
```
or create a delegator function as the *getMediator* from the Baseclass is not public.
## Background
Every Subscriber has stored a pointer to the Mediator it is part of. The subscribers are stored in a tuple that is wrapped into a unique_ptr. So far every Subscriber should be of a different type, otherwise the IMediator getter will not work correctly as it returns only the first occurence of the requested type
such as 
```cpp
m.get<Subscriber2>();
```
If this gets problematic then it can be easily fixed by changing the subscribers member *subscriber_* as protected and define accessors to the needed tuple using *std::get*.

## Advantages of this approach
* absolute lifetime safety of subscribers since they are embedded into the mediator
* PIMPL idiom avoids complete rebuilds when one subclass is changed
* copy, swap and assignment work without having to define custom functions


