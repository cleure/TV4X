
#if defined(_MSC_VER)
    #define TVXX_INLINE __forceinline
    #define TVXX_RESTRICT
#elif defined(__GNUC__)
    #define TVXX_INLINE __inline
    #define TVXX_RESTRICT __restrict__
#elif defined(__clang__)
    #define TVXX_INLINE __inline
    #define TVXX_RESTRICT __restrict__
#else
    #define TVXX_INLINE
    #define TVXX_RESTRICT
#endif

/* Input/Output Data Types */
#ifndef tv4x_in_type
    #define tv4x_in_type uint32_t
#endif

#ifndef tv4x_out_type
    #define tv4x_out_type uint32_t
#endif

/* Input/Output Data Types */
#ifndef tv2x_in_type
    #define tv2x_in_type uint32_t
#endif

#ifndef tv2x_out_type
    #define tv2x_out_type uint32_t
#endif
