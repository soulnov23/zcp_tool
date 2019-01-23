#ifndef __ATOMIC_COUNT_GCC_X86_H__
#define __ATOMIC_COUNT_GCC_X86_H__

// atomic_count for g++ on 486+/AMD64

class atomic_count
{
public:

    explicit atomic_count(long v = 0) : value_( static_cast<int>(v)) {}

    void operator++()
    {
        __asm__
        (
            "lock\n\t"
            "incl %0":
            "+m"( value_ ):     // output (%0)
            :                   // inputs
            "cc"                // clobbers
        );
    }

    long operator--()
    {
        return atomic_exchange_and_add(&value_, -1) - 1;
    }

    operator long() const
    {
        return atomic_exchange_and_add(&value_, 0);
    }

private:

    atomic_count(atomic_count const &);
    atomic_count &operator=(atomic_count const &);

    mutable int value_;

private:

    static int atomic_exchange_and_add(int *pw, int dv)
    {
        // int r = *pw;
        // *pw += dv;
        // return r;

        int r;

        __asm__ __volatile__
        (
            "lock\n\t"
            "xadd %1, %0":
            "+m"( *pw ), "=r"( r ):     // outputs (%0, %1)
            "1"( dv ):                  // inputs (%2 == %1)
            "memory", "cc"              // clobbers
        );

        return r;
    }
};

#endif
