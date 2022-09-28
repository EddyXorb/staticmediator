#include <array>
#include <memory>
#include <tuple>
#include <type_traits>

#include "iostream"

struct Mediator;

template <class T, class Tuple>
struct Index
{
   static constexpr auto value = std::numeric_limits<int>::min();
};

template <class T, class... Types>
struct Index<T, const std::tuple<T, Types...>>
{
   static const int value = 0;
};

template <class T, class U, class... Types>
struct Index<T, const std::tuple<U, Types...>>
{
   static const int value = 1 + Index<T, const std::tuple<Types...>>::value;
};

template <class Mediator>
class ISubscriber
{
public:
   // ISubscriber(Mediator* m) : mediator_(m) {}
   auto getMediator() -> Mediator& { return *mediator_; }
   void setMediator(Mediator& m) { mediator_ = &m; }

private:
   // friend class IMediator;
   Mediator* mediator_ = nullptr;
};

struct Subscriber1 : public ISubscriber<Mediator>
{
   int i = 99;
};

struct Subscriber2 : public ISubscriber<Mediator>
{
   std::string s = "subs2";
};

template <class Derived, typename... Subscriber>
class IMediator
{
protected:
   std::tuple<std::unique_ptr<Subscriber>...> subscriber_;

public:
   IMediator(Subscriber... subscriber)
   {
      initSubs<0>(subscriber...);
      updateSubscriberWithThis<0>();
   }
   // IMediator() { initSubs<0>(Subscriber(static_cast<Derived&>(*this))...); }

   IMediator(const IMediator& other) { updateSubscriberWithThis<0>(); }

   template <class Type>
   auto get() -> Type&
   {
      static constexpr int indexThisType = Index<Type, const std::tuple<Subscriber...>>::value;
      return *std::get<indexThisType>(subscriber_);
   }

protected:
   template <std::size_t index, class S1, class... SubscriberRest>
   void initSubs(S1 s1, SubscriberRest... rest)
   {
      std::get<index>(subscriber_) = std::make_unique<S1>(std::move(s1));
      initSubs<index + 1, SubscriberRest...>(rest...);
   }

   template <std::size_t index>
   void initSubs()
   {
   }

   template <std::size_t index, typename = void>
   auto updateSubscriberWithThis() -> typename std::enable_if<index >= (sizeof...(Subscriber)), void>::type
   {
   }

   template <std::size_t index>
       auto updateSubscriberWithThis() -> typename std::enable_if < index<(sizeof...(Subscriber)), void>::type
   {
      std::get<index>(subscriber_)->setMediator(static_cast<Derived&>(*this));
      updateSubscriberWithThis<index + 1>();
   }
};

struct Mediator : public IMediator<Mediator, Subscriber1, Subscriber2>
{
   Mediator(Subscriber1 s1, Subscriber2 s2) : IMediator(std::move(s1), std::move(s2)) {}

   auto& getS1() { return get<Subscriber1>(); }
   auto& getS2() { return get<Subscriber2>(); }
};

int main()
{
   auto m = Mediator(Subscriber1(), Subscriber2());
   std::cout << &m.getS1().i << "\n";
   std::cout << m.getS2().s << "\n";
   std::cout << &m.getS2().getMediator().getS1().i;
}