//
// Created by max on 8/17/20.
//

#ifndef IOC_WARNINGS_H
#define IOC_WARNINGS_H
#if defined(_MSC_VER)
	#define DISABLE_WARNING_PUSH           __pragma(warning( push ))
    #define DISABLE_WARNING_POP            __pragma(warning( pop ))
    #define DISABLE_WARNING(warningNumber) __pragma(warning( disable : warningNumber ))

	#define GCC_DISABLE_WARNING_PUSH
	#define GCC_DISABLE_WARNING_POP
	#define GCC_DISABLE_WARNING(warningName)

	#define CLANG_DISABLE_WARNING_PUSH
	#define CLANG_DISABLE_WARNING_POP
	#define CLANG_DISABLE_WARNING(warningName)

	#define MSVC_DISABLE_WARNING_PUSH           DISABLE_WARNING_PUSH
	#define MSVC_DISABLE_WARNING_POP            DISABLE_WARNING_POP
	#define MSVC_DISABLE_WARNING(warningNumber) DISABLE_WARNING(warningNumber)
#elif defined(__GNUC__)
	#define DO_PRAGMA(X) _Pragma(#X)
	#define DISABLE_WARNING_PUSH           DO_PRAGMA(GCC diagnostic push)
	#define DISABLE_WARNING_POP            DO_PRAGMA(GCC diagnostic pop)
	#define DISABLE_WARNING(warningName)   DO_PRAGMA(GCC diagnostic ignored #warningName)

	#define GCC_DISABLE_WARNING_PUSH        DISABLE_WARNING_PUSH
	#define GCC_DISABLE_WARNING_POP         DISABLE_WARNING_POP
	#define GCC_DISABLE_WARNING(warningName)DISABLE_WARNING(warningName)

	#define CLANG_DISABLE_WARNING_PUSH
	#define CLANG_DISABLE_WARNING_POP
	#define CLANG_DISABLE_WARNING(warningName)

	#define MSVC_DISABLE_WARNING_PUSH
	#define MSVC_DISABLE_WARNING_POP
	#define MSVC_DISABLE_WARNING(warningNumber)
#elif defined(__clang__)
	#define DO_PRAGMA(X) _Pragma(#X)
	#define DISABLE_WARNING_PUSH           DO_PRAGMA(clang diagnostic push)
	#define DISABLE_WARNING_POP            DO_PRAGMA(clang diagnostic pop)
	#define DISABLE_WARNING(warningName)   DO_PRAGMA(clang diagnostic ignored #warningName)

	#define GCC_DISABLE_WARNING_PUSH
	#define GCC_DISABLE_WARNING_POP
	#define GCC_DISABLE_WARNING(warningName)

	#define CLANG_DISABLE_WARNING_PUSH          DISABLE_WARNING_PUSH
	#define CLANG_DISABLE_WARNING_POP           DISABLE_WARNING_POP
	#define CLANG_DISABLE_WARNING(warningName)  DISABLE_WARNING(warningName)

	#define MSVC_DISABLE_WARNING_PUSH
	#define MSVC_DISABLE_WARNING_POP
	#define MSVC_DISABLE_WARNING(warningNumber)
#endif
#endif //IOC_WARNINGS_H
