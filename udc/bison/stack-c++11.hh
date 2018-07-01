# C++11 skeleton for Bison

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

m4_pushdef([b4_copyright_years],
           [2002-2015])

b4_percent_define_default([[api.stack.container]], [[std::vector]])
# b4_stack_define
# ---------------
m4_define([b4_stack_define],
[[  // reserve, can be overriden for other containers
  template <class S> void stack_prepare (S &) { }
  template <class T> void stack_prepare (std::vector <T> &s) { s.reserve (200); }

  template <class T, class S = ]]b4_percent_define_get([[api.stack.container]])[[<T> >
  class stack
  {
  public:
    // Hide our reversed order.
    typedef typename S::reverse_iterator iterator;
    typedef typename S::const_reverse_iterator const_iterator;

    stack ()
    {
      stack_prepare (seq_);
    }

    inline
    T&
    operator[] (size_t i)
    {
      return seq_[seq_.size () - 1 - i];
    }

    inline
    const T&
    operator[] (size_t i) const
    {
      return seq_[seq_.size () - 1 - i];
    }

    inline
    void
    push (T&& t)
    {
      seq_.emplace_back (std::move (t));
    }

    inline
    void
    pop (size_t n = 1)
    {
      for (; n; --n)
        seq_.pop_back ();
    }

    void
    clear ()
    {
      seq_.clear ();
    }

    inline
    typename S::size_type
    size () const
    {
      return seq_.size ();
    }

    inline
    const_iterator
    begin () const
    {
      return seq_.rbegin ();
    }

    inline
    const_iterator
    end () const
    {
      return seq_.rend ();
    }

  private:
    stack (const stack&) = delete;
    stack& operator= (const stack&) = delete;
    /// The wrapped container.
    S seq_;
  };

  /// Present a slice of the top of a stack.
  template <class T, class S = stack<T> >
  class slice
  {
  public:
    slice (const S& stack, size_t range)
      : stack_ (stack)
      , range_ (range)
    {}

    inline
    const T&
    operator [] (size_t i) const
    {
      return stack_[range_ - i];
    }

  private:
    const S& stack_;
    size_t range_;
  };
]])

b4_defines_if(
[b4_output_begin([b4_dir_prefix[]b4_percent_define_get([[extra_header_prefix]])stack.hh])
b4_copyright([Stack handling for Bison parsers in C++11])[

/**
 ** \file ]b4_dir_prefix[]b4_percent_define_get([[extra_header_prefix]])[stack.hh
 ** Define the ]b4_namespace_ref[::stack class.
 */

]b4_cpp_guard_open([b4_dir_prefix[]b4_percent_define_get([[extra_header_prefix]])stack.hh])[

# include <vector>

]b4_namespace_open[
]b4_stack_define[
]b4_namespace_close[

]b4_cpp_guard_close([b4_dir_prefix[]b4_percent_define_get([[extra_header_prefix]])stack.hh])
b4_output_end()
])

m4_popdef([b4_copyright_years])
