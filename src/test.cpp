
#include "iostream"

#include "gtest/gtest.h"

#include "staticmediator.hpp"

struct Mediator;

struct Subscriber1 : public ISubscriber<Mediator>
{
   int i = 99;
   using ISubscriber<Mediator>::getMediator;  // to make it public
};

struct Subscriber2 : public ISubscriber<Mediator>
{
   std::string s = "subs2";
   using ISubscriber<Mediator>::getMediator;
};

struct Mediator : public IMediator<Mediator, Subscriber1, Subscriber2>
{
   using Base = IMediator<Mediator, Subscriber1, Subscriber2>;
   Mediator(Subscriber1 s1, Subscriber2 s2) : Base(std::move(s1), std::move(s2)) {}

   auto& getS1() { return get<Subscriber1>(); }
   auto& getS2() { return get<Subscriber2>(); }
};

class StaticMediatorTester : public ::testing::Test
{
public:
   StaticMediatorTester() : m(Subscriber1(), Subscriber2()), m2(Subscriber1(), Subscriber2())
   {
      m.getS1().i = 1;
      m.getS2().s = "m";
      m2.getS1().i = 2;
      m2.getS2().s = "m2";
   }
   Mediator m;
   Mediator m2;
};

TEST_F(StaticMediatorTester, BasicUsagesWork)
{
   EXPECT_EQ(&m.getS1().i, &m.getS2().getMediator().getS1().i);
}

TEST_F(StaticMediatorTester, getMediatorReturnsPointerToThis)
{
   EXPECT_EQ(&m.getS1().getMediator(), &m);
   EXPECT_EQ(&m.getS2().getMediator(), &m);
}

TEST_F(StaticMediatorTester, swapWorks)
{
   EXPECT_EQ(m.getS1().i, 1);
   EXPECT_EQ(m2.getS1().i, 2);

   using std::swap;
   swap(m, m2);

   EXPECT_EQ(m.getS1().i, 2);
   EXPECT_EQ(m2.getS1().i, 1);
}
TEST_F(StaticMediatorTester, constConstructedDoesNotPointToOldMediator)
{
   Mediator m3(m);
   EXPECT_EQ(&m3.getS1().i, &m3.getS2().getMediator().getS1().i);
}

TEST_F(StaticMediatorTester, AssignedDoesNotPointToOldMediator)
{
   EXPECT_EQ(m2.getS1().i, 2);

   m2 = m;

   EXPECT_EQ(m2.getS1().i, 1);

   EXPECT_EQ(&m.getS1().i, &m.getS2().getMediator().getS1().i);
   EXPECT_EQ(m2.getS1().i, m.getS1().i);
   EXPECT_EQ(m2.getS2().s, m.getS2().s);
}

TEST_F(StaticMediatorTester, MoveOperatorDoesNotPointToOldMediator)
{
   Mediator m3(std::move(m));
   EXPECT_EQ(&m3.getS1().i, &m3.getS2().getMediator().getS1().i);
}