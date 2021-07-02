#ifndef WOTH_WGE_ERROR_EXCEPTIONS_HPP
#define WOTH_WGE_ERROR_EXCEPTIONS_HPP

#include <exception>

namespace wge {
template <typename E>
class bad_expected_access;

template <>
class bad_expected_access<void> : public std::exception {
public:
    explicit bad_expected_access() : std::exception() {}
};

template <class E>
class bad_expected_access : public bad_expected_access<void> {
public:
    using error_type = E;

    explicit bad_expected_access(error_type e) : m_value(e) {}
    virtual const char* what() const noexcept override { return "expected access violation"; }

    auto error() & -> error_type& { return m_value; }
    auto error() const& -> const error_type& { return m_value; }
    auto error() && -> error_type&& { return std::move(m_value); }
    auto error() const&& -> const error_type&& { return std::move(m_value); }

private:
    error_type m_value;  // exposition only
};
}  // namespace wge

#endif  // WOTH_WGE_ERROR_EXCEPTIONS_HPP
