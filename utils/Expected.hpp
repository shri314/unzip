#pragma once

#include <type_traits>
#include <utility>
#include <cassert>

namespace utils
{

template<class ErrT>
class UnExpected
{
public:
    template<
        class ErrCT = ErrT,
        class       = std::enable_if_t<
                  !std::is_same_v<std::remove_cv_t<std::remove_reference_t<ErrCT>>, UnExpected>
                  && std::is_constructible_v<ErrT, ErrCT>>>
    UnExpected(ErrCT&& Err)
      : m_Err(std::forward<ErrCT>(Err))
    {
    }

    UnExpected(const UnExpected&) = default;
    UnExpected(UnExpected&&)      = default;

    UnExpected&
    operator=(const UnExpected& Rhs)
        = default;

    UnExpected&
    operator=(UnExpected&&)
        = default;

    ~UnExpected() = default;

    ErrT&
    Error() &
    {
        return m_Err;
    }

    const ErrT&
    Error() const&
    {
        return m_Err;
    }

    ErrT&&
    Error() &&
    {
        return std::move(m_Err);
    }

    const ErrT&&
    Error() const&&
    {
        return std::move(m_Err);
    }

    friend void
    swap(UnExpected& A, UnExpected& B)
    {
        using std::swap;
        swap(A.m_Err, B.m_Err);
    }

private:
    ErrT m_Err;
};

template<class ErrT>
UnExpected(ErrT) -> UnExpected<ErrT>;

struct UnExpect_t
{
};

template<class ValT, class ErrT>
class Expected
{
    union Store
    {
        ValT m_Val;
        ErrT m_Err;

        Store() {}

        ~Store() {}
    };

    template<class... ArgTs>
    void
    ConstructT(ArgTs&&... Args)
    {
        new (std::addressof(m_Store.m_Val)) ValT(std::forward<ArgTs>(Args)...);
    }

    template<class... ArgTs>
    void
    ConstructE(ArgTs&&... Args)
    {
        new (std::addressof(m_Store.m_Err)) ErrT(std::forward<ArgTs>(Args)...);
    }

    void
    DestructT()
    {
        m_Store.m_Val.~ValT();
    }

    void
    DestructE()
    {
        m_Store.m_Err.~ErrT();
    }

public:
    bool
    HasError() const
    {
        return !m_HasVal;
    }

    bool
    HasValue() const
    {
        return m_HasVal;
    }

    Expected()
      : m_HasVal{ true }
    {
        ConstructT();
    }

    template<
        class ValCT = ValT,
        class       = std::enable_if_t<
                  !std::is_same_v<std::remove_cv_t<std::remove_reference_t<ValCT>>, Expected>
                  && std::is_constructible_v<ValT, ValCT>>>
    Expected(ValCT&& Val)
      : m_HasVal{ true }
    {
        ConstructT(std::forward<ValCT>(Val));
    }

    template<class ErrCT, class = std::enable_if_t<std::is_constructible_v<ErrT, ErrCT>>>
    Expected(UnExpect_t, ErrCT&& Err)
      : m_HasVal{ false }
    {
        ConstructE(std::forward<ErrCT>(Err));
    }

    ~Expected()
    {
        if (HasValue())
        {
            DestructT();
        }
        else
        {
            DestructE();
        }
    }

    template<class ErrCT, class = std::enable_if_t<std::is_constructible_v<ErrT, const ErrCT&>>>
    Expected(const UnExpected<ErrCT>& Rhs)
      : Expected{ UnExpect_t{}, Rhs.Error() }
    {
    }

    template<class ErrCT, class = std::enable_if_t<std::is_constructible_v<ErrT, ErrCT>>>
    Expected(UnExpected<ErrCT>&& Rhs)
      : Expected{ UnExpect_t{}, std::move(Rhs).Error() }
    {
    }

    Expected(const Expected& Rhs)
      : m_HasVal(Rhs.m_HasVal)
    {
        if (Rhs.HasValue())
        {
            ConstructT(Rhs.Value());
        }
        else
        {
            ConstructE(Rhs.Error());
        }
    }

    Expected(Expected&& Rhs) noexcept(
        std::is_nothrow_move_constructible_v<ValT> && std::is_nothrow_move_constructible_v<ErrT>
    )
      : m_HasVal(Rhs.m_HasVal)
    {
        if (Rhs.HasValue())
        {
            ConstructT(std::move(Rhs).Value());
        }
        else
        {
            ConstructE(std::move(Rhs).Error());
        }
    }

