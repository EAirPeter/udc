# C++17 skeleton for Bison

# Copyright (C) 2002-2015 Free Software Foundation, Inc.

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


## -------------------------- ##
## using C++17 std::variant.  ##
## -------------------------- ##

# b4_symbol_variant(YYTYPE, YYVAL, ACTION, [ARGS])
# ------------------------------------------------
# Run some ACTION on YYVAL of symbol type YYTYPE.
m4_define([b4_symbol_variant],
[m4_pushdef([b4_dollar_dollar],
            [$2.$3< $][3 > (m4_shift3($@))])dnl
  switch ($1)
    {
b4_type_foreach([b4_type_action_])[]dnl
      default:
        break;
    }
m4_popdef([b4_dollar_dollar])dnl
])


# b4_variant_types(SYMBOL-NUMS)
# -----------------------------
# List types used in variant.
m4_define([b4_variant_types],
[b4_symbol_if([$1], [has_type],
[
m4_map([      b4_symbol_tag_comment], [$@])dnl
      , b4_symbol([$1], [type])
])])


# b4_variant_includes
# -------------------
# The needed includes for variants support.
m4_define([b4_variant_includes],
[[
# include <variant>
]])

# b4_variant_define
# -----------------
# A helper function to support the "$<...>$ = foo" syntax.
m4_define([b4_variant_define],
[[  template <typename T, typename V>
  T& variant_setter (V& v)
  {
    // if variant has the wanted type already, keep the value
    if (auto r = std::get_if<T> (&v))
      return *r;
    // otherwise change variant to the wanted type, construct default value
    v.template emplace<T> ();
    return std::get<T> (v);
  }
]])


## -------------------------- ##
## Adjustments for variants.  ##
## -------------------------- ##


# b4_value_type_declare
# ---------------------
# Declare semantic_type.
# std::monostate as the first type both makes the declaration here easier
# to write and allows for default-initialization ($$ = { }) in the parser.
m4_define([b4_value_type_declare],
[[  /// Symbol semantic values.
    typedef std::variant<std::monostate]b4_type_foreach([b4_variant_types])[
    > semantic_type;][]dnl
])


# How the semantic value is extracted when using variants.

# b4_symbol_value(VAL, [TYPE])
# ----------------------------
m4_define([b4_symbol_value],
[m4_ifval([$2],
          [std::get<$2> ($1)],
          [$1])])
m4_define([b4_symbol_value_lhs],
[m4_ifval([$2],
          b4_namespace_ref[::variant_setter<$2> ($1)],
          [$1])])


## ------------- ##
## make_SYMBOL.  ##
## ------------- ##


# b4_symbol_constructor_declare_(SYMBOL-NUMBER)
# ---------------------------------------------
# Declare the overloaded version of make_symbol for the (common) type of
# these SYMBOL-NUMBERS.  Use at class-level.
m4_define([b4_symbol_constructor_declare_],
[b4_symbol_if([$1], [is_token], [b4_symbol_if([$1], [has_id],
[    static inline
    symbol_type
    make_[]b4_symbol_([$1], [id]) (dnl
b4_join(b4_symbol_if([$1], [has_type],
                     [b4_symbol([$1], [type]) v]),
        b4_locations_if([const location_type& l])));

])])])


# b4_symbol_constructor_declare
# -----------------------------
# Declare symbol constructors for all the value types.
# Use at class-level.
m4_define([b4_symbol_constructor_declare],
[    // Symbol constructors declarations.
    static inline
    symbol_type
    make_token (token_type type, b4_locations_if([const location_type& l, ])semantic_type&& v = { });

b4_symbol_foreach([b4_symbol_constructor_declare_])])



# b4_symbol_constructor_define_(SYMBOL-NUMBER)
# --------------------------------------------
# Define symbol constructor for this SYMBOL-NUMBER.
m4_define([b4_symbol_constructor_define_],
[b4_symbol_if([$1], [is_token], [b4_symbol_if([$1], [has_id],
[  b4_parser_class_name::symbol_type
  b4_parser_class_name::make_[]b4_symbol_([$1], [id]) (dnl
b4_join(b4_symbol_if([$1], [has_type],
                     [b4_symbol([$1], [type]) v]),
        b4_locations_if([const location_type& l])))
  {
    return symbol_type (b4_join([token::b4_symbol([$1], [id])],
                                b4_symbol_if([$1], [has_type], [std::move (v)]),
                                b4_locations_if([l])));
  }

])])])


# b4_basic_symbol_constructor_declare
# -----------------------------------
# Generate a constructor declaration for basic_symbol from given type.
m4_define([b4_basic_symbol_constructor_declare],
[[
      basic_symbol (]b4_join(
              [typename Base::kind_type t],
              b4_locations_if([const location_type& l]))[)
        : Base (t)]b4_locations_if([
        , location (l)])[
      {}

      template <typename T>
      basic_symbol (]b4_join(
              [typename Base::kind_type t],
              [T&& v],
              b4_locations_if([const location_type& l]))[)
        : Base (t)
        , value (std::forward <T> (v))]b4_locations_if([
        , location (l)])[
      {}]
])

# b4_symbol_constructor_define
# ----------------------------
# Define the overloaded versions of make_symbol for all the value types.
m4_define([b4_symbol_constructor_define],
[  // Implementation of make_symbol for each symbol type.

  b4_parser_class_name::symbol_type
  b4_parser_class_name::make_token (token_type type, b4_locations_if([const location_type& l, ])semantic_type&& v)
  {
    return symbol_type (type, std::move (v)b4_locations_if([, l]));
  }

b4_symbol_foreach([b4_symbol_constructor_define_])])
