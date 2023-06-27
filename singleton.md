# When Singletons go bad..

## Richard Shepherd

---

# Simple thread safe construction for a shared resource
`static` object is only constructed once

```cpp
class Manager1
{
public:
  static Manager1& Instance() {
    static Manager1 manager;
    return manager;
  }
  std::string const * GetResource() const {
```

<!---
In c++11 `static` was improved to have threadsafe initialization.
AKA the Scott Meyers Singleton for his popularization of it.
Much better than previous, platform specific initialization or static variables created at compile object scope.
-->

---

# Uncertain initialization?

```cpp
class Manager3
{
  Resource * resource_;
  Manager3(): resource_{CreateResource()}{}
  ~Manager3() {
    if (resource_)
      DestroyResource(resource_);
  }
public:
  static Manager3& Instance() {
    static Manager3 manager;
    return manager;
  }
  Resource const * GetResource() const {
```

<!---
A recent bug investigation eventually pointed the finger at a singleton that kept being glanced over because everyone knew it was safe.
The singleton was created (using a 3rdparty c-library call), the resource was accessed and the handle passed to another c-function, which internally was deallocating it, with a bang!
Once realized, it was an obvious hole, but how to spot, manage and avoid or recover from the problem - hidden inside the singleton constructor, which could only be tried once?
-->

---

# Only one chance for construction

```cpp
  if (const auto res = Manager3::Instance().GetResource())
    DoWork(res);
  if (const auto res = Manager3::Instance().GetResource())
    DoWork(res);
```

<!---
Even if we now checked for it on calls, the singleton had already been created and wouldn't retry getting the resource which might now be available
-->

---

# Fixes

- for each use: check `IsValid` and `ReInit`
- repeated creation with manual flag and mutex
- some more complex static which included constructing another static capturing the resource
- nothing that had the reliability, portability or elegance of static

---

# Re-examine static construction
`static` object only **completes** construction once.

.. and throwing an exception interrupts construction...

```cpp
class Manager4
{
  Manager4(): resource_{CreateResource()} {
    if (!resource_)
      throw std::exception{"Not ready"};
  }
public:
  static Manager4* Instance() {
    try {
      static Manager4 manager;
      return &manager;
    }
    catch(...) {
      return nullptr;
    }
```

<!---
I'm not quoting from the standard - just that team's evolving mental model
Note that the catch forces us to consider what to return in the absence of an instance,
which forces the change of the overall API
-->

---

# API encourages checking singleton's readiness

```cpp
  if (const auto mgr = Manager4::Instance())
    DoWork(mgr->GetResource());
  if (const auto mgr = Manager4::Instance())
    DoWork(mgr->GetResource());
```

<!---
.. but at the level of the singleton, not the more detailed internal objects
-->
---

# Takeaways

- Successful construction of an object doesn't mean it's useable
- Don't forget exceptions as part of the constructor/destructor toolkit
- 

