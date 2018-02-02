/*
 * Copyright 2018 Google
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Firestore/core/src/firebase/firestore/model/field_path.h"

#include <initializer_list>
#include <string>
#include <vector>

#include "gtest/gtest.h"

namespace firebase {
namespace firestore {
namespace model {

TEST(FieldPath, Constructors) {
  const FieldPath empty_path;
  EXPECT_TRUE(empty_path.empty());
  EXPECT_EQ(0, empty_path.size());
  EXPECT_TRUE(empty_path.begin() == empty_path.end());

  const FieldPath path_from_list = {"rooms", "Eros", "messages"};
  EXPECT_FALSE(path_from_list.empty());
  EXPECT_EQ(3, path_from_list.size());
  EXPECT_TRUE(path_from_list.begin() + 3 == path_from_list.end());

  std::vector<std::string> segments{"rooms", "Eros", "messages"};
  const FieldPath path_from_segments{segments.begin(), segments.end()};
  EXPECT_FALSE(path_from_segments.empty());
  EXPECT_EQ(3, path_from_segments.size());
  EXPECT_TRUE(path_from_segments.begin() + 3 == path_from_segments.end());

  FieldPath copied = path_from_list;
  EXPECT_EQ(path_from_list, copied);
  const FieldPath moved = std::move(copied);
  // Because FieldPath is immutable, move constructor performs a copy.
  EXPECT_EQ(copied, moved);
}

TEST(FieldPath, Indexing) {
  const FieldPath path{"rooms", "Eros", "messages"};

  EXPECT_EQ(path.front(), "rooms");
  EXPECT_EQ(path[0], "rooms");
  EXPECT_EQ(path.at(0), "rooms");

  EXPECT_EQ(path[1], "Eros");
  EXPECT_EQ(path.at(1), "Eros");

  EXPECT_EQ(path[2], "messages");
  EXPECT_EQ(path.at(2), "messages");
  EXPECT_EQ(path.back(), "messages");
}

TEST(FieldPath, DropFirst) {
  const FieldPath abc{"rooms", "Eros", "messages"};
  const FieldPath bc{"Eros", "messages"};
  const FieldPath c{"messages"};
  const FieldPath empty;
  const FieldPath abc_dupl{"rooms", "Eros", "messages"};

  EXPECT_NE(empty, c);
  EXPECT_NE(c, bc);
  EXPECT_NE(bc, abc);

  EXPECT_EQ(bc, abc.DropFirst());
  EXPECT_EQ(c, abc.DropFirst(2));
  EXPECT_EQ(empty, abc.DropFirst(3));
  EXPECT_EQ(abc_dupl, abc);
}

TEST(FieldPath, DropLast) {
  const FieldPath abc{"rooms", "Eros", "messages"};
  const FieldPath ab{"rooms", "Eros"};
  const FieldPath a{"rooms"};
  const FieldPath empty;
  const FieldPath abc_dupl{"rooms", "Eros", "messages"};

  EXPECT_EQ(ab, abc.DropLast());
  EXPECT_EQ(a, abc.DropLast().DropLast());
  EXPECT_EQ(empty, abc.DropLast().DropLast().DropLast());
}

TEST(FieldPath, Concatenation) {
  const FieldPath path;
  const FieldPath a{"rooms"};
  const FieldPath ab{"rooms", "Eros"};
  const FieldPath abc{"rooms", "Eros", "messages"};

  EXPECT_EQ(a, path.Concat("rooms"));
  EXPECT_EQ(ab, path.Concat("rooms").Concat("Eros"));
  EXPECT_EQ(abc, path.Concat("rooms").Concat("Eros").Concat("messages"));
  EXPECT_EQ(abc, path.Concat(FieldPath{"rooms", "Eros", "messages"}));
  EXPECT_EQ(abc, path.Concat({"rooms", "Eros", "messages"}));

  const FieldPath bcd{"Eros", "messages", "this_week"};
  EXPECT_EQ(bcd, abc.DropFirst().Concat("this_week"));
}

TEST(FieldPath, Comparison) {
  const FieldPath abc{"a", "b", "c"};
  const FieldPath abc2{"a", "b", "c"};
  const FieldPath xyz{"x", "y", "z"};
  EXPECT_EQ(abc, abc2);
  EXPECT_NE(abc, xyz);

  const FieldPath empty;
  const FieldPath a{"a"};
  const FieldPath b{"b"};
  const FieldPath ab{"a", "b"};

  EXPECT_TRUE(empty < a);
  EXPECT_TRUE(a < b);
  EXPECT_TRUE(a < ab);

  EXPECT_TRUE(a > empty);
  EXPECT_TRUE(b > a);
  EXPECT_TRUE(ab > a);
}

TEST(FieldPath, IsPrefixOf) {
  const FieldPath empty;
  const FieldPath a{"a"};
  const FieldPath ab{"a", "b"};
  const FieldPath abc{"a", "b", "c"};
  const FieldPath b{"b"};
  const FieldPath ba{"b", "a"};

  EXPECT_TRUE(empty.IsPrefixOf(empty));
  EXPECT_TRUE(empty.IsPrefixOf(a));
  EXPECT_TRUE(empty.IsPrefixOf(ab));
  EXPECT_TRUE(empty.IsPrefixOf(abc));
  EXPECT_TRUE(empty.IsPrefixOf(b));
  EXPECT_TRUE(empty.IsPrefixOf(ba));

  EXPECT_FALSE(a.IsPrefixOf(empty));
  EXPECT_TRUE(a.IsPrefixOf(a));
  EXPECT_TRUE(a.IsPrefixOf(ab));
  EXPECT_TRUE(a.IsPrefixOf(abc));
  EXPECT_FALSE(a.IsPrefixOf(b));
  EXPECT_FALSE(a.IsPrefixOf(ba));

  EXPECT_FALSE(ab.IsPrefixOf(empty));
  EXPECT_FALSE(ab.IsPrefixOf(a));
  EXPECT_TRUE(ab.IsPrefixOf(ab));
  EXPECT_TRUE(ab.IsPrefixOf(abc));
  EXPECT_FALSE(ab.IsPrefixOf(b));
  EXPECT_FALSE(ab.IsPrefixOf(ba));

  EXPECT_FALSE(abc.IsPrefixOf(empty));
  EXPECT_FALSE(abc.IsPrefixOf(a));
  EXPECT_FALSE(abc.IsPrefixOf(ab));
  EXPECT_TRUE(abc.IsPrefixOf(abc));
  EXPECT_FALSE(abc.IsPrefixOf(b));
  EXPECT_FALSE(abc.IsPrefixOf(ba));
}

TEST(FieldPath, AccessFailures) {
  const FieldPath path;
  ASSERT_DEATH_IF_SUPPORTED(path.front(), "");
  ASSERT_DEATH_IF_SUPPORTED(path.back(), "");
  ASSERT_DEATH_IF_SUPPORTED(path[0], "");
  ASSERT_DEATH_IF_SUPPORTED(path[1], "");
  ASSERT_DEATH_IF_SUPPORTED(path.at(0), "");
  ASSERT_DEATH_IF_SUPPORTED(path.DropFirst(), "");
  ASSERT_DEATH_IF_SUPPORTED(path.DropFirst(2), "");
  ASSERT_DEATH_IF_SUPPORTED(path.DropLast(), "");
}

TEST(FieldPath, Parsing) {
  const auto parse = [](const std::pair<std::string, size_t> expected) {
    const auto path = FieldPath::ParseServerFormat(expected.first);
    return std::make_pair(path.CanonicalString(), path.size());
  };
  const auto make_expected = [](const std::string& str, const size_t size) {
    return std::make_pair(str, size);
  };

  auto expected = make_expected("foo", 1);
  EXPECT_EQ(expected, parse(expected));
  expected = make_expected("foo.bar", 2);
  EXPECT_EQ(expected, parse(expected));
  expected = make_expected("foo.bar.baz", 3);
  EXPECT_EQ(expected, parse(expected));
  expected = make_expected(R"(`.foo\\`)", 1);
  EXPECT_EQ(expected, parse(expected));
  expected = make_expected(R"(`.foo\\`.`.foo`)", 2);
  EXPECT_EQ(expected, parse(expected));
  expected = make_expected(R"(foo.`\``.bar)", 3);
  EXPECT_EQ(expected, parse(expected));

  const auto path_with_dot = FieldPath::ParseServerFormat(R"(foo\.bar)");
  EXPECT_EQ(path_with_dot.CanonicalString(), "`foo.bar`");
  EXPECT_EQ(path_with_dot.size(), 1);
}

// This is a special case in C++: std::string may contain embedded nulls. To
// fully mimic behavior of Objective-C code, parsing must terminate upon
// encountering the first null terminator in the string.
TEST(FieldPath, ParseEmbeddedNull) {
  std::string str{"foo"};
  str += '\0';
  str += ".bar";

  const auto path = FieldPath::ParseServerFormat(str);
  EXPECT_EQ(path.size(), 1);
  EXPECT_EQ(path.CanonicalString(), "foo");
}

TEST(FieldPath, ParseFailures) {
  ASSERT_DEATH_IF_SUPPORTED(FieldPath::ParseServerFormat(""), "");
  ASSERT_DEATH_IF_SUPPORTED(FieldPath::ParseServerFormat("."), "");
  ASSERT_DEATH_IF_SUPPORTED(FieldPath::ParseServerFormat(".."), "");
  ASSERT_DEATH_IF_SUPPORTED(FieldPath::ParseServerFormat("foo."), "");
  ASSERT_DEATH_IF_SUPPORTED(FieldPath::ParseServerFormat(".bar"), "");
  ASSERT_DEATH_IF_SUPPORTED(FieldPath::ParseServerFormat("foo..bar"), "");
  ASSERT_DEATH_IF_SUPPORTED(FieldPath::ParseServerFormat(R"(foo\)"), "");
  ASSERT_DEATH_IF_SUPPORTED(FieldPath::ParseServerFormat(R"(foo.\)"), "");
  ASSERT_DEATH_IF_SUPPORTED(FieldPath::ParseServerFormat("foo`"), "");
  ASSERT_DEATH_IF_SUPPORTED(FieldPath::ParseServerFormat("foo```"), "");
  ASSERT_DEATH_IF_SUPPORTED(FieldPath::ParseServerFormat("`foo"), "");
}

TEST(FieldPath, CanonicalStringOfSubstring) {
  const auto path = FieldPath::ParseServerFormat("foo.bar.baz");
  EXPECT_EQ(path.CanonicalString(), "foo.bar.baz");
  EXPECT_EQ(path.DropFirst().CanonicalString(), "bar.baz");
  EXPECT_EQ(path.DropLast().CanonicalString(), "foo.bar");
  EXPECT_EQ(path.DropFirst().DropLast().CanonicalString(), "bar");
  EXPECT_EQ(path.DropFirst().DropLast().CanonicalString(), "bar");
  EXPECT_EQ(path.DropLast().DropFirst().DropLast().CanonicalString(), "");
}

TEST(FieldPath, CanonicalStringEscaping) {
  // Should be escaped
  EXPECT_EQ(FieldPath::ParseServerFormat("1").CanonicalString(), "`1`");
  EXPECT_EQ(FieldPath::ParseServerFormat("1ab").CanonicalString(), "`1ab`");
  EXPECT_EQ(FieldPath::ParseServerFormat("ab!").CanonicalString(), "`ab!`");
  EXPECT_EQ(FieldPath::ParseServerFormat("/ab").CanonicalString(), "`/ab`");
  EXPECT_EQ(FieldPath::ParseServerFormat("a#b").CanonicalString(), "`a#b`");

  // Should not be escaped
  EXPECT_EQ(FieldPath::ParseServerFormat("_ab").CanonicalString(), "_ab");
  EXPECT_EQ(FieldPath::ParseServerFormat("a1").CanonicalString(), "a1");
  EXPECT_EQ(FieldPath::ParseServerFormat("a_").CanonicalString(), "a_");
}

TEST(FieldPath, CreateKeyFieldPath) {
  const auto key_field_path = FieldPath::KeyFieldPath();
  EXPECT_EQ(key_field_path, FieldPath{key_field_path});
  EXPECT_EQ(key_field_path,
            FieldPath::ParseServerFormat(key_field_path.CanonicalString()));
  EXPECT_NE(key_field_path, FieldPath::ParseServerFormat(
                                key_field_path.CanonicalString().substr(1)));
}

}  // namespace model
}  // namespace firestore
}  // namespace firebase
