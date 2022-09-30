#include <array>
#include <memory>
#include <tuple>
#include <type_traits>

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

template <class Derived, typename... Subscriber>
class IMediator;

template <class Mediator>
class ISubscriber
{
protected:
   auto getMediator() -> Mediator&
   {
      if (mediator_ == nullptr)
         throw std::runtime_error("Uninitialized mediator in Subscriber!");
      else
         return *mediator_;
   }

   auto getMediator() const -> const Mediator&
   {
      if (mediator_ == nullptr)
         throw std::runtime_error("Uninitialized mediator in Subscriber!");
      else
         return *mediator_;
   }

private:
   template <class Derived, typename... Subscriber>
   friend class IMediator;
   Mediator* mediator_ = nullptr;

   void setMediator(Mediator& m) { mediator_ = &m; }
};

template <class Derived, typename... Subscriber>
class IMediator
{
private:
   std::unique_ptr<std::tuple<Subscriber...>> subscriber_;

public:
   friend void swap(Derived& l, Derived& r) noexcept
   {
      using std::swap;
      swap(l.subscriber_, r.subscriber_);
      l.template updateSubscriberWithThis<0>();
      r.template updateSubscriberWithThis<0>();
   }

   auto operator=(const IMediator& r) -> IMediator&
   {
      *subscriber_ = *r.subscriber_;
      updateSubscriberWithThis<0>();
      return *this;
   }

   IMediator(Subscriber... subscriber)
       : subscriber_(std::make_unique<std::tuple<Subscriber...>>(std::make_tuple(std::move(subscriber)...)))
   {
      updateSubscriberWithThis<0>();
   }

   template <class Type>
   auto get() -> Type&
   {
      static constexpr int indexThisType = Index<Type, const std::tuple<Subscriber...>>::value;
      return std::get<indexThisType>(*subscriber_);
   }
   
   template <class Type>
   auto get() const -> const Type&
   {
      static constexpr int indexThisType = Index<Type, const std::tuple<Subscriber...>>::value;
      return std::get<indexThisType>(*subscriber_);
   }

   IMediator(const IMediator& other) : subscriber_(std::make_unique<std::tuple<Subscriber...>>(*other.subscriber_))
   {
      updateSubscriberWithThis<0>();
   }

   IMediator(IMediator&& other) noexcept : subscriber_(std::move(other.subscriber_)) { updateSubscriberWithThis<0>(); }

private:
   template <std::size_t index, typename = void>
   auto updateSubscriberWithThis() -> typename std::enable_if<index >= (sizeof...(Subscriber)), void>::type
   {
   }

   template <std::size_t index>
       auto updateSubscriberWithThis() -> typename std::enable_if < index<(sizeof...(Subscriber)), void>::type
   {
      std::get<index>(*subscriber_).setMediator(static_cast<Derived&>(*this));
      updateSubscriberWithThis<index + 1>();
   }
};
