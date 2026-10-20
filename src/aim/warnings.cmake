if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  list(APPEND PROJECT_WARNING_FLAGS
    -Wall                # Enables most warnings.
    -Wextra              # Enables an extra set of warnings.

    -Wno-unused-function
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-sign-compare
    -Wno-unused-but-set-variable

    -Wcast-align         # Pointer casts which increase alignment.
    -Wcast-qual          # A pointer is cast to remove a type qualifier, or add an unsafe one.
    -Wformat=2           # printf/scanf/strftime/strfmon format string anomalies.
    -Wnon-virtual-dtor   # Non-virtual destructors are found.
    -Woverloaded-virtual # Overloaded virtual function names.
    # -Wshadow             # One variable shadows another.

    # -Wconversion         # Implicit type conversions that may change a value.
    # -Wsign-conversion    # Implicit conversions between signed and unsigned integers.
    # -Wswitch-enum        # A switch statement has an index of enumerated type and lacks a case.
    #-Wundef              # An undefined identifier is evaluated in an #if directive.
    #-Wunused             # Enable all -Wunused- warnings.
    )
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND PROJECT_WARNING_FLAGS
      -Wno-deprecated-attributes
      -Wno-unused-private-field
    )
  endif ()
  # Enable additional warnings depending on the compiler and compiler version in use.
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    list(APPEND PROJECT_WARNING_FLAGS
      -Wdisabled-optimization       # GCC’s optimizers are unable to handle the code effectively.
      # -Weffc++                      # Warnings related to guidelines from Scott Meyers’ Effective C++ books.
      -Wlogical-op                  # Warn when a logical operator is always evaluating to true or false.
      # -Wsign-promo                  # Overload resolution chooses a promotion from unsigned to a signed type.
      # -Wswitch-default              # A switch statement does not have a default case.
      # -Wdouble-promotion          # Warn about implicit conversions from "float" to "double".
      -Wdate-time                 # Warn when encountering macros that might prevent bit-wise-identical compilations.
      # -Wsuggest-final-methods     # Virtual methods that could be declared final or in an anonymous namespace.
      # -Wsuggest-final-types       # Types with virtual methods that can be declared final or in an anonymous namespace.
      -Wduplicated-cond           # Warn about duplicated conditions in an if-else-if chain.
      -Wmisleading-indentation    # Warn when indentation does not reflect the block structure.
      -Wnull-dereference          # Dereferencing a pointer may lead to undefined behavior.
      -Walloca                    # Warn on any usage of alloca in the code.
      # -Wduplicated-branches       # Warn about duplicated branches in if-else statements.
      -Wunsafe-loop-optimizations # The loop cannot be optimized because the compiler cannot assume anything.
      -Wsuggest-override          # Overriding virtual functions that are not marked with the override keyword.
      # -Warith-conversion          # Stricter implicit conversion warnings in arithmetic operations.
      # -Wdouble-promotion            # Warn about implicit conversions from "float" to "double".
      -Wnull-dereference            # Dereferencing a pointer may lead to erroneous or undefined behavior.
      )
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  list(APPEND PROJECT_WARNING_FLAGS
    /permissive- # Specify standards conformance mode to the compiler.
    /W4          # Enable level 4 warnings.
    /w14062      # Enumerator 'identifier' in a switch of enum 'enumeration' is not handled.
    /w14242      # The types are different, possible loss of data. The compiler makes the conversion.
    /w14254      # A larger bit field was assigned to a smaller bit field, possible loss of data.
    /w14263      # Member function does not override any base class virtual member function.
    /w14265      # 'class': class has virtual functions, but destructor is not virtual.
    /w14287      # 'operator': unsigned/negative constant mismatch.
    /w14289      # Loop control variable is used outside the for-loop scope.
    /w14296      # 'operator': expression is always false.
    /w14311      # 'variable' : pointer truncation from 'type' to 'type'.
    /w14545      # Expression before comma evaluates to a function which is missing an argument list.
    /w14546      # Function call before comma missing argument list.
    /w14547      # Operator before comma has no effect; expected operator with side-effect.
    /w14549      # Operator before comma has no effect; did you intend 'operator2'?
    /w14555      # Expression has no effect; expected expression with side-effect.
    /w14619      # #pragma warning: there is no warning number 'number'.
    /w14640      # 'instance': construction of local static object is not thread-safe.
    /w14826      # Conversion from 'type1' to 'type2' is sign-extended.
    /w14905      # Wide string literal cast to 'LPSTR'.
    /w14906      # String literal cast to 'LPWSTR'.
    /w14928      # Illegal copy-initialization; applied more than one user-defined conversion.

    /wd4100
    /wd4091
    /wd4127
    /wd4267
    /wd4244
    /wd4458
    /wd4505
    /wd4702
    /wd4324
    /wd4305
    /wd4242
    /wd4185
    /wd4189
    /wd4457
    /wd4456
    /wd4099 # Struct -> Class fwd decl
    /wd4018

    # /we4062 treats missing enum cases as errors but allows default
    /we4062

    # switch fallthrough
    /we4670
    /we5262
    )
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  list(APPEND PROJECT_WARNING_FLAGS -Werror)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  list(APPEND PROJECT_WARNING_FLAGS /WX)
endif ()
