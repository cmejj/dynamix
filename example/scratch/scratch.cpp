// DynaMix
// Copyright (c) 2013-2017 Borislav Stanimirov, Zahary Karadjov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include <dynamix/dynamix.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../../test/doctest/doctest.h"

TEST_SUITE("bids");

using namespace dynamix;
using namespace std;

DYNAMIX_DECLARE_MIXIN(a);
DYNAMIX_DECLARE_MIXIN(b);
DYNAMIX_DECLARE_MIXIN(c);
DYNAMIX_DECLARE_MIXIN(d);

DYNAMIX_MESSAGE_1(void, bids_uni, std::ostream&, out);
DYNAMIX_CONST_MESSAGE_1(void, bids_bad_uni, std::ostream&, out);
DYNAMIX_MULTICAST_MESSAGE_1(void, bids_multi, std::ostream&, out);
DYNAMIX_CONST_MULTICAST_MESSAGE_1(void, bids_multi_override, std::ostream&, out);

TEST_CASE("bids")
{
    object o;

    mutate(o)
        .add<a>()
        .add<b>()
        .add<c>()
        .add<d>();

    ostringstream sout;
    bids_uni(o, sout);
    CHECK(sout.str() == "abc");

    CHECK_THROWS_AS(bids_bad_uni(o, sout), bad_next_bidder_call);

    sout.str(string());
    bids_multi_override(o, sout);
    CHECK(sout.str() == "cd");

    sout.str(string());
    bids_multi(o, sout);
    CHECK(sout.str() == "bacd");

    mutate(o)
        .remove<c>()
        .remove<d>();

    CHECK_THROWS_AS(bids_uni(o, sout), bad_next_bidder_call);

    sout.str(string());
    bids_multi_override(o, sout);
    CHECK(sout.str() == "ab");

    sout.str(string());
    bids_multi(o, sout);
    CHECK(sout.str() == "ba");
}

class a
{
public:
    void bids_uni(std::ostream& out)
    {
        out << "a";
        CHECK(DYNAMIX_HAS_NEXT_BIDDER(bids_uni_msg));
        DYNAMIX_CALL_NEXT_BIDDER(bids_uni_msg, out);
    }

    void bids_bad_uni(std::ostream& out) const
    {
        out << "a";
        CHECK(!DYNAMIX_HAS_NEXT_BIDDER(bids_bad_uni_msg));
        DYNAMIX_CALL_NEXT_BIDDER(bids_bad_uni_msg, out);
    }

    void bids_multi(std::ostream& out)
    {
        out << "a";
    }

    void bids_multi_override(std::ostream& out) const
    {
        out << "a";
    }
};

class b
{
public:
    void bids_uni(std::ostream& out)
    {
        out << "b";
        DYNAMIX_CALL_NEXT_BIDDER(bids_uni_msg, out);
    }

    void bids_bad_uni(std::ostream& out) const
    {
        out << "b";
    }

    void bids_multi(std::ostream& out)
    {
        out << "b";
        CHECK(DYNAMIX_HAS_NEXT_BIDDER(bids_multi_msg));
        DYNAMIX_CALL_NEXT_BIDDER(bids_multi_msg, out);
    }

    void bids_multi_override(std::ostream& out) const
    {
        out << "b";
    }
};

class c
{
public:
    void bids_uni(std::ostream& out)
    {
        out << "c";
        CHECK(!DYNAMIX_HAS_NEXT_BIDDER(bids_uni_msg));
    }

    void bids_multi(std::ostream& out)
    {
        out << "c";
    }

    void bids_multi_override(std::ostream& out) const
    {
        out << "c";
    }
};

class d
{
public:
    void bids_uni(std::ostream& out)
    {
        out << "d";
    }

    void bids_multi(std::ostream& out)
    {
        out << "d";
    }

    void bids_multi_override(std::ostream& out) const
    {
        out << "d";
    }
};

// this order should be important if the messages aren't sorted by mixin name
DYNAMIX_DEFINE_MIXIN(b, priority(1, bid(1, bids_uni_msg)) & priority(-1, bids_bad_uni_msg) & bids_multi_override_msg & bid(1, bids_multi_msg));
DYNAMIX_DEFINE_MIXIN(d, bids_uni_msg & bid(1, bids_multi_override_msg) & bid(1, bids_multi_msg));
DYNAMIX_DEFINE_MIXIN(a, bid(2, priority(1, bids_uni_msg)) & bids_bad_uni_msg & bids_multi_override_msg & bids_multi_msg);
DYNAMIX_DEFINE_MIXIN(c, priority(1, bids_uni_msg) & bid(1, bids_multi_override_msg) & bids_multi_msg);

DYNAMIX_DEFINE_MESSAGE(bids_uni);
DYNAMIX_DEFINE_MESSAGE(bids_bad_uni);
DYNAMIX_DEFINE_MESSAGE(bids_multi);
DYNAMIX_DEFINE_MESSAGE(bids_multi_override);