    Expected&
    operator=(const Expected& Rhs)
    {
        using std::swap;

        Expected tmp{ Rhs };
        swap(*this, tmp);

        return *this;
    }

    Expected&
    operator=(Expected&& Rhs)
    {
        using std::swap;

        Expected tmp{ std::move(Rhs) };
        swap(*this, tmp);

        return *this;
    }

    template<class ErrCT, class = std::enable_if_t<std::is_constructible_v<ErrT, const ErrCT&>>>
    Expected&
    operator=(const UnExpected<ErrCT>& Rhs)
    {
        using std::swap;

        Expected tmp{ UnExpect_t{}, Rhs.Error() };
        swap(*this, tmp);

        return *this;
    }

    template<class ErrCT, class = std::enable_if_t<std::is_constructible_v<ErrT, ErrCT>>>
    Expected&
    operator=(UnExpected<ErrCT>&& Rhs)
    {
        using std::swap;

        Expected tmp{ UnExpect_t{}, std::move(Rhs).Error() };
        swap(*this, tmp);

        return *this;
    }

    ErrT&
    Error() &
    {
        assert(!HasValue());
        return m_Store.m_Err;
    }

    const ErrT&
    Error() const&
    {
        assert(!HasValue());
        return m_Store.m_Err;
    }

    ErrT&&
    Error() &&
    {
        assert(!HasValue());
        return std::move(m_Store.m_Err);
    }

    const ErrT&&
    Error() const&&
    {
        assert(!HasValue());
        return std::move(m_Store.m_Err);
    }

    template<class ErrCT>
    ErrT
    ErrorOr(ErrCT&& Def) const&
    {
        if (HasValue())
        {
            return m_Store.m_Err;
        }
        else
        {
            return std::forward<ErrCT>(Def);
        }
    }

    template<class ErrCT>
    ErrT
    ErrorOr(ErrCT&& Def) &&
    {
        if (HasValue())
        {
            return std::move(m_Store.m_Err);
        }
        else
        {
            return std::forward<ErrCT>(Def);
        }
    }

    ValT&
    Value() &
    {
        assert(HasValue());
        return m_Store.m_Val;
    }

    const ValT&
    Value() const&
    {
        assert(HasValue());
        return m_Store.m_Val;
    }

    ValT&&
    Value() &&
    {
        assert(HasValue());
        return std::move(m_Store.m_Val);
    }

    const ValT&&
    Value() const&&
    {
        assert(HasValue());
        return std::move(m_Store.m_Val);
    }

    template<class ValCT>
    ValT
    ValueOr(ValCT&& Def) const&
    {
        if (HasValue())
        {
            return m_Store.m_Val;
        }
        else
        {
            return std::forward<ValCT>(Def);
        }
    }

    template<class ValCT>
    ValT
    ValueOr(ValCT&& Def) &&
    {
        if (HasValue())
        {
            return std::move(m_Store.m_Val);
        }
        else
        {
            return std::forward<ValCT>(Def);
        }
    }

    friend void
    swap(Expected& A, Expected& B)
    {
        using std::swap;

        if (A.HasValue() == B.HasValue())
        {
            if (A.HasValue())
            {
                swap(A.Value(), B.Value());
            }
            else
            {
                swap(A.Error(), B.Error());
            }
        }
        else
        {
            if (A.HasValue())
            {
                // NOTE: if both ErrT's or ValT's move constructors throw, this
                //       function cannot have a safe implementation.
                //
                // FIXME: actually, if either ErrT's or ValT's move constructors
                //        throw - but not both, that case could be handled with
                //        more code and with appropriate exception handling,
                //        but this code below instead avoids that complexity by
                //        just demanding that both ValT's and ErrT's move
                //        constructors do not throw. (that's simpler)

                static_assert(std::is_nothrow_move_constructible_v<ErrT>);
                static_assert(std::is_nothrow_move_constructible_v<ValT>);

                ValT t_{ std::move(A).Value() };
                A.DestructT();

                A.ConstructE(std::move(B).Error());
                B.DestructE();

                B.ConstructT(std::move(t_));

                A.m_HasVal = false;
                B.m_HasVal = true;
            }
            else
            {
                swap(A, B);
            }
        }
    }

private:
    Store m_Store;
    bool m_HasVal = true;
};

template<class ErrT>
class Expected<void, ErrT>
{
    union Store
    {
        ErrT m_Err;
    };

