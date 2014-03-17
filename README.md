game-in-a-box
=============

First attempt at doing a reaaaaaly simple fps multiplayer game.

I failed :-(

## Dead pile of clay

I failed because I got lost in the details. I did not iterate.
The parable from the book "Art & Fear" by David Bayles explains the problem.

> The ceramics teacher announced on opening day that he was dividing the class into two groups. All those on the left side of the studio, he said, would be graded solely on the quantity of work they produced, all those on the right solely on its quality.

> His procedure was simple: on the final day of class he would bring in his bathroom scales and weigh the work of the “quantity” group: fifty pound of pots rated an “A”, forty pounds a “B”, and so on. Those being graded on “quality”, however, needed to produce only one pot”albeit a perfect one”to get an “A”.

> Well, came grading time and a curious fact emerged: the works of highest quality were all produced by the group being graded for quantity. It seems that while the “quantity” group was busily churning out piles of work”and learning from their mistakes”the “quality” group had sat theorizing about perfection, and in the end had little more to show for their efforts than grandiose theories and a pile of dead clay.

## Lessons

You don't spend 1300 hours coding over 1 year and learn nothing. I realised that I wasn't actually writing a game in 1 year. I actually spent it learning the plumbing of C++, C++11, C++1y, the standard library, boost, assembler, compilers, multi platform support, continous integration, static analysis and my love hatred of unit tests.

### What broke the camel's back

I found myself implementing physics calculations so that game state calculation could become a massive parallel process and realised that:
a) I was no longer having fun, it was a chore
b) There are already physics engines that I can just use instead
c) It's been a year and I don't even have something I can run that looks remotly like a game
sure, the physics engine was using POD phyics data units with template based unit checking (but not boost::units as that's not POD). But it wasn't a game!

### Current status

- The master branch is pretty old.
- feature-gamestate-parallel is the most recent code branch (only builds GCC I think).
- feature-gamestate-parallel-boostunits was where I trued to use boost:units for my unit system only to find out that the unit classes were not POD data types, so I abandoned it.
- feature-gamestate-parallel-vector was where I went off to write my own SSE/Neon accelerated vector class library using intrinsics, only to come full circle to write a c++ only version structures so that modern compilers would vectorise the code anyway. The only reason every man and his dog made a SIMD/SSE/Neon/Cell/Altivec/vector3/vector4/quanternion class was because the xbox and ps3 compilers were so crap they wouldn't do it for you (http://www.altdevblogaday.com/2011/12/24/beyond-intrinsics-to-code-idiom/)
- feature-physics was just me importing a snapshot of the bullet physics sdk.


### Fails

- No game
- Didn't even write the delta based lossy fps network compression algorithms I was writing the game for.

### Learnt

- Iterate
- Full understanding of all delta based fps game transports, and various compression algorithms.
- C++
 - new/delete free programming
 - null free programming (boost/std::optional)
 - Don't write your own assembler vector library, and instead trust the compiler
 - Filter, map, reduce and interators in C++ for parallel programming
 - POD and non-POD
 - Casting between different types under -fstrict-aliasing (you don't, use memcpy and trust the compiler to optimise it properly)
 - Bools should only ever be returned, never passed in as arguments
 - still no reflection, so I hacked one together using macros.
- C++11
 - Bjarne Stroustrup: C++11 Style
 - #include <cstdint> // (about bloody time.)
 - argument forwarding (well sorta)
 - my love for unique_ptr
 - PIMPL
 - Lambdas and captures
 - delegating constructors (I really missed that from my C# days)
 - =delete, =default. Yay, no private member hacks, or rewriting the default constructor
 - Move semantics
 - Communicating ownership via the API
 - To avoid floating point maths because it'll drive you insane (d = a*b + c; e=a*b; e+=c; e!=d // argh) 
 - compile time unit system
 - constexpr
 - static_assert
 - Pass by value if it's copied, const reference if it's only referenced
- Unit tests
 - gmock
 - How to avoid writing bad ones
 - How every time they have saved my arse even though I hate writing them
 - How they still don't capture every error in your code
 - When to use EXPECTS when to use ASSERT
 - Comparing floating point
- Tries for autocomplete
- Static analysis
- Valgrind
 - Valgrinding unit tests is awesome
 - C++11's random number implementation uses uninitilised memory on purpose.
- Continous integration builds (travis-ci)
- GCC/Clang/MSVC2013 C++11 and C++1y support.
- github markdown
- bullet physics
- lots of SI units
- qtcreator
- how to crash GCC with bad code :-)



<a href="https://travis-ci.org/JodiTheTigger/game-in-a-box"><img src="https://travis-ci.org/JodiTheTigger/game-in-a-box.png"></a>
