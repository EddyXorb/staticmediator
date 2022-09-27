#include <array>
#include <memory>
#include <tuple>

#include "iostream"

template <class Mediator>
class ISubscriber
{
public:
   ISubscriber(Mediator* m) : mediator_(m) {}
   auto getMediator() -> Mediator& { return *mediator_; }

private:
   // friend class Mediator;
   Mediator* mediator_ = nullptr;
};

struct Subscriber1
{
   int i = 99;
};

struct Subscriber2
{
   std::string s = "subs2";
};

template <typename... Subscriber>
class IMediator
{
protected:
   std::tuple<std::unique_ptr<Subscriber>...> subscriber_;

public:
   IMediator(Subscriber... subscriber) { initSubs<0>(subscriber...); }

   IMediator(const IMediator& other) { updateSubscriberWithThis<0>(Subscriber...); }

protected:
   template <std::size_t index,
             typename =
                 std::enable_if<index<(sizeof...(Subscriber))>::type> void initSubs(S1 s1, SubscriberRest... rest)
   {
      std::get<index>(subscriber_) = std::make_unique<S1>(std::move(s1));
      initSubs<index + 1, SubscriberRest...>(rest...);
   }

   template <std::size_t index>
   void initSubs()
   {
   }

   template <std::size_t index, class S1, class... SubscriberRest>
   void updateSubscriberWithThis(S1 s1, SubscriberRest... rest)
   {
      std::get<index>(subscriber_).mediator_ = this;
      updateSubscriberWithThis<index + 1, SubscriberRest...>(rest...);
   }

   template <std::size_t index>
   void updateSubscriberWithThis()
   {
   }
};

struct Mediator : public IMediator<Subscriber1, Subscriber2>
{
   Mediator(Subscriber1 s1, Subscriber2 s2) : IMediator(std::move(s1), std::move(s2)) {}

   auto getS1() -> Subscriber1& { return *std::get<0>(subscriber_); }
   auto getS2() -> Subscriber2& { return *std::get<1>(subscriber_); }
};

int main()
{
   auto m = Mediator(Subscriber1(), Subscriber2());
   std::cout << m.getS1().i << "\n";
   std::cout << m.getS2().s;
}