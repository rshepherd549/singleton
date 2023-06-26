#include <iostream>

template< typename T>
void DoWork(const T& handle) {
  std::cout << *handle << "\n";
}

class Manager1
{
public:
  static Manager1& Instance() {
    static Manager1 manager;
    return manager;
  }
  std::string const * GetResource() const {
    static std::string resource{"Hello!"};
    return &resource;
  }
};

//////////////////////

class Manager2
{
  std::unique_ptr<int> resource_;
  Manager2(): resource_{new int{42}}{}
public:
  static Manager2& Instance() {
    static Manager2 manager;
    return manager;
  }
  int const * GetResource() const {
    return resource_.get();
  }
};

//////////////////////

struct Resource{int value_{43};};
std::ostream& operator<<(std::ostream& os, const Resource& res){ return os << res.value_;}

Resource * CreateResource() {
  static int count = 0;
  return count++ < 1 ? nullptr : new Resource{};
}
void DestroyResource(Resource* res) {
  delete res;
}

class Manager3
{
  Resource * resource_;
  Manager3() {
    resource_ = CreateResource();
  }
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
    return resource_;
  }
};

//////////////////////

class Manager4
{
  Resource * resource_;
  Manager4() {
    resource_ = CreateResource();
    if (!resource_)
      throw std::exception{"Not ready"};
  }
  ~Manager4() {
    if (resource_)
      DestroyResource(resource_);
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
  }
  Resource const * GetResource() const {
    return resource_;
  }
};

//////////////////////

int main() {
  std::cout << "Start\n";

  //DoWork(Manager1::Instance().GetResource());

  //DoWork(Manager2::Instance().GetResource());

  //DoWork(Manager3::Instance().GetResource());

  //if (const auto res = Manager3::Instance().GetResource())
  //  DoWork(res);
  //if (const auto res = Manager3::Instance().GetResource())
  //  DoWork(res);

  if (const auto mgr = Manager4::Instance())
    DoWork(mgr->GetResource());
  if (const auto mgr = Manager4::Instance())
    DoWork(mgr->GetResource());

  std::cout << "Finish\n";
}
