// RUN: %clang_cc1 -std=c++20 -verify %s

// Reproduces a crash when redefining a default template argument in the
// global module fragment of a named module. This should produce a normal
// diagnostic instead of crashing in Module::getFullModuleName.

module;

// Original minimal repro
template <class = int> class foo;  // expected-note {{previous default template argument defined here}}
template <class = void> class foo; // expected-error {{template parameter redefines default argument}}

// --------------- Simple non-module-like variants (still in GMF) ---------------

// Identical default repeated: should be OK (no error)
template <class = int> class Same;
template <class = int> class Same; // (no-diagnostic expected)

// Multi-parameter template: change only one default -> diagnostic for the changed one
template <class T = int, class U = double> class Pair;       // expected-note {{previous default template argument defined here}}
template <class T = int, class U = float> class Pair;        // expected-error {{template parameter redefines default argument}}

// Non-type template parameter default changed
template <int N = 5> class NT;  // expected-note {{previous default template argument defined here}}
template <int N = 7> class NT;  // expected-error {{template parameter redefines default argument}}

// Re-declare with the same non-type default: OK
template <int M = 1> class NTsame;
template <int M = 1> class NTsame; // (no-diagnostic expected)

// --------------- Forward declaration / definition patterns ---------------

// Legal: forward-declare with default, later define without default (allowed)
template <class T = int> class Legal; 
template <class T> class Legal { T value; }; // (no-diagnostic expected)

// Illegal: forward declare with default, then define with a different default
template <class T = int> class Illegal;           // expected-note {{previous default template argument defined here}}
template <class T = void> class Illegal { T v; }; // expected-error {{template parameter redefines default argument}}

// --------------- Slightly more complex combinations ---------------

// Mixed: multiple templates where one redeclaration changes only one template's default
template <class A = int> class A1;        // expected-note {{previous default template argument defined here}}
template <class A = void> class A1;       // expected-error {{template parameter redefines default argument}}

template <class B = int> class B1;        // expected-note {{previous default template argument defined here}}
template <class B = int> class B1;        // identical default, should be OK

// Multiple successive redeclarations: first change should be diagnosed
template <class X = int> class MultiStep;            // expected-note {{previous default template argument defined here}}
template <class X = int> class MultiStep;            // OK
template <class X = long> class MultiStep;           // expected-error {{template parameter redefines default argument}}

// --------------- Edge-ish case: template with both type and non-type parameters ---------------

template <class T = int, int N = 10> class Mixed;    // expected-note {{previous default template argument defined here}}
template <class T = void, int N = 10> class Mixed;   // expected-error {{template parameter redefines default argument}}

// --------------- End of tests for global module fragment ---------------

export module a;