    template<class... ArgTs>
    void
    ConstructE(ArgTs&&... Args)
    {
        new (std::addressof(m_Store.m_Err)) ErrT(std::forward<ArgTs>(Args)...);
    }

    void
    DestructE()
    {
        m_Store.m_Err.~ErrT();
    }

public:
    bool
    HasError() const
    {
        return !m_HasVal;
    }

    bool
    HasValue() const
    {
        return m_HasVal;
    }

    Expected()
      : m_HasVal{ true }
    {
    }

    template<class ErrCT = ErrT>
    Expected(UnExpect_t, ErrCT&& Err)
      : m_HasVal{ false }
    {
        ConstructE(std::forward<ErrCT>(Err));
    }

    ~Expected()
    {
        if (!HasError())
        {
            DestructE();
        }
    }

    template<class ErrCT, class = std::enable_if_t<std::is_constructible_v<ErrT, const ErrCT&>>>
    Expected(const UnExpected<ErrCT>& Rhs)
      : Expected{ UnExpect_t{}, Rhs.Error() }
    {
    }

    template<class ErrCT, class = std::enable_if_t<std::is_constructible_v<ErrT, ErrCT>>>
    Expected(UnExpected<ErrCT>&& Rhs)
      : Expected{ UnExpect_t{}, std::move(Rhs).Error() }
    {
    }

    Expected(const Expected& Rhs)
      : m_HasVal(Rhs.m_HasVal)
    {
        if (!Rhs.HasValue())
        {
            ConstructE(Rhs.Error());
        }
    }

    Expected(Expected&& Rhs) noexcept(std::is_nothrow_move_constructible_v<ErrT>)
      : m_HasVal(Rhs.m_HasVal)
    {
        if (!Rhs.HasValue())
        {
            ConstructE(std::move(Rhs).Error());
        }
    }

    Expected&
    operator=(const Expected& Rhs)
    {
        using std::swap;

        Expected tmp = Rhs;
        swap(*this, tmp);

        return *this;
    }

    Expected&
    operator=(Expected&& Rhs)
    {
        using std::swap;

        Expected tmp = std::move(Rhs);
        swap(*this, tmp);

        return *this;
    }

    template<class ErrCT, class = std::enable_if_t<std::is_constructible_v<ErrT, const ErrCT&>>>
    Expected&
    operator=(const UnExpected<ErrCT>& Rhs)
    {
        using std::swap;

        Expected tmp{ UnExpect_t{}, Rhs.Error() };
        swap(*this, tmp);

        return *this;
    }

    template<class ErrCT, class = std::enable_if_t<std::is_constructible_v<ErrT, ErrCT>>>
    Expected&
    operator=(UnExpected<ErrCT>&& Rhs)
    {
        using std::swap;

        Expected tmp{ UnExpect_t{}, std::move(Rhs).Error() };
        swap(*this, tmp);

        return *this;
    }

    ErrT&
    Error() &
    {
        assert(!HasValue());
        return m_Store.m_Err;
    }

    const ErrT&
    Error() const&
    {
        assert(!HasValue());
        return m_Store.m_Err;
    }

    ErrT&&
    Error() &&
    {
        assert(!HasValue());
        return std::move(m_Store.m_Err);
    }

    const ErrT&&
    Error() const&&
    {
        assert(!HasValue());
        return std::move(m_Store.m_Err);
    }

    template<class ErrCT>
    ErrT
    ErrorOr(ErrCT&& Def) const&
    {
        if (HasValue())
        {
            return m_Store.m_Err;
        }
        else
        {
            return std::forward<ErrCT>(Def);
        }
    }

    template<class ErrCT>
    ErrT
    ErrorOr(ErrCT&& Def) &&
    {
        if (HasValue())
        {
            return std::move(m_Store.m_Err);
        }
        else
        {
            return std::forward<ErrCT>(Def);
        }
    }

    void
    Value() const&
    {
        assert(HasValue());
    }

    void
    Value() &&
    {
        assert(HasValue());
    }

    friend void
    swap(Expected& A, Expected& B)
    {
        using std::swap;

        if (A.HasValue() == B.HasValue())
        {
            if (!A.HasValue())
            {
                swap(A.Error(), B.Error());
            }
        }
        else
        {
            if (A.HasValue())
            {
                A.ConstructE(std::move(B).Error());
                B.DestructE();

                A.m_HasVal = false;
                B.m_HasVal = true;
            }
            else
            {
                swap(A, B);
            }
        }
    }

private:
    Store m_Store;
    bool m_HasVal = true;
};

}